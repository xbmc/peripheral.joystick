/*
 *  Copyright (C) 2016-2021 Garrett Brown
 *  Copyright (C) 2016-2021 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickUdev.h"
#include "api/JoystickTypes.h"
#include "log/Log.h"
#if defined(HAVE_PERIPHERAL_JOYSTICK_BATTERY)
#include "utils/linux/PowerSupply.h"
#endif

#include <algorithm>
#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <limits.h>
#include <linux/input.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

using namespace JOYSTICK;

#ifndef INVALID_FD
  #define INVALID_FD  (-1)
#endif

// From RetroArch
#define test_bit(nr, addr) \
   (((1UL << ((nr) % (sizeof(long) * CHAR_BIT))) & ((addr)[(nr) / (sizeof(long) * CHAR_BIT)])) != 0)

// From RetroArch
#define NBITS(x)  ((((x) - 1) / (sizeof(long) * CHAR_BIT)) + 1)

#if defined(HAVE_PERIPHERAL_JOYSTICK_MOTION)
namespace
{
constexpr float STANDARD_GRAVITY = 9.80665f;
constexpr float RADIANS_PER_DEGREE = 3.14159265358979f / 180.0f;
constexpr int NINTENDO_VENDOR_ID = 0x057e;

// How long to keep looking for a motion sensor that appears after the pad
constexpr auto MOTION_SENSOR_SEARCH_TIME = std::chrono::seconds(5);
constexpr auto MOTION_SENSOR_SEARCH_INTERVAL = std::chrono::milliseconds(500);

unsigned int MotionAxisCode(unsigned int axis)
{
  return axis < 3 ? ABS_X + axis : ABS_RX + (axis - 3);
}

const char* GetHidSyspath(udev_device* dev)
{
  udev_device* hid = udev_device_get_parent_with_subsystem_devtype(dev, "hid", nullptr);
  return hid != nullptr ? udev_device_get_syspath(hid) : nullptr;
}
} // namespace
#endif

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
  // The device is released once the joystick is constructed
  if (const char* syspath = udev_device_get_syspath(dev))
    m_syspath = syspath;

#if defined(HAVE_PERIPHERAL_JOYSTICK_MOTION)
  m_created = std::chrono::steady_clock::now();
  FindMotionSensor(dev);
#endif

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

#if defined(HAVE_PERIPHERAL_JOYSTICK_MOTION)
    OpenMotionSensor();
#endif

    m_bInitialized = true;
  }

  return m_bInitialized;
}

void CJoystickUdev::Close(void)
{
  if (m_fd >= 0)
  {
    close(m_fd);
    m_fd = INVALID_FD;
  }

#if defined(HAVE_PERIPHERAL_JOYSTICK_MOTION)
  CloseMotionSensor();
#endif
}

void CJoystickUdev::Deinitialize(void)
{
  Close();
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

  if (!bPlayStop && m_effect != -1)
  {
    if (ioctl(m_fd, EVIOCRMFF, m_effect) < 0)
    {
      const int err = errno;

      esyslog("[udev]: Failed to remove rumble effect %d on \"%s\" - %s", m_effect,
              Name().c_str(), strerror(err));

      // EINVAL means the slot is already gone. Any other error can leave the
      // effect allocated to us, and the id is the only handle left to it.
      if (err != EINVAL)
        return;
    }

    m_effect = -1;
  }
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

#if defined(HAVE_PERIPHERAL_JOYSTICK_BATTERY)
bool CJoystickUdev::GetBattery(JOYSTICK_BATTERY& battery)
{
  if (m_syspath.empty())
    return false;

  return CPowerSupply::GetBattery(m_syspath, battery);
}
#endif

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

              int middle = (info.minimum + info.maximum) / 2;
              int length = (info.maximum - info.minimum) / 2;

              SetAxisValue(axisIndex, event.value - middle, length);
            }
          }
          break;
        }
        default:
          break;
      }
    }
  }

#if defined(HAVE_PERIPHERAL_JOYSTICK_MOTION)
  ScanMotionEvents();
#endif

  return true;
}

#if defined(HAVE_PERIPHERAL_JOYSTICK_MOTION)
void CJoystickUdev::FindMotionSensor(udev_device* dev)
{
  const char* hidSyspath = GetHidSyspath(dev);
  if (hidSyspath == nullptr)
    return;

  udev* context = udev_device_get_udev(dev);
  udev_enumerate* enumerate = udev_enumerate_new(context);
  if (enumerate == nullptr)
    return;

  udev_enumerate_add_match_subsystem(enumerate, "input");
  udev_enumerate_add_match_property(enumerate, "ID_INPUT_ACCELEROMETER", "1");
  udev_enumerate_scan_devices(enumerate);

  udev_list_entry* entry;
  udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(enumerate))
  {
    udev_device* sensor = udev_device_new_from_syspath(context, udev_list_entry_get_name(entry));
    if (sensor == nullptr)
      continue;

    const char* devnode = udev_device_get_devnode(sensor);
    const char* sensorHidSyspath = GetHidSyspath(sensor);
    if (devnode != nullptr && sensorHidSyspath != nullptr &&
        strncmp(devnode, "/dev/input/event", 16) == 0 &&
        strcmp(sensorHidSyspath, hidSyspath) == 0)
      m_motionPath = devnode;

    udev_device_unref(sensor);

    if (!m_motionPath.empty())
      break;
  }

  udev_enumerate_unref(enumerate);
}

bool CJoystickUdev::OpenMotionSensor()
{
  if (m_motionPath.empty())
    return false;

  m_motionFd = open(m_motionPath.c_str(), O_RDONLY | O_NONBLOCK | O_CLOEXEC);
  if (m_motionFd < 0)
    return false;

  // An axis the sensor lacks keeps a resolution of 0: the DualShock 3's has no gyroscope
  bool hasAxis = false;
  for (unsigned int i = 0; i < JOYSTICK_MOTION_AXIS_COUNT; i++)
  {
    input_absinfo abs = {};
    const bool present =
        ioctl(m_motionFd, EVIOCGABS(MotionAxisCode(i)), &abs) == 0 && abs.resolution != 0;

    m_motionResolution[i] = present ? abs.resolution : 0;
    m_motionRaw[i] = present ? abs.value : 0;
    hasAxis |= present;
  }

  if (!hasAxis)
  {
    CloseMotionSensor();
    return false;
  }

  m_motionDropped = false;

  // Linux reports only changes, so a still pad's first reading goes out now
  ReportMotion();
  return true;
}

void CJoystickUdev::ReadMotionAxes()
{
  for (unsigned int i = 0; i < JOYSTICK_MOTION_AXIS_COUNT; i++)
  {
    input_absinfo abs = {};
    if (m_motionResolution[i] != 0 && ioctl(m_motionFd, EVIOCGABS(MotionAxisCode(i)), &abs) == 0)
      m_motionRaw[i] = abs.value;
  }
}

void CJoystickUdev::ScanMotionEvents()
{
  if (m_motionFd < 0)
  {
    const auto now = std::chrono::steady_clock::now();
    if (m_syspath.empty() || now - m_created > MOTION_SENSOR_SEARCH_TIME ||
        now < m_nextMotionSearch)
      return;

    m_nextMotionSearch = now + MOTION_SENSOR_SEARCH_INTERVAL;

    udev* context = udev_new();
    if (context == nullptr)
      return;

    if (udev_device* dev = udev_device_new_from_syspath(context, m_syspath.c_str()))
    {
      FindMotionSensor(dev);
      udev_device_unref(dev);
    }
    udev_unref(context);

    if (!OpenMotionSensor())
      return;
  }

  // Said here rather than when opening, as the joystick scan makes and drops a
  // copy of each joystick every few seconds
  if (!m_motionReported)
  {
    m_motionReported = true;
    isyslog("[udev]: \"%s\" has a motion sensor at %s", Name().c_str(), m_motionPath.c_str());
  }

  input_event events[32];

  int len;
  while ((len = read(m_motionFd, events, sizeof(events))) > 0)
  {
    len /= sizeof(*events);
    for (unsigned int i = 0; i < static_cast<unsigned int>(len); i++)
    {
      const input_event& event = events[i];

      if (event.type == EV_SYN && event.code == SYN_DROPPED)
      {
        // Events were lost: the rest of this packet is skipped and the axes read afresh
        m_motionDropped = true;
      }
      else if (event.type == EV_SYN && event.code == SYN_REPORT)
      {
        if (std::exchange(m_motionDropped, false))
          ReadMotionAxes();
        ReportMotion();
      }
      else if (event.type == EV_ABS && !m_motionDropped)
      {
        if (event.code >= ABS_X && event.code <= ABS_Z)
          m_motionRaw[event.code - ABS_X] = event.value;
        else if (event.code >= ABS_RX && event.code <= ABS_RZ)
          m_motionRaw[3 + event.code - ABS_RX] = event.value;
      }
    }
  }

  if (len < 0 && errno == ENODEV)
    CloseMotionSensor();
}

void CJoystickUdev::ReportMotion()
{
  std::array<float, JOYSTICK_MOTION_AXIS_COUNT> values;

  for (unsigned int i = 0; i < JOYSTICK_MOTION_AXIS_COUNT; i++)
  {
    const float unit = i < 3 ? STANDARD_GRAVITY : RADIANS_PER_DEGREE;
    values[i] = m_motionResolution[i] != 0 ? m_motionRaw[i] * unit / m_motionResolution[i] : 0.0f;
  }

  // hid-nintendo orders its axes differently from SDL's coordinate system
  if (VendorID() == NINTENDO_VENDOR_ID)
  {
    for (unsigned int i = 0; i < JOYSTICK_MOTION_AXIS_COUNT; i += 3)
    {
      const float x = values[i];
      const float y = values[i + 1];
      const float z = values[i + 2];
      values[i] = -y;
      values[i + 1] = z;
      values[i + 2] = -x;
    }
  }

  for (unsigned int i = 0; i < JOYSTICK_MOTION_AXIS_COUNT; i++)
    SetMotionValue(i, values[i]);
}

void CJoystickUdev::CloseMotionSensor()
{
  if (m_motionFd >= 0)
  {
    close(m_motionFd);
    m_motionFd = INVALID_FD;
  }
}
#endif

bool CJoystickUdev::OpenJoystick()
{
  unsigned long evbit[NBITS(EV_MAX)]   = { };
  unsigned long keybit[NBITS(KEY_MAX)] = { };
  unsigned long absbit[NBITS(ABS_MAX)] = { };

  m_fd = open(m_path.c_str(), O_RDWR | O_NONBLOCK);

  if (m_fd < 0)
    return false;

  if (ioctl(m_fd, EVIOCGBIT(0, sizeof(evbit)), evbit) < 0)
  {
    Close();
    return false;
  }

  // Has to at least support EV_KEY interface
  if (!test_bit(EV_KEY, evbit))
  {
    Close();
    return false;
  }

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

  unsigned short id[4] = {};
  char val[16] = {};
  if (ioctl(m_fd, EVIOCGID, id) == 0)
  {
    std::sprintf(val, "%x", id[ID_VENDOR]);
    SetVendorID(strtol(val, NULL, 16));

    std::sprintf(val, "%x", id[ID_PRODUCT]);
    SetProductID(strtol(val, NULL, 16));
  }

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
  for (unsigned int i = 0; i < KEY_MAX; i++)
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
