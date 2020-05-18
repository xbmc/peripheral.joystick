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

void CPeripheralJoystick::GetCapabilities(PERIPHERAL_CAPABILITIES& capabilities)
{
  capabilities.provides_joysticks = true;
  capabilities.provides_buttonmaps = true;
}

PERIPHERAL_ERROR CPeripheralJoystick::PerformDeviceScan(unsigned int* peripheral_count, PERIPHERAL_INFO** scan_results)
{
  if (!peripheral_count || !scan_results)
    return PERIPHERAL_ERROR_INVALID_PARAMETERS;

  JoystickVector joysticks;
  if (!CJoystickManager::Get().PerformJoystickScan(joysticks))
    return PERIPHERAL_ERROR_FAILED;

  // Upcast array pointers
  std::vector<kodi::addon::Peripheral*> peripherals;
  for (JoystickVector::const_iterator it = joysticks.begin(); it != joysticks.end(); ++it)
    peripherals.push_back(it->get());

  *peripheral_count = static_cast<unsigned int>(peripherals.size());
  kodi::addon::Peripherals::ToStructs(peripherals, scan_results);

  return PERIPHERAL_NO_ERROR;
}

void CPeripheralJoystick::FreeScanResults(unsigned int peripheral_count, PERIPHERAL_INFO* scan_results)
{
  kodi::addon::Peripherals::FreeStructs(peripheral_count, scan_results);
}

PERIPHERAL_ERROR CPeripheralJoystick::GetEvents(unsigned int* event_count, PERIPHERAL_EVENT** events)
{
  if (!event_count || !events)
    return PERIPHERAL_ERROR_INVALID_PARAMETERS;

  PERIPHERAL_ERROR result = PERIPHERAL_ERROR_FAILED;

  std::vector<kodi::addon::PeripheralEvent> peripheralEvents;
  if (CJoystickManager::Get().GetEvents(peripheralEvents))
  {
    *event_count = static_cast<unsigned int>(peripheralEvents.size());
    kodi::addon::PeripheralEvents::ToStructs(peripheralEvents, events);
    result = PERIPHERAL_NO_ERROR;
  }

  CJoystickManager::Get().ProcessEvents();

  return result;
}

void CPeripheralJoystick::FreeEvents(unsigned int event_count, PERIPHERAL_EVENT* events)
{
  kodi::addon::PeripheralEvents::FreeStructs(event_count, events);
}

bool CPeripheralJoystick::SendEvent(const PERIPHERAL_EVENT* event)
{
  bool bHandled = false;

  if (event != nullptr)
    bHandled = CJoystickManager::Get().SendEvent(kodi::addon::PeripheralEvent(*event));

  return bHandled;
}

PERIPHERAL_ERROR CPeripheralJoystick::GetJoystickInfo(unsigned int index, JOYSTICK_INFO* info)
{
  if (!info)
    return PERIPHERAL_ERROR_INVALID_PARAMETERS;

  JoystickPtr joystick = CJoystickManager::Get().GetJoystick(index);
  if (!joystick)
    return PERIPHERAL_ERROR_NOT_CONNECTED;

  // Need to be explicit because we're using typedef struct { ... }T instead of struct T{ ... }
  joystick->kodi::addon::Joystick::ToStruct(*info);

  return PERIPHERAL_NO_ERROR;
}

void CPeripheralJoystick::FreeJoystickInfo(JOYSTICK_INFO* info)
{
  if (!info)
    return;

  kodi::addon::Joystick::FreeStruct(*info);
}

PERIPHERAL_ERROR CPeripheralJoystick::GetFeatures(const JOYSTICK_INFO* joystick, const char* controller_id,
                                                  unsigned int* feature_count, JOYSTICK_FEATURE** features)
{
  if (!joystick || !controller_id || !feature_count || !features)
    return PERIPHERAL_ERROR_INVALID_PARAMETERS;

  FeatureVector featureVector;
  CStorageManager::Get().GetFeatures(kodi::addon::Joystick(*joystick), controller_id,  featureVector);

  *feature_count = static_cast<unsigned int>(featureVector.size());
  kodi::addon::JoystickFeatures::ToStructs(featureVector, features);

  return PERIPHERAL_NO_ERROR;
}

void CPeripheralJoystick::FreeFeatures(unsigned int feature_count, JOYSTICK_FEATURE* features)
{
  kodi::addon::JoystickFeatures::FreeStructs(feature_count, features);
}

PERIPHERAL_ERROR CPeripheralJoystick::MapFeatures(const JOYSTICK_INFO* joystick, const char* controller_id,
                             unsigned int feature_count, const JOYSTICK_FEATURE* features)
{
  if (!joystick || !controller_id || (feature_count > 0 && !features))
    return PERIPHERAL_ERROR_INVALID_PARAMETERS;

  FeatureVector featureVector(features, features + feature_count);
  bool bSuccess = CStorageManager::Get().MapFeatures(kodi::addon::Joystick(*joystick), controller_id, featureVector);

  return bSuccess ? PERIPHERAL_NO_ERROR : PERIPHERAL_ERROR_FAILED;
}

PERIPHERAL_ERROR CPeripheralJoystick::GetIgnoredPrimitives(const JOYSTICK_INFO* joystick,
                                      unsigned int* primitive_count,
                                      JOYSTICK_DRIVER_PRIMITIVE** primitives)
{
  if (joystick == nullptr || primitive_count == nullptr || primitives == nullptr)
    return PERIPHERAL_ERROR_INVALID_PARAMETERS;

  PrimitiveVector primitiveVector;
  CStorageManager::Get().GetIgnoredPrimitives(kodi::addon::Joystick(*joystick), primitiveVector);

  *primitive_count = static_cast<unsigned int>(primitiveVector.size());
  kodi::addon::DriverPrimitives::ToStructs(primitiveVector, primitives);

  return PERIPHERAL_NO_ERROR;
}

void CPeripheralJoystick::FreePrimitives(unsigned int primitive_count, JOYSTICK_DRIVER_PRIMITIVE* primitives)
{
  kodi::addon::DriverPrimitives::FreeStructs(primitive_count, primitives);
}

PERIPHERAL_ERROR CPeripheralJoystick::SetIgnoredPrimitives(const JOYSTICK_INFO* joystick,
                                      unsigned int primitive_count,
                                      const JOYSTICK_DRIVER_PRIMITIVE* primitives)
{
  if (joystick == nullptr || (primitive_count > 0 && primitives == nullptr))
    return PERIPHERAL_ERROR_INVALID_PARAMETERS;

  PrimitiveVector primitiveVector;

  for (unsigned int i = 0; i < primitive_count; i++)
    primitiveVector.emplace_back(*(primitives + i));

  bool bSuccess = CStorageManager::Get().SetIgnoredPrimitives(kodi::addon::Joystick(*joystick), primitiveVector);

  return bSuccess ? PERIPHERAL_NO_ERROR : PERIPHERAL_ERROR_FAILED;
}

void CPeripheralJoystick::SaveButtonMap(const JOYSTICK_INFO* joystick)
{
  if (joystick == nullptr)
    return;

  kodi::addon::Joystick addonJoystick(*joystick);

  CStorageManager::Get().SaveButtonMap(addonJoystick);
}

void CPeripheralJoystick::RevertButtonMap(const JOYSTICK_INFO* joystick)
{
  if (joystick == nullptr)
    return;

  kodi::addon::Joystick addonJoystick(*joystick);

  CStorageManager::Get().RevertButtonMap(addonJoystick);
}

void CPeripheralJoystick::ResetButtonMap(const JOYSTICK_INFO* joystick, const char* controller_id)
{
  if (!joystick || !controller_id)
    return;

  kodi::addon::Joystick addonJoystick(*joystick);

  CStorageManager::Get().ResetButtonMap(addonJoystick, controller_id);
}

void CPeripheralJoystick::PowerOffJoystick(unsigned int index)
{
  JoystickPtr joystick = CJoystickManager::Get().GetJoystick(index);
  if (!joystick || !joystick->SupportsPowerOff())
    return;

  joystick->PowerOff();
}

ADDONCREATOR(CPeripheralJoystick) // Don't touch this!
