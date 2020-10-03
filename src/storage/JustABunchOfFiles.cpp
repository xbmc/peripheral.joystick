/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JustABunchOfFiles.h"
#include "StorageDefinitions.h"
#include "StorageUtils.h"
#include "filesystem/DirectoryUtils.h"
#include "log/Log.h"

#include <algorithm>
#include <kodi/tools/StringUtils.h>

using namespace JOYSTICK;

#define FOLDER_DEPTH  1  // Recurse into max 1 subdirectories (provider)

// --- CResources --------------------------------------------------------------

CResources::CResources(const CJustABunchOfFiles* database) :
  m_database(database)
{
}

CResources::~CResources(void)
{
  for (ResourceMap::iterator it = m_resources.begin(); it != m_resources.end(); ++it)
    delete it->second;
}

DevicePtr CResources::GetDevice(const CDevice& deviceInfo) const
{
  DevicePtr device;

  auto itDevice = m_devices.find(deviceInfo);
  if (itDevice != m_devices.end())
    device = itDevice->second;

  return device;
}

CButtonMap* CResources::GetResource(const CDevice& deviceInfo, bool bCreate)
{
  CButtonMap* buttonMap = nullptr;

  auto itResource = m_resources.find(deviceInfo);
  if (itResource == m_resources.end() && bCreate)
  {
    // Resource doesn't exist yet, try to create it now
    std::string resourcePath;
    if (m_database->GetResourcePath(deviceInfo, resourcePath))
    {
      DevicePtr device = m_database->CreateDevice(deviceInfo);
      CButtonMap* resource = m_database->CreateResource(resourcePath, device);
      if (!AddResource(resource))
      {
        delete resource;
        resource = nullptr;
      }
    }

    itResource = m_resources.find(deviceInfo);
  }

  if (itResource != m_resources.end())
    buttonMap = itResource->second;

  return buttonMap;
}

bool CResources::AddResource(CButtonMap* resource)
{
  if (resource != nullptr && resource->IsValid())
  {
    CButtonMap* oldResource = m_resources[*resource->Device()];
    delete oldResource;
    m_resources[*resource->Device()] = resource;
    m_devices[*resource->Device()] = resource->Device();
    return true;
  }
  return false;
}

void CResources::RemoveResource(const std::string& strPath)
{
  for (ResourceMap::iterator it = m_resources.begin(); it != m_resources.end(); ++it)
  {
    if (it->second->Path() == strPath)
    {
      delete it->second;
      m_resources.erase(it);
      break;
    }
  }
}

bool CResources::GetIgnoredPrimitives(const CDevice& deviceInfo, PrimitiveVector& primitives) const
{
  DevicePtr device = GetDevice(deviceInfo);
  if (device)
  {
    primitives = device->Configuration().GetIgnoredPrimitives();
    return true;
  }

  return false;
}

void CResources::SetIgnoredPrimitives(const CDevice& deviceInfo, const PrimitiveVector& primitives)
{
  auto itDevice = m_devices.find(deviceInfo);
  auto itOriginal = m_originalDevices.find(deviceInfo);

  // Ensure resource exists
  if (itDevice == m_devices.end())
  {
    GetResource(deviceInfo, true);
    itDevice = m_devices.find(deviceInfo);
  }

  if (itDevice != m_devices.end())
  {
    // Create a backup to allow revert
    if (itOriginal == m_originalDevices.end())
      m_originalDevices[deviceInfo].reset(new CDevice(*itDevice->second));

    itDevice->second->Configuration().SetIgnoredPrimitives(primitives);
  }
}

void CResources::Revert(const CDevice& deviceInfo)
{
  CButtonMap* resource = GetResource(deviceInfo, false);

  if (resource)
    resource->RevertButtonMap();

  auto itOriginal = m_originalDevices.find(deviceInfo);

  if (itOriginal != m_originalDevices.end())
  {
    m_devices[deviceInfo]->Configuration() = itOriginal->second->Configuration();
    m_originalDevices.erase(itOriginal);
  }
}

// --- CJustABunchOfFiles ------------------------------------------------------

CJustABunchOfFiles::CJustABunchOfFiles(const std::string& strResourcePath,
                                       const std::string& strExtension,
                                       bool bReadWrite,
                                       IDatabaseCallbacks* callbacks) :
  IDatabase(callbacks),
  m_strResourcePath(strResourcePath),
  m_strExtension(strExtension),
  m_bReadWrite(bReadWrite),
  m_resources(this)
{
  m_directoryCache.Initialize(this);

  if (m_bReadWrite)
    CStorageUtils::EnsureDirectoryExists(m_strResourcePath);
}

CJustABunchOfFiles::~CJustABunchOfFiles(void)
{
  m_directoryCache.Deinitialize();
}

const ButtonMap& CJustABunchOfFiles::GetButtonMap(const kodi::addon::Joystick& driverInfo)
{
  static ButtonMap empty;

  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  // Update index
  IndexDirectory(m_strResourcePath, FOLDER_DEPTH);

  CButtonMap* resource = m_resources.GetResource(driverInfo, false);

  if (resource)
    return resource->GetButtonMap();

  return empty;
}

bool CJustABunchOfFiles::MapFeatures(const kodi::addon::Joystick& driverInfo,
                                     const std::string& controllerId,
                                     const FeatureVector& features)
{
  if (!m_bReadWrite)
    return false;

  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  CButtonMap* resource = m_resources.GetResource(driverInfo, true);
  if (resource)
  {
    resource->MapFeatures(controllerId, features);
    return true;
  }

  return false;
}

bool CJustABunchOfFiles::GetIgnoredPrimitives(const kodi::addon::Joystick& driverInfo, PrimitiveVector& primitives)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  // Update index
  IndexDirectory(m_strResourcePath, FOLDER_DEPTH);

  return m_resources.GetIgnoredPrimitives(driverInfo, primitives);
}

bool CJustABunchOfFiles::SetIgnoredPrimitives(const kodi::addon::Joystick& driverInfo, const PrimitiveVector& primitives)
{
  if (!m_bReadWrite)
    return false;

  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  // Ensure resource exists
  m_resources.SetIgnoredPrimitives(driverInfo, primitives);

  return true;
}

bool CJustABunchOfFiles::SaveButtonMap(const kodi::addon::Joystick& driverInfo)
{
  if (!m_bReadWrite)
    return false;

  CDevice device(driverInfo);

  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  CButtonMap* resource = m_resources.GetResource(device, false);

  if (resource)
    return resource->SaveButtonMap();

  return false;
}

bool CJustABunchOfFiles::RevertButtonMap(const kodi::addon::Joystick& driverInfo)
{
  if (!m_bReadWrite)
    return false;

  CDevice device(driverInfo);

  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  m_resources.Revert(device);

  return true;
}

bool CJustABunchOfFiles::ResetButtonMap(const kodi::addon::Joystick& driverInfo, const std::string& controllerId)
{
  if (!m_bReadWrite)
    return false;

  CDevice deviceInfo(driverInfo);

  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  DevicePtr device = m_resources.GetDevice(deviceInfo);
  if (device)
    device->Configuration().Reset();

  CButtonMap* resource = m_resources.GetResource(deviceInfo, false);

  if (resource)
    return resource->ResetButtonMap(controllerId);

  return false;
}

void CJustABunchOfFiles::IndexDirectory(const std::string& path, unsigned int folderDepth)
{
  // Enumerate the directory
  std::vector<kodi::vfs::CDirEntry> items;
  if (!m_directoryCache.GetDirectory(path, items))
    CDirectoryUtils::GetDirectory(path, m_strExtension + "|", items);

  // Recurse into subdirectories
  if (folderDepth > 0)
  {
    for (std::vector<kodi::vfs::CDirEntry>::const_iterator it = items.begin(); it != items.end(); ++it)
    {
      const kodi::vfs::CDirEntry& item = *it;
      if (item.IsFolder())
      {
        IndexDirectory(item.Path(), folderDepth - 1);
      }
    }
  }

  // Erase all folders and resources with different extensions
  items.erase(std::remove_if(items.begin(), items.end(),
    [this](const kodi::vfs::CDirEntry& item)
    {
      return !item.IsFolder() && !kodi::tools::StringUtils::EndsWith(item.Path(), this->m_strExtension);
    }), items.end());

  m_directoryCache.UpdateDirectory(path, items);
}

void CJustABunchOfFiles::OnAdd(const kodi::vfs::CDirEntry& item)
{
  if (!item.IsFolder())
  {
    // TODO: Switch to unique_ptr or shared_ptr
    CButtonMap* resource = CreateResource(item.Path());

    // Load device info
    if (resource && resource->Refresh())
    {
      if (m_resources.AddResource(resource))
        m_callbacks->OnAdd(resource->Device(), resource->GetButtonMap());
      else
        delete resource;
    }
    else
      delete resource;
  }
}

void CJustABunchOfFiles::OnRemove(const kodi::vfs::CDirEntry& item)
{
  m_resources.RemoveResource(item.Path());
}

bool CJustABunchOfFiles::GetResourcePath(const kodi::addon::Joystick& deviceInfo, std::string& resourcePath) const
{
  // Calculate folder path
  std::string strFolder = m_strResourcePath + "/" + deviceInfo.Provider();

  // Calculate resource path
  resourcePath = strFolder + "/" + CStorageUtils::RootFileName(deviceInfo) + m_strExtension;

  // Ensure folder path exists
  return CStorageUtils::EnsureDirectoryExists(strFolder);
}

DevicePtr CJustABunchOfFiles::CreateDevice(const CDevice& deviceInfo) const
{
  if (Callbacks())
    return Callbacks()->CreateDevice(deviceInfo);

  return std::make_shared<CDevice>(deviceInfo);
}
