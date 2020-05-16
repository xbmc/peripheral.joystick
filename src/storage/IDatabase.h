/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
