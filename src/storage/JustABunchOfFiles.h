/*
 *      Copyright (C) 2015-2017 Garrett Brown
 *      Copyright (C) 2015-2017 Team Kodi
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
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "ButtonMap.h"
#include "Device.h"
#include "IDatabase.h"
#include "filesystem/DirectoryCache.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace JOYSTICK
{
  class CJustABunchOfFiles;

  /*!
   * \brief Container class for device records and button maps
   */
  class CResources
  {
  public:
    CResources(const CJustABunchOfFiles* database);
    ~CResources(void);

    DevicePtr GetDevice(const CDevice& deviceInfo) const;

    CButtonMap* GetResource(const CDevice& deviceInfo, bool bCreate);
    bool AddResource(CButtonMap* resource);
    void RemoveResource(const std::string& strPath);

    bool GetIgnoredPrimitives(const CDevice& deviceInfo, PrimitiveVector& primitives) const;
    void SetIgnoredPrimitives(const CDevice& deviceInfo, const PrimitiveVector& primitives);

    void Revert(const CDevice& deviceInfo);

  private:
    typedef std::map<CDevice, DevicePtr>   DeviceMap;
    typedef std::map<CDevice, CButtonMap*> ResourceMap;

    // Construction parameters
    const CJustABunchOfFiles* const m_database;

    // Resource parameters
    DeviceMap   m_devices;
    DeviceMap   m_originalDevices;
    ResourceMap m_resources;
  };

  class CJustABunchOfFiles : public IDatabase,
                             public IDirectoryCacheCallback
  {
  public:
    CJustABunchOfFiles(const std::string& strResourcePath,
                       const std::string& strExtension,
                       bool bReadWrite,
                       IDatabaseCallbacks* callbacks);

    virtual ~CJustABunchOfFiles(void);

    // implementation of IDatabase
    virtual const ButtonMap& GetButtonMap(const kodi::addon::Joystick& driverInfo) override;
    virtual bool MapFeatures(const kodi::addon::Joystick& driverInfo,
                             const std::string& controllerId,
                             const FeatureVector& features) override;
    virtual bool GetIgnoredPrimitives(const kodi::addon::Joystick& driverInfo, PrimitiveVector& primitives) override;
    virtual bool SetIgnoredPrimitives(const kodi::addon::Joystick& driverInfo, const PrimitiveVector& primitives) override;
    virtual bool SaveButtonMap(const kodi::addon::Joystick& driverInfo) override;
    virtual bool RevertButtonMap(const kodi::addon::Joystick& driverInfo) override;
    virtual bool ResetButtonMap(const kodi::addon::Joystick& driverInfo,
                                const std::string& controllerId) override;

    // implementation of IDirectoryCacheCallback
    virtual void OnAdd(const kodi::vfs::CDirEntry& item) override;
    virtual void OnRemove(const kodi::vfs::CDirEntry& item) override;

    // Interface for child class to provide
    virtual CButtonMap* CreateResource(const std::string& resourcePath) const = 0;
    virtual CButtonMap* CreateResource(const std::string& resourcePath, const DevicePtr& driverInfo) const = 0;

    /*!
     * \brief Calculate and create a path for a device record
     *
     * The path is created if it doesn't exist.
     *
     * \return true if the path exists or was created
     */
    bool GetResourcePath(const kodi::addon::Joystick& deviceInfo, std::string& resourcePath) const;

    DevicePtr CreateDevice(const CDevice& deviceInfo) const;

  private:
    /*!
     * \brief Recursively index a path, enumerating the folder and updating
     *        the directory cache
     */
    void IndexDirectory(const std::string& path, unsigned int folderDepth);

    const std::string m_strResourcePath;
    const std::string m_strExtension;
    const bool        m_bReadWrite;
    CDirectoryCache   m_directoryCache;
    CResources        m_resources;
    std::mutex m_mutex;
  };
}
