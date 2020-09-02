/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "utils/CommonMacros.h"

#include <kodi/addon-instance/Peripheral.h>

namespace JOYSTICK
{
  class CPeripheralScanner;
}

class DLL_PRIVATE CPeripheralJoystick
  : public kodi::addon::CAddonBase,
    public kodi::addon::CInstancePeripheral
{
public:
  CPeripheralJoystick();
  virtual ~CPeripheralJoystick();

  ADDON_STATUS Create() override;
  ADDON_STATUS GetStatus() override;
  ADDON_STATUS SetSetting(const std::string& settingName, const kodi::CSettingValue& settingValue) override;

  void GetCapabilities(kodi::addon::PeripheralCapabilities& capabilities) override;
  PERIPHERAL_ERROR PerformDeviceScan(std::vector<std::shared_ptr<kodi::addon::Peripheral>>& scan_results) override;
  PERIPHERAL_ERROR GetEvents(std::vector<kodi::addon::PeripheralEvent>& events) override;
  bool SendEvent(const kodi::addon::PeripheralEvent& event) override;
  PERIPHERAL_ERROR GetJoystickInfo(unsigned int index, kodi::addon::Joystick& info) override;
  PERIPHERAL_ERROR GetFeatures(const kodi::addon::Joystick& joystick,
                               const std::string& controller_id,
                               std::vector<kodi::addon::JoystickFeature>& features) override;
  PERIPHERAL_ERROR MapFeatures(const kodi::addon::Joystick& joystick,
                               const std::string& controller_id,
                               const std::vector<kodi::addon::JoystickFeature>& features) override;
  PERIPHERAL_ERROR GetIgnoredPrimitives(const kodi::addon::Joystick& joystick,
                                        std::vector<kodi::addon::DriverPrimitive>& primitives) override;
  PERIPHERAL_ERROR SetIgnoredPrimitives(const kodi::addon::Joystick& joystick,
                                        const std::vector<kodi::addon::DriverPrimitive>& primitives) override;
  void SaveButtonMap(const kodi::addon::Joystick& joystick) override;
  void RevertButtonMap(const kodi::addon::Joystick& joystick) override;
  void ResetButtonMap(const kodi::addon::Joystick& joystick, const std::string& controller_id) override;
  void PowerOffJoystick(unsigned int index) override;

private:
  JOYSTICK::CPeripheralScanner* m_scanner;
};
