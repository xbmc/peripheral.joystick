/*
 *      Copyright (C) 2016-2020 Garrett Brown
 *      Copyright (C) 2016-2020 Team Kodi
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "JoystickUdev.h"
#include "api/JoystickTypes.h"
#include "log/Log.h"

#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <limits.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace JOYSTICK;

#ifndef INVALID_FD
  #define INVALID_FD  (-1)
#endif

// From RetroArch
#define test_bit(nr, addr) \
   (((1UL << ((nr) % (sizeof(long) * CHAR_BIT))) & ((addr)[(nr) / (sizeof(long) * CHAR_BIT)])) != 0)

// From RetroArch
#define NBITS(x)  ((((x) - 1) / (sizeof(long) * CHAR_BIT)) + 1)

CJoystickUdev::CJoystickUdev(udev_device* dev, const char* path)
 : CJoystick(EJoystickInterface::UDEV),
   m_dev(dev),
   m_path(path),
   m_deviceNumber(0),
   m_fd(INVALID_FD),
   m_bInitialized(false),
   m_effect(-1),
   m_motors(),
   m_previousMotors()
{
  // Must initialize in the constructor to fill out joystick properties
  Initialize();
}

bool CJoystickUdev::Equals(const CJoystick* rhs) const
{
  const CJoystickUdev* rhsUdev = dynamic_cast<const CJoystickUdev*>(rhs);
  if (rhsUdev == nullptr)
    return false;

  return m_deviceNumber == rhsUdev->m_deviceNumber;
}

bool CJoystickUdev::Initialize(void)
{
  if (!m_bInitialized)
  {
    if (!OpenJoystick())
      return false;

    if (!GetProperties())
      return false;

    if (!CJoystick::Initialize())
      return false;

    m_bInitialized = true;
  }

  return m_bInitialized;
}

void CJoystickUdev::Deinitialize(void)
{
  if (m_fd >= 0)
  {
    close(m_fd);
    m_fd = INVALID_FD;
  }

  CJoystick::Deinitialize();
}

void CJoystickUdev::ProcessEvents(void)
{

  std::array<uint16_t, MOTOR_COUNT> motors;
  std::array<uint16_t, MOTOR_COUNT> previousMotors;

  {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    motors         = m_motors;
    previousMotors = m_previousMotors;
  }

  uint32_t oldStrength = static_cast<uint32_t>(previousMotors[MOTOR_STRONG]) +
                         static_cast<uint32_t>(previousMotors[MOTOR_WEAK]);
  uint32_t newStrength = static_cast<uint32_t>(motors[MOTOR_STRONG]) +
                         static_cast<uint32_t>(motors[MOTOR_WEAK]);

  bool bWasPlaying = (oldStrength > 0);
  bool bIsPlaying = (newStrength > 0);

  if (!bWasPlaying && !bIsPlaying)
  {
    // Nothing to do
  }
  else if (!bWasPlaying && bIsPlaying)
  {
    UpdateMotorState(motors);

    // Play effect
    Play(true);
  }
  else if (bWasPlaying && !bIsPlaying)
  {
    // Stop the effect
    Play(false);
  }
  else
  {
    if (oldStrength != newStrength)
      UpdateMotorState(motors);
  }

  {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_previousMotors = motors;
  }
}

void CJoystickUdev::Play(bool bPlayStop)
{
  struct input_event play = { { } };

  play.type  = EV_FF;
  play.code  = m_effect;
  play.value = bPlayStop;

  if (write(m_fd, &play, sizeof(play)) < (ssize_t)sizeof(play))
    esyslog("[udev]: Failed to play rumble effect %d on \"%s\" - %s", m_effect, Name().c_str(), strerror(errno));

  if (!bPlayStop)
    m_effect = -1;
}

void CJoystickUdev::UpdateMotorState(const std::array<uint16_t, MOTOR_COUNT>& motors)
{
  struct ff_effect e = { };

  e.type                      = FF_RUMBLE;
  e.id                        = m_effect;
  e.u.rumble.strong_magnitude = motors[MOTOR_STRONG];
  e.u.rumble.weak_magnitude   = motors[MOTOR_WEAK];

  if (ioctl(m_fd, EVIOCSFF, &e) < 0)
  {
    esyslog("Failed to set rumble effect %d (0x%04x, 0x%04x) on \"%s\" - %s",
        e.id, e.u.rumble.strong_magnitude, e.u.rumble.weak_magnitude,
        Name().c_str(), strerror(errno));
  }
  else
  {
    m_effect = e.id;
  }
}

bool CJoystickUdev::ScanEvents(void)
{
  input_event events[32];

  if (m_fd < 0)
    return false;

  int len;
  while ((len = read(m_fd, events, sizeof(events))) > 0)
  {
    len /= sizeof(*events);
    for (unsigned int i = 0; i < static_cast<unsigned int>(len); i++)
    {
      const input_event& event = events[i];

      int code = event.code;

      switch (event.type)
      {
        case EV_KEY:
        {
          if (code >= BTN_MISC || (code >= KEY_UP && code <= KEY_DOWN))
          {
            auto it = m_button_bind.find(code);
            if (it != m_button_bind.end())
            {
              const unsigned int buttonIndex = it->second;
              SetButtonValue(buttonIndex, event.value ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
            }
          }
          break;
        }
        case EV_ABS:
        {
          if (code < ABS_MISC)
          {
            auto it = m_axes_bind.find(code);
            if (it != m_axes_bind.end())
            {
              const unsigned int axisIndex = it->second.axisIndex;
              const input_absinfo& info = it->second.axisInfo;

              if (event.value >= 0)
                SetAxisValue(axisIndex, event.value, info.maximum);
              else
                SetAxisValue(axisIndex, event.value, -info.minimum);
            }
          }
          break;
        }
        default:
          break;
      }
    }
  }

  return true;
}

bool CJoystickUdev::OpenJoystick()
{
  unsigned long evbit[NBITS(EV_MAX)]   = { };
  unsigned long keybit[NBITS(KEY_MAX)] = { };
  unsigned long absbit[NBITS(ABS_MAX)] = { };

  m_fd = open(m_path.c_str(), O_RDWR | O_NONBLOCK);

  if (m_fd < 0)
    return false;

  if (ioctl(m_fd, EVIOCGBIT(0, sizeof(evbit)), evbit) < 0)
    return false;

  // Has to at least support EV_KEY interface
  if (!test_bit(EV_KEY, evbit))
    return false;

  return true;
}

bool CJoystickUdev::GetProperties()
{
  unsigned long keybit[NBITS(KEY_MAX)] = { };
  unsigned long absbit[NBITS(ABS_MAX)] = { };
  unsigned long ffbit[NBITS(FF_MAX)]   = { };

  char name[64] = { };
  if (ioctl(m_fd, EVIOCGNAME(sizeof(name)), name) < 0)
  {
    esyslog("[udev]: Failed to get pad name");
    return false;
  }
  SetName(name);

  // Don't worry about unref'ing the parent
  struct udev_device* parent = udev_device_get_parent_with_subsystem_devtype(m_dev, "usb", "usb_device");

  const char* buf;
  if ((buf = udev_device_get_sysattr_value(parent, "idVendor")) != nullptr)
    SetVendorID(strtol(buf, NULL, 16));

  if ((buf = udev_device_get_sysattr_value(parent, "idProduct")) != nullptr)
    SetProductID(strtol(buf, NULL, 16));

  struct stat st;
  if (fstat(m_fd, &st) < 0)
  {
    esyslog("[udev]: Failed to add pad: %s", m_path.c_str());
    return false;
  }
  m_deviceNumber = st.st_rdev;

  if ((ioctl(m_fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), keybit) < 0) ||
      (ioctl(m_fd, EVIOCGBIT(EV_ABS, sizeof(absbit)), absbit) < 0))
  {
    esyslog("[udev]: Failed to add pad: %s", m_path.c_str());
    return false;
  }

  // Go through all possible keycodes, check if they are used, and map them to
  // button/axes/hat indices
  unsigned int buttons = 0;
  for (unsigned int i = KEY_UP; i <= KEY_DOWN; i++)
  {
    if (test_bit(i, keybit))
      m_button_bind[i] = buttons++;
  }
  for (unsigned int i = BTN_MISC; i < KEY_MAX; i++)
  {
    if (test_bit(i, keybit))
      m_button_bind[i] = buttons++;
  }
  SetButtonCount(m_button_bind.size());

  unsigned int axes = 0;
  for (unsigned i = 0; i < ABS_MISC; i++)
  {
    if (test_bit(i, absbit))
    {
      input_absinfo abs;
      if (ioctl(m_fd, EVIOCGABS(i), &abs) < 0)
        continue;

      if (abs.maximum > abs.minimum)
        m_axes_bind[i] = { axes++, abs };
    }
  }
  SetAxisCount(m_axes_bind.size());

  // Check for rumble features
  if (ioctl(m_fd, EVIOCGBIT(EV_FF, sizeof(ffbit)), ffbit) >= 0)
  {
    unsigned int num_effects;
    if (ioctl(m_fd, EVIOCGEFFECTS, &num_effects) >= 0)
      SetMotorCount(std::min(num_effects, static_cast<unsigned int>(MOTOR_COUNT)));
  }

  return true;
}

bool CJoystickUdev::SetMotor(unsigned int motorIndex, float magnitude)
{

  if (!m_bInitialized)
    return false;

  if (motorIndex >= MotorCount() || magnitude < 0.0f)
    return false;

  if (magnitude < 0.01f)
    magnitude = 0.0f;

  uint16_t strength = std::min(0xffff, static_cast<int>(magnitude * 0xffff));

  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  m_motors[motorIndex] = strength;

  return true;
}
