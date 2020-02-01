/*
 *      Copyright (C) 2014-2020 Garrett Brown
 *      Copyright (C) 2014-2020 Team Kodi
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
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

  void GetCapabilities(PERIPHERAL_CAPABILITIES &capabilities) override;
  PERIPHERAL_ERROR PerformDeviceScan(unsigned int* peripheral_count, PERIPHERAL_INFO** scan_results) override;
  void FreeScanResults(unsigned int peripheral_count, PERIPHERAL_INFO* scan_results) override;
  PERIPHERAL_ERROR GetEvents(unsigned int* event_count, PERIPHERAL_EVENT** events) override;
  void FreeEvents(unsigned int event_count, PERIPHERAL_EVENT* events) override;
  bool SendEvent(const PERIPHERAL_EVENT* event) override;
  PERIPHERAL_ERROR GetJoystickInfo(unsigned int index, JOYSTICK_INFO* info) override;
  void FreeJoystickInfo(JOYSTICK_INFO* info) override;
  PERIPHERAL_ERROR GetFeatures(const JOYSTICK_INFO* joystick, const char* controller_id,
                                       unsigned int* feature_count, JOYSTICK_FEATURE** features) override;
  void FreeFeatures(unsigned int feature_count, JOYSTICK_FEATURE* features) override;
  PERIPHERAL_ERROR MapFeatures(const JOYSTICK_INFO* joystick, const char* controller_id,
                                       unsigned int feature_count, const JOYSTICK_FEATURE* features) override;
  PERIPHERAL_ERROR GetIgnoredPrimitives(const JOYSTICK_INFO* joystick,
                                                unsigned int* primitive_count,
                                                JOYSTICK_DRIVER_PRIMITIVE** primitives) override;
  void FreePrimitives(unsigned int primitive_count, JOYSTICK_DRIVER_PRIMITIVE* primitives) override;
  PERIPHERAL_ERROR SetIgnoredPrimitives(const JOYSTICK_INFO* joystick,
                                                unsigned int primitive_count,
                                                const JOYSTICK_DRIVER_PRIMITIVE* primitives) override;
  void SaveButtonMap(const JOYSTICK_INFO* joystick) override;
  void RevertButtonMap(const JOYSTICK_INFO* joystick) override;
  void ResetButtonMap(const JOYSTICK_INFO* joystick, const char* controller_id) override;
  void PowerOffJoystick(unsigned int index) override;

private:
  JOYSTICK::CPeripheralScanner* m_scanner;
};
