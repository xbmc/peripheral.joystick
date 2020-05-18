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
