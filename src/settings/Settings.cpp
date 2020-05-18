/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Settings.h"
#include "api/JoystickManager.h"
#include "log/Log.h"

#include <array>

using namespace JOYSTICK;

#define SETTING_RETROARCH_CONFIG    "retroarchconfig"
#define SETTING_LINUX_DRIVER        "driver_linux"
#define SETTING_SDL_DRIVER          "driver_sdl"
#define SETTING_OSX_DRIVER          "driver_osx"
#define SETTING_XINPUT_DRIVER       "driver_xinput"
#define SETTING_DIRECTINPUT_DRIVER  "driver_directinput"

CSettings::CSettings(void)
  : m_bInitialized(false),
    m_bGenerateRetroArchConfigs(false)
{
}

CSettings& CSettings::Get(void)
{
  static CSettings _instance;
  return _instance;
}

void CSettings::SetSetting(const std::string& strName, const kodi::CSettingValue& value)
{
  if (strName == SETTING_RETROARCH_CONFIG)
  {
    m_bGenerateRetroArchConfigs = value.GetBoolean();
    dsyslog("Setting \"%s\" set to %f", SETTING_RETROARCH_CONFIG, m_bGenerateRetroArchConfigs ? "true" : "false");
  }
  else if (strName == SETTING_LINUX_DRIVER ||
           strName == SETTING_SDL_DRIVER ||
           strName == SETTING_OSX_DRIVER)
  {
    std::array<EJoystickInterface, 4> drivers;

    if (strName == SETTING_LINUX_DRIVER)
    {
      drivers = {
          EJoystickInterface::LINUX,
          EJoystickInterface::UDEV,
          EJoystickInterface::NONE,
      };
    }
    else if (strName == SETTING_SDL_DRIVER)
    {
      drivers = {
          EJoystickInterface::SDL,
          EJoystickInterface::LINUX,
          EJoystickInterface::UDEV,
          EJoystickInterface::NONE,
      };
    }
    else if (strName == SETTING_OSX_DRIVER)
    {
      drivers = {
          EJoystickInterface::COCOA,
          EJoystickInterface::NONE,
      };
    }

    int ifaceIndex = value.GetInt();
    unsigned int driverIndex = 0;
    for (auto driver : drivers)
    {
      if (driver == EJoystickInterface::NONE)
        break;
      CJoystickManager::Get().SetEnabled(driver, (driverIndex == ifaceIndex));
      driverIndex++;
    }

    CJoystickManager::Get().TriggerScan();
  }
  else if (strName == SETTING_XINPUT_DRIVER)
  {
    const EJoystickInterface iface = EJoystickInterface::XINPUT;
    CJoystickManager::Get().SetEnabled(iface, value.GetBoolean());
    CJoystickManager::Get().TriggerScan();
  }
  else if (strName == SETTING_DIRECTINPUT_DRIVER)
  {
    const EJoystickInterface iface = EJoystickInterface::DIRECTINPUT;
    CJoystickManager::Get().SetEnabled(iface, value.GetBoolean());
    CJoystickManager::Get().TriggerScan();
  }

  m_bInitialized = true;
}
