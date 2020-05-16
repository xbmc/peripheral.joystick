/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickInterfaceUdev.h"
#include "JoystickUdev.h"
#include "api/JoystickTypes.h"

#include <libudev.h>
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
    return false;

  m_udev_mon = udev_monitor_new_from_netlink(m_udev, "udev");
  if (m_udev_mon)
  {
     udev_monitor_filter_add_match_subsystem_devtype(m_udev_mon, "input", nullptr);
     udev_monitor_enable_receiving(m_udev_mon);
  }

  return true;
}

void CJoystickInterfaceUdev::Deinitialize()
{
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
     const char*         name = udev_list_entry_get_name(item);
     struct udev_device* dev = udev_device_new_from_syspath(m_udev, name);
     const char*         devnode = udev_device_get_devnode(dev);

     if (devnode != nullptr)
     {
       JoystickPtr joystick = JoystickPtr(new CJoystickUdev(dev, devnode));
       joysticks.push_back(joystick);
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
