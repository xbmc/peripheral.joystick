/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#define PERIPHERAL_ADDON_JOYSTICKS

#include "addon.h"

#include "api/Joystick.h"
#include "api/JoystickManager.h"
#include "api/PeripheralScanner.h"
#include "filesystem/Filesystem.h"
#include "log/Log.h"
#include "log/LogAddon.h"
#include "settings/Settings.h"
#include "storage/StorageManager.h"
#include "utils/CommonMacros.h"

#include <algorithm>
#include <vector>

using namespace JOYSTICK;

CPeripheralJoystick::CPeripheralJoystick() :
  m_scanner(nullptr)
{
}

ADDON_STATUS CPeripheralJoystick::Create()
{
  CLog::Get().SetPipe(new CLogAddon());

  if (!CFilesystem::Initialize())
    return ADDON_STATUS_PERMANENT_FAILURE;

  m_scanner = new CPeripheralScanner(this);
  if (!CJoystickManager::Get().Initialize(m_scanner))
    return ADDON_STATUS_PERMANENT_FAILURE;

  if (!CStorageManager::Get().Initialize(this))
    return ADDON_STATUS_PERMANENT_FAILURE;

  return ADDON_STATUS_NEED_SETTINGS;
}

ADDON_STATUS CPeripheralJoystick::SetSetting(const std::string& settingName, const kodi::CSettingValue& settingValue)
{
  CSettings::Get().SetSetting(settingName, settingValue);
  return ADDON_STATUS_OK;
}


ADDON_STATUS CPeripheralJoystick::GetStatus()
{
  if (!CSettings::Get().IsInitialized())
    return ADDON_STATUS_NEED_SETTINGS;

  return ADDON_STATUS_OK;
}

CPeripheralJoystick::~CPeripheralJoystick()
{
  CStorageManager::Get().Deinitialize();
  CJoystickManager::Get().Deinitialize();
  CFilesystem::Deinitialize();

  CLog::Get().SetType(SYS_LOG_TYPE_CONSOLE);

  delete m_scanner;
}

void CPeripheralJoystick::GetCapabilities(kodi::addon::PeripheralCapabilities& capabilities)
{
  capabilities.SetProvidesJoysticks(true);
  capabilities.SetProvidesButtonmaps(true);
}

PERIPHERAL_ERROR CPeripheralJoystick::PerformDeviceScan(std::vector<std::shared_ptr<kodi::addon::Peripheral>>& scan_results)
{
  JoystickVector joysticks;
  if (!CJoystickManager::Get().PerformJoystickScan(joysticks))
    return PERIPHERAL_ERROR_FAILED;

  // Upcast array pointers
  for (const auto& it : joysticks)
    scan_results.emplace_back(it);

  return PERIPHERAL_NO_ERROR;
}

PERIPHERAL_ERROR CPeripheralJoystick::GetEvents(std::vector<kodi::addon::PeripheralEvent>& events)
{
  PERIPHERAL_ERROR result = PERIPHERAL_ERROR_FAILED;

  if (CJoystickManager::Get().GetEvents(events))
    result = PERIPHERAL_NO_ERROR;

  CJoystickManager::Get().ProcessEvents();

  return result;
}

bool CPeripheralJoystick::SendEvent(const kodi::addon::PeripheralEvent& event)
{
  return CJoystickManager::Get().SendEvent(event);
}

PERIPHERAL_ERROR CPeripheralJoystick::GetJoystickInfo(unsigned int index, kodi::addon::Joystick& info)
{
  JoystickPtr joystick = CJoystickManager::Get().GetJoystick(index);
  if (!joystick)
    return PERIPHERAL_ERROR_NOT_CONNECTED;

  info = *joystick;

  return PERIPHERAL_NO_ERROR;
}

PERIPHERAL_ERROR CPeripheralJoystick::GetFeatures(const kodi::addon::Joystick& joystick,
                                                  const std::string& controller_id,
                                                  std::vector<kodi::addon::JoystickFeature>& features)
{
  CStorageManager::Get().GetFeatures(joystick, controller_id, features);

  return PERIPHERAL_NO_ERROR;
}

PERIPHERAL_ERROR CPeripheralJoystick::MapFeatures(const kodi::addon::Joystick& joystick,
                                                  const std::string& controller_id,
                                                  const std::vector<kodi::addon::JoystickFeature>& features)
{
  bool bSuccess = CStorageManager::Get().MapFeatures(joystick, controller_id, features);

  return bSuccess ? PERIPHERAL_NO_ERROR : PERIPHERAL_ERROR_FAILED;
}

PERIPHERAL_ERROR CPeripheralJoystick::GetIgnoredPrimitives(const kodi::addon::Joystick& joystick,
                                                           std::vector<kodi::addon::DriverPrimitive>& primitives)
{
  CStorageManager::Get().GetIgnoredPrimitives(joystick, primitives);

  return PERIPHERAL_NO_ERROR;
}

PERIPHERAL_ERROR CPeripheralJoystick::SetIgnoredPrimitives(const kodi::addon::Joystick& joystick,
                                                           const std::vector<kodi::addon::DriverPrimitive>& primitives)
{
  bool bSuccess = CStorageManager::Get().SetIgnoredPrimitives(joystick, primitives);

  return bSuccess ? PERIPHERAL_NO_ERROR : PERIPHERAL_ERROR_FAILED;
}

void CPeripheralJoystick::SaveButtonMap(const kodi::addon::Joystick& joystick)
{
  CStorageManager::Get().SaveButtonMap(joystick);
}

void CPeripheralJoystick::RevertButtonMap(const kodi::addon::Joystick& joystick)
{
  CStorageManager::Get().RevertButtonMap(joystick);
}

void CPeripheralJoystick::ResetButtonMap(const kodi::addon::Joystick& joystick, const std::string& controller_id)
{
  CStorageManager::Get().ResetButtonMap(joystick, controller_id);
}

void CPeripheralJoystick::PowerOffJoystick(unsigned int index)
{
  JoystickPtr joystick = CJoystickManager::Get().GetJoystick(index);
  if (!joystick || !joystick->SupportsPowerOff())
    return;

  joystick->PowerOff();
}

ADDONCREATOR(CPeripheralJoystick) // Don't touch this!
