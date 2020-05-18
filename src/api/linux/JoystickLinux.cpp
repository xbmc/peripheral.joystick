/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickLinux.h"
#include "JoystickInterfaceLinux.h"
#include "api/JoystickTypes.h"
#include "log/Log.h"
#include "utils/CommonMacros.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <sstream>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace JOYSTICK;

#define MAX_AXIS           32767
#define INVALID_FD         -1

CJoystickLinux::CJoystickLinux(int fd, const std::string& strFilename)
 : CJoystick(EJoystickInterface::LINUX),
   m_fd(fd),
   m_strFilename(strFilename)
{
}

void CJoystickLinux::Deinitialize(void)
{
  close(m_fd);
  m_fd = INVALID_FD;
}

bool CJoystickLinux::Equals(const CJoystick* rhs) const
{
  if (rhs == nullptr)
    return false;

  const CJoystickLinux* rhsLinux= dynamic_cast<const CJoystickLinux*>(rhs);
  if (rhsLinux == nullptr)
    return false;

  return m_strFilename == rhsLinux->m_strFilename;
}

bool CJoystickLinux::ScanEvents(void)
{
  js_event joyEvent;

  while (true)
  {
    // Flush the driver queue
    if (read(m_fd, &joyEvent, sizeof(joyEvent)) != sizeof(joyEvent))
    {
      if (errno == EAGAIN)
      {
        // The circular driver queue holds 64 events. If compiling your own driver,
        // you can increment this size bumping up JS_BUFF_SIZE in joystick.h
        break;
      }
      else
      {
        esyslog("%s: failed to read joystick \"%s\" on %s - %d (%s)",
            __FUNCTION__, Name().c_str(), m_strFilename.c_str(), errno, strerror(errno));
        break;
      }
    }

    // The possible values of joystickEvent.type are:
    // JS_EVENT_BUTTON    0x01    // button pressed/released
    // JS_EVENT_AXIS      0x02    // joystick moved
    // JS_EVENT_INIT      0x80    // (flag) initial state of device

    // Ignore initial events, because they mess up the buttons
    switch (joyEvent.type)
    {
    case JS_EVENT_BUTTON:
      SetButtonValue(joyEvent.number, (joyEvent.value ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED));
      break;
    case JS_EVENT_AXIS:
      SetAxisValue(joyEvent.number, joyEvent.value, MAX_AXIS);
      break;
    default:
      break;
    }
  }

  return true;
}
