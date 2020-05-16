/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "StorageManager.h"
#include "JustABunchOfFiles.h"
#include "StorageUtils.h"
#include "buttonmapper/ButtonMapper.h"
#include "log/Log.h"
#include "storage/api/DatabaseJoystickAPI.h"
//#include "storage/retroarch/DatabaseRetroarch.h" // TODO
#include "storage/xml/DatabaseXml.h"
#include "utils/StringUtils.h"

#include "addon.h"

using namespace JOYSTICK;

// Resources folder for add-on and user data
#define USER_RESOURCES_FOLDER   "resources"
#define ADDON_RESOURCES_FOLDER  "resources"

// Subdirectory under resources folder for storing button maps
#define BUTTONMAP_FOLDER        "buttonmaps"

CStorageManager::CStorageManager(void) :
  m_peripheralLib(nullptr)
{
}

CStorageManager::~CStorageManager()
{
  Deinitialize();
}

CStorageManager& CStorageManager::Get(void)
{
  static CStorageManager _instance;
  return _instance;
}

bool CStorageManager::Initialize(CPeripheralJoystick* peripheralLib)
{
  std::string strUserPath = peripheralLib->UserPath();
  std::string strAddonPath = peripheralLib->AddonPath();

  if (peripheralLib == NULL || strUserPath.empty() || strAddonPath.empty())
    return false;

  m_peripheralLib = peripheralLib;

  m_buttonMapper.reset(new CButtonMapper(peripheralLib));

  if (!m_buttonMapper->Initialize(m_familyManager))
    return false;

  // Remove slash at end
  StringUtils::TrimRight(strUserPath, "\\/");
  StringUtils::TrimRight(strAddonPath, "\\/");

  strUserPath += "/" USER_RESOURCES_FOLDER;
  strAddonPath += "/" ADDON_RESOURCES_FOLDER;

  // Ensure resources path exists in user data
  CStorageUtils::EnsureDirectoryExists(strUserPath);

  std::string strUserButtonMapPath = strUserPath + "/" BUTTONMAP_FOLDER;
  std::string strAddonButtonMapPath = strAddonPath + "/" BUTTONMAP_FOLDER;

  // Ensure button map path exists in user data
  CStorageUtils::EnsureDirectoryExists(strUserButtonMapPath);

  m_databases.push_back(DatabasePtr(new CDatabaseXml(strUserButtonMapPath, true, m_buttonMapper->GetCallbacks(), this)));
  //m_databases.push_back(DatabasePtr(new CDatabaseRetroArch(strUserButtonMapPath, true, &m_controllerMapper))); // TODO
  m_databases.push_back(DatabasePtr(new CDatabaseXml(strAddonButtonMapPath, false, m_buttonMapper->GetCallbacks(), this)));
  //m_databases.push_back(DatabasePtr(new CDatabaseRetroArch(strAddonButtonMapPath, false))); // TODO

  m_databases.push_back(DatabasePtr(new CDatabaseJoystickAPI(m_buttonMapper->GetCallbacks())));

  for (auto& database : m_databases)
    m_buttonMapper->RegisterDatabase(database);

  m_familyManager.Initialize(strAddonPath);

  return true;
}

void CStorageManager::Deinitialize(void)
{
  m_familyManager.Deinitialize();
  m_databases.clear();
  m_buttonMapper.reset();
  m_peripheralLib = nullptr;
}

void CStorageManager::GetFeatures(const kodi::addon::Joystick& joystick,
                                  const std::string& strControllerId,
                                  FeatureVector& features)
{
  if (m_buttonMapper)
    m_buttonMapper->GetFeatures(joystick, strControllerId, features);
}

bool CStorageManager::MapFeatures(const kodi::addon::Joystick& joystick,
                                  const std::string& strControllerId,
                                  const FeatureVector& features)
{
  bool bSuccess = false;

  for (DatabaseVector::const_iterator it = m_databases.begin(); it != m_databases.end(); ++it)
    bSuccess |= (*it)->MapFeatures(joystick, strControllerId, features);

  return bSuccess;
}

void CStorageManager::GetIgnoredPrimitives(const kodi::addon::Joystick& joystick, PrimitiveVector& primitives)
{
  for (DatabaseVector::const_iterator it = m_databases.begin(); it != m_databases.end(); ++it)
  {
    if ((*it)->GetIgnoredPrimitives(joystick, primitives))
      break;
  }
}

bool CStorageManager::SetIgnoredPrimitives(const kodi::addon::Joystick& joystick, const PrimitiveVector& primitives)
{
  bool bSuccess = false;

  for (DatabaseVector::const_iterator it = m_databases.begin(); it != m_databases.end(); ++it)
    bSuccess |= (*it)->SetIgnoredPrimitives(joystick, primitives);

  return bSuccess;
}

bool CStorageManager::SaveButtonMap(const kodi::addon::Joystick& joystick)
{
  bool bModified = false;

  for (DatabaseVector::const_iterator it = m_databases.begin(); it != m_databases.end(); ++it)
    bModified |= (*it)->SaveButtonMap(joystick);

  return bModified;
}

bool CStorageManager::RevertButtonMap(const kodi::addon::Joystick& joystick)
{
  bool bModified = false;

  for (DatabaseVector::const_iterator it = m_databases.begin(); it != m_databases.end(); ++it)
    bModified |= (*it)->RevertButtonMap(joystick);

  return bModified;
}

bool CStorageManager::ResetButtonMap(const kodi::addon::Joystick& joystick, const std::string& strControllerId)
{
  bool bModified = false;

  for (DatabaseVector::const_iterator it = m_databases.begin(); it != m_databases.end(); ++it)
    bModified |= (*it)->ResetButtonMap(joystick, strControllerId);

  return bModified;
}

void CStorageManager::RefreshButtonMaps(const std::string& strDeviceName /* = "" */)
{
  // Request the frontend to refresh its button maps
  if (m_peripheralLib)
    m_peripheralLib->RefreshButtonMaps(strDeviceName);
}

JOYSTICK_FEATURE_TYPE CStorageManager::FeatureType(const std::string& strControllerId, const std::string &featureName)
{
  if (m_peripheralLib)
    return m_peripheralLib->FeatureType(strControllerId, featureName);

  return JOYSTICK_FEATURE_TYPE_UNKNOWN;
}
