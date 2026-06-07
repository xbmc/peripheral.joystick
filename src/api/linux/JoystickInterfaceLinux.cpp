/*
 *  Copyright (C) 2014-2021 Garrett Brown
 *  Copyright (C) 2014-2021 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickInterfaceLinux.h"
#include "JoystickLinux.h"
#include "api/JoystickManager.h"
#include "api/JoystickTypes.h"
#include "log/Log.h"

#include <cstdint>
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace JOYSTICK;

namespace
{
  constexpr const char *DEV_INPUT_DIR = "/dev/input";
}

EJoystickInterface CJoystickInterfaceLinux::Type(void) const
{
  return EJoystickInterface::LINUX;
}

bool CJoystickInterfaceLinux::Initialize(void)
{
  // Hotplug support discovers devices by watching /dev/input/js* files.
  // Watch /dev/input with inotify so that joystick nodes being added or
  // removed trigger an immediate rescan instead of waiting for the next
  // periodic scan.
  m_inotifyFd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
  if (m_inotifyFd < 0)
  {
    esyslog("%s: failed to initialize inotify (errno=%d)", __FUNCTION__, errno);
    return true;
  }

  // Listen for created, deleted, and attribute modifications. There is an issue discovered where a device
  // won't have the correct permissions for opening on creation, so there will be two events, one on creation and
  // one on permission set.
  if (inotify_add_watch(m_inotifyFd, DEV_INPUT_DIR, IN_CREATE | IN_DELETE | IN_ATTRIB) < 0)
  {
    esyslog("%s: failed to watch %s (errno=%d)", __FUNCTION__, DEV_INPUT_DIR, errno);
    close(m_inotifyFd);
    m_inotifyFd = -1;
    return true;
  }

  // eventfd used to wake the monitor thread immediately on shutdown rather than relying on a poll timeout
  // A failure returns -1, which polling will ignore.
  m_stopFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (m_stopFd < 0)
    esyslog("%s: failed to create eventfd (errno=%d)", __FUNCTION__, errno);

  m_bStop = false;
  m_monitorThread = std::thread(&CJoystickInterfaceLinux::MonitorThreadProcess, this);

  return true;
}

void CJoystickInterfaceLinux::Deinitialize(void)
{
  // Set stop and trigger shutdown event. After monitor thread finishes cleanup all resources
  m_bStop = true;
  if (m_stopFd >= 0)
  {
    const uint64_t one = 1;
    if (write(m_stopFd, &one, sizeof(one)) < 0)
      esyslog("%s: failed to signal monitor stop (errno=%d)", __FUNCTION__, errno);
  }

  if (m_monitorThread.joinable())
    m_monitorThread.join();

  if (m_stopFd >= 0)
  {
    close(m_stopFd);
    m_stopFd = -1;
  }

  if (m_inotifyFd >= 0)
  {
    close(m_inotifyFd);
    m_inotifyFd = -1;
  }
}

void CJoystickInterfaceLinux::MonitorThreadProcess()
{
  alignas(struct inotify_event) char buf[4096];

  while (!m_bStop)
  {
    struct pollfd pfds[2] = {};
    pfds[0].fd = m_inotifyFd;
    pfds[0].events = POLLIN;
    pfds[1].fd = m_stopFd;
    pfds[1].events = POLLIN;

    // Block until inotify has data or shutdown is signalled via the eventfd
    // If eventfd couldn't be created, use a finite timeout so shutdown can't hang forever.
    const int timeoutMs = (m_stopFd >= 0) ? -1 : 1000;
    const int rc = poll(pfds, 2, timeoutMs);
    if (rc < 0)
    {
      if (errno == EINTR)
        continue;
      esyslog("%s: poll() failed (errno=%d)", __FUNCTION__, errno);
      break;
    }

    // Woken by the stop eventfd, break the loop immediately
    if (pfds[1].revents & POLLIN)
      break;

    if (!(pfds[0].revents & POLLIN))
      continue;

    bool bChanged = false;

    // Drain all queued events
    ssize_t len;
    while ((len = read(m_inotifyFd, buf, sizeof(buf))) > 0)
    {
      for (char *ptr = buf; ptr < buf + len;)
      {
        const struct inotify_event *event = reinterpret_cast<const struct inotify_event *>(ptr);

        // We only care about joystick device changes. (/dev/input/js* files)
        if (event->len > 0 && std::strncmp(event->name, "js", 2) == 0)
          bChanged = true;

        ptr += sizeof(struct inotify_event) + event->len;
      }
    }

    if (bChanged)
    {
      CJoystickManager::Get().SetChanged(true);
      CJoystickManager::Get().TriggerScan();
    }
  }
}

bool CJoystickInterfaceLinux::ScanForJoysticks(JoystickVector& joysticks)
{
  // TODO: Use udev to grab device names instead of reading /dev/input/js*
  std::string inputDir("/dev/input");
  DIR *pd = opendir(inputDir.c_str());
  if (pd == NULL)
  {
    // Disabled until udev is used to grab device names
    //esyslog("%s: can't open %s (errno=%d)", __FUNCTION__, inputDir.c_str(), errno);
    return false;
  }

  dirent *pDirent;
  while ((pDirent = readdir(pd)) != NULL)
  {
    if (std::string(pDirent->d_name).substr(0, 2) == "js")
    {
      // Found a joystick device
      std::string filename(inputDir + "/" + pDirent->d_name);

      int fd = open(filename.c_str(), O_RDONLY);
      if (fd < 0)
      {
        esyslog("%s: can't open %s (errno=%d)", __FUNCTION__, filename.c_str(), errno);
        continue;
      }

      unsigned char axes      = 0;
      unsigned char buttons   = 0;
      int           version   = 0x000000;
      char          name[128] = { };

      if (ioctl(fd, JSIOCGVERSION, &version) < 0 ||
          ioctl(fd, JSIOCGAXES, &axes)       < 0 ||
          ioctl(fd, JSIOCGBUTTONS, &buttons) < 0 ||
          ioctl(fd, JSIOCGNAME(128), name)   < 0)
      {
        esyslog("%s: failed ioctl() (errno=%d)", __FUNCTION__, errno);
        close(fd);
        continue;
      }

      if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
      {
        esyslog("%s: failed fcntl() (errno=%d)", __FUNCTION__, errno);
        close(fd);
        continue;
      }

      // We don't support the old (0.x) interface
      if (version < 0x010000)
      {
        esyslog("%s: old (0.x) interface is not supported (version=%08x)", __FUNCTION__, version);
        close(fd);
        continue;
      }

      JoystickPtr joystick = JoystickPtr(new CJoystickLinux(fd, filename));
      joystick->SetName(name);
      joystick->SetButtonCount(buttons);
      joystick->SetAxisCount(axes);
      joysticks.push_back(joystick);
    }
  }

  closedir(pd);

  return true;
}
