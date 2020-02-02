/*
 *      Copyright (C) 2015-2020 Garrett Brown
 *      Copyright (C) 2015-2020 Team Kodi
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

#include "StorageTypes.h"
#include "buttonmapper/ButtonMapTypes.h"

#include <string>

namespace kodi
{
namespace addon
{
  class Joystick;
}
}

namespace JOYSTICK
{
  class CDevice;

  class IDatabaseCallbacks
  {
  public:
    virtual ~IDatabaseCallbacks() = default;

    virtual void OnAdd(const DevicePtr& driverInfo, const ButtonMap& buttonMap) = 0;

    virtual DevicePtr CreateDevice(const CDevice& deviceInfo) = 0;
  };

  class IDatabase
  {
  public:
    IDatabase(IDatabaseCallbacks* callbacks) : m_callbacks(callbacks) { }

    virtual ~IDatabase(void) { }

    /*!
     * \copydoc CStorageManager::GetFeatures()
     */
    virtual const ButtonMap& GetButtonMap(const kodi::addon::Joystick& driverInfo) = 0;

    /*!
     * \copydoc CStorageManager::MapFeatures()
     */
    virtual bool MapFeatures(const kodi::addon::Joystick& driverInfo,
                             const std::string& controllerId,
                             const FeatureVector& features) = 0;

    /*!
     * \copydoc CStorageManager::GetIgnoredPrimitives()
     */
    virtual bool GetIgnoredPrimitives(const kodi::addon::Joystick& driverInfo, PrimitiveVector& primitives) = 0;

    /*!
     * \copydoc CStorageManager::SetIgnoredPrimitives()
     */
    virtual bool SetIgnoredPrimitives(const kodi::addon::Joystick& driverInfo, const PrimitiveVector& primitives) = 0;

    /*!
     * \copydoc CStorageManager::SaveButtonMap()
     */
    virtual bool SaveButtonMap(const kodi::addon::Joystick& driverInfo) = 0;

    /*!
     * \copydoc CStorageManager::RevertButtonMap()
     */
    virtual bool RevertButtonMap(const kodi::addon::Joystick& driverInfo) = 0;

    /*!
     * \copydoc CStorageManager::ResetButtonMap()
     */
    virtual bool ResetButtonMap(const kodi::addon::Joystick& driverInfo,
                                const std::string& controllerId) = 0;

    IDatabaseCallbacks* Callbacks() const { return m_callbacks; }

  protected:
    IDatabaseCallbacks* const m_callbacks;
  };
}
