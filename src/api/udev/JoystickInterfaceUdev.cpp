/*
 *  Copyright (C) 2016-2021 Garrett Brown
 *  Copyright (C) 2016-2021 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickInterfaceUdev.h"
#include "JoystickUdev.h"
#include "api/JoystickManager.h"
#include "api/JoystickTypes.h"
#include "log/Log.h"

#include <cstdint>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <utility>

using namespace JOYSTICK;

ButtonMap CJoystickInterfaceUdev::m_buttonMap = {
    std::make_pair("game.controller.default", FeatureVector{
        kodi::addon::JoystickFeature("leftmotor", JOYSTICK_FEATURE_TYPE_MOTOR),
        kodi::addon::JoystickFeature("rightmotor", JOYSTICK_FEATURE_TYPE_MOTOR),
    }),
    std::make_pair("game.controller.ps", FeatureVector{
        kodi::addon::JoystickFeature("strongmotor", JOYSTICK_FEATURE_TYPE_MOTOR),
        kodi::addon::JoystickFeature("weakmotor", JOYSTICK_FEATURE_TYPE_MOTOR),
    }),
};

CJoystickInterfaceUdev::CJoystickInterfaceUdev() :
  m_udev(nullptr),
  m_udev_mon(nullptr)
{
}

EJoystickInterface CJoystickInterfaceUdev::Type() const
{
  return EJoystickInterface::UDEV;
}

bool CJoystickInterfaceUdev::Initialize()
{
  m_udev = udev_new();
  if (!m_udev)
  {
    esyslog("Failed to initialize udev");
    return false;
  }

  m_udev_mon = udev_monitor_new_from_netlink(m_udev, "udev");
  if (!m_udev_mon)
  {
    esyslog("Failed to create udev monitor");
    udev_unref(m_udev);
    return false;
  }

  udev_monitor_filter_add_match_subsystem_devtype(m_udev_mon, "input", nullptr);
  udev_monitor_enable_receiving(m_udev_mon);

  // eventfd used to wake the monitor thread immediately on shutdown rather than relying on a poll timeout
  // A failure returns -1, which polling will ignore.
  m_stopFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (m_stopFd < 0)
    esyslog("Failed to create eventfd for udev monitor (errno=%d)", errno);

  // Watch the monitor for hotplug events so devices are detected immediately
  // instead of waiting for the next periodic scan
  m_bStop = false;
  m_monitorThread = std::thread(&CJoystickInterfaceUdev::MonitorThreadProcess, this);

  return true;
}

void CJoystickInterfaceUdev::Deinitialize()
{
  // Set stop and trigger shutdown event. After monitor thread finishes cleanup all resources
  m_bStop = true;
  if (m_stopFd >= 0)
  {
    const uint64_t one = 1;
    if (write(m_stopFd, &one, sizeof(one)) < 0)
      esyslog("Failed to signal udev monitor stop (errno=%d)", errno);
  }

  if (m_monitorThread.joinable())
    m_monitorThread.join();

  if (m_stopFd >= 0)
  {
    close(m_stopFd);
    m_stopFd = -1;
  }

  if (m_udev_mon)
  {
    udev_monitor_unref(m_udev_mon);
    m_udev_mon = nullptr;
  }

  if (m_udev)
  {
    udev_unref(m_udev);
    m_udev = nullptr;
  }
}

void CJoystickInterfaceUdev::MonitorThreadProcess()
{
  const int fd = udev_monitor_get_fd(m_udev_mon);
  if (fd < 0)
  {
    esyslog("Failed to get udev monitor file descriptor");
    return;
  }

  while (!m_bStop)
  {
    struct pollfd pfds[2] = {};
    pfds[0].fd = fd;
    pfds[0].events = POLLIN;
    pfds[1].fd = m_stopFd;
    pfds[1].events = POLLIN;

    // Block until the monitor has data or shutdown is signalled via the eventfd.
    // If eventfd couldn't be created, use a finite timeout so shutdown can't hang forever.
    const int timeoutMs = (m_stopFd >= 0) ? -1 : 1000;
    const int rc = poll(pfds, 2, timeoutMs);
    if (rc < 0)
    {
      if (errno == EINTR)
        continue;
      esyslog("poll() failed on udev monitor (errno=%d)", errno);
      break;
    }

    // Woken by the stop eventfd, break the loop immediately
    if (pfds[1].revents & POLLIN)
      break;

    if (!(pfds[0].revents & POLLIN))
      continue;

    bool bChanged = false;

    // Drain all queued events
    struct udev_device *dev;
    while ((dev = udev_monitor_receive_device(m_udev_mon)) != nullptr)
    {
      const char *action = udev_device_get_action(dev);
      const char *isJoystick = udev_device_get_property_value(dev, "ID_INPUT_JOYSTICK");

      // We only care about joysticks devices changes.
      if (isJoystick != nullptr && std::strcmp(isJoystick, "1") == 0 &&
          action != nullptr &&
          (std::strcmp(action, "add") == 0 || std::strcmp(action, "remove") == 0))
      {
        bChanged = true;
      }

      udev_device_unref(dev);
    }

    if (bChanged)
    {
      CJoystickManager::Get().SetChanged(true);
      CJoystickManager::Get().TriggerScan();
    }
  }
}

bool CJoystickInterfaceUdev::ScanForJoysticks(JoystickVector& joysticks)
{
  if (!m_udev)
    return false;

  struct udev_enumerate* enumerate = udev_enumerate_new(m_udev);
  if (enumerate == nullptr)
  {
    Deinitialize();
    return false;
  }

  udev_enumerate_add_match_property(enumerate, "ID_INPUT_JOYSTICK", "1");
  udev_enumerate_scan_devices(enumerate);

  struct udev_list_entry* devs = udev_enumerate_get_list_entry(enumerate);
  for (struct udev_list_entry* item = devs; item != nullptr; item = udev_list_entry_get_next(item))
  {
    const char* name = udev_list_entry_get_name(item);
    struct udev_device* dev = udev_device_new_from_syspath(m_udev, name);
    const char*  devnode = udev_device_get_devnode(dev);

    if (devnode != nullptr)
    {
      std::shared_ptr<CJoystickUdev>joystick = std::make_shared<CJoystickUdev>(dev, devnode);
      if (joystick->IsInitialized())
        joysticks.emplace_back(std::move(joystick));
    }

    udev_device_unref(dev);
  }

  udev_enumerate_unref(enumerate);
  return true;
}

const ButtonMap& CJoystickInterfaceUdev::GetButtonMap()
{
  auto& dflt = m_buttonMap["game.controller.default"];
  dflt[CJoystickUdev::MOTOR_STRONG].SetPrimitive(JOYSTICK_MOTOR_PRIMITIVE, kodi::addon::DriverPrimitive::CreateMotor(CJoystickUdev::MOTOR_STRONG));
  dflt[CJoystickUdev::MOTOR_WEAK].SetPrimitive(JOYSTICK_MOTOR_PRIMITIVE, kodi::addon::DriverPrimitive::CreateMotor(CJoystickUdev::MOTOR_WEAK));

  auto& ps = m_buttonMap["game.controller.ps"];
  ps[CJoystickUdev::MOTOR_STRONG].SetPrimitive(JOYSTICK_MOTOR_PRIMITIVE, kodi::addon::DriverPrimitive::CreateMotor(CJoystickUdev::MOTOR_STRONG));
  ps[CJoystickUdev::MOTOR_WEAK].SetPrimitive(JOYSTICK_MOTOR_PRIMITIVE, kodi::addon::DriverPrimitive::CreateMotor(CJoystickUdev::MOTOR_WEAK));

  return m_buttonMap;
}
