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
#include "buttonmapper/JoystickFamily.h"
#include "utils/CommonMacros.h"

#include <memory>
#include <string>

class CPeripheralJoystick;
struct AddonProps_Peripheral;

namespace kodi
{
namespace addon
{
  class Joystick;
}
}

namespace JOYSTICK
{
  /*!
   * \brief Helper functions for dealing with controllers and controller features
   */
  class IControllerHelper
  {
  public:
    virtual ~IControllerHelper() = default;

    /*!
     * \brief Return the type of the specified feature
     *
     * \param controllerId    The controller ID to check
     * \param featureName     The feature to check
     *
     * \return The type of the feature, or JOYSTICK_FEATURE_TYPE_UNKNOWN if unknown
     */
    virtual JOYSTICK_FEATURE_TYPE FeatureType(const std::string& strControllerId, const std::string &featureName) = 0;
  };

  class CButtonMapper;
  class CDevice;
  class IDatabase;

  class DLL_PRIVATE CStorageManager : public IControllerHelper
  {
  private:
    CStorageManager(void);

  public:
    static CStorageManager& Get(void);

    ~CStorageManager(void);

    /*!
     * \brief Initialize storage manager
     *
     * \param peripheralLib The peripheral API helper library
     * \param props used in add-on creation (TODO: Change to two strings)
     *
     * \return true if the storage manager has been initialized and can be safely used
     */
    bool Initialize(CPeripheralJoystick* peripheralLib);

    /*!
     * \brief Deinitialize storage manager
     */
    void Deinitialize(void);

    /*!
     * \brief Get the map of features to driver primitives from a storage backend
     *
     * \param joystick      The device's joystick properties; unknown values may be left at their default
     * \param controller_id The controller profile being requested, e.g. game.controller.default
     * \param features      The array of features and their driver primitives
     */
    void GetFeatures(const kodi::addon::Joystick& joystick,
                     const std::string& strDeviceId,
                     FeatureVector& features);

    /*!
     * \brief Update button maps
     *
     * \param joystick      The device's joystick properties; unknown values may be left at their default
     * \param controller_id The game controller profile being updated
     * \param features      The array of features and their driver primitives
     *
     * \return true if features were mapped in a storage backend
     */
    bool MapFeatures(const kodi::addon::Joystick& joystick,
                     const std::string& strDeviceId,
                     const FeatureVector& features);

    /*!
     * \brief Get the ignored primitives from a storage backend
     *
     * \param joystick      The device's joystick properties; unknown values may be left at their default
     * \param primitives    The array of driver primitives
     *
     * \return true if results were loaded from a storage backend
     */
    void GetIgnoredPrimitives(const kodi::addon::Joystick& joystick, PrimitiveVector& primitives);

    /*!
     * \brief Update the list of ignored driver primitives
     *
     * \param joystick      The device's joystick properties; unknown values may be left at their default
     * \param primitives    The array of driver primitives
     *
     * \return true if driver primitives were set in a storage backend
     */
    bool SetIgnoredPrimitives(const kodi::addon::Joystick& joystick, const PrimitiveVector& primitives);

    /*!
     * \brief Save the button map for the specified device
     *
     * \param deviceName The name of the device to reset
     *
     * \return true if the underlying storage was modified, false otherwise
     */
    bool SaveButtonMap(const kodi::addon::Joystick& joystick);

    /*!
     * \brief Revert the button map to the last time it was loaded or committed to disk
     *
     * \param deviceName The name of the device to revert
     * \param controllerId The controller ID to revert
     *
     * \return true if the underlying storage was modified, false otherwise
     */
    bool RevertButtonMap(const kodi::addon::Joystick& joystick);

    /*!
     * \brief Reset the button map for the specified device and controller profile
     *
     * \param deviceName The name of the device to reset
     * \param controllerId The controller ID to reset
     *
     * \return true if the underlying storage was modified, false otherwise
     */
    bool ResetButtonMap(const kodi::addon::Joystick& joystick, const std::string& strControllerId);

    /*!
     * \brief Notify the frontend that button maps have changed
     *
     * \param[optional] deviceName The name of the device to refresh, or empty for all devices
     * \param[optional] controllerId The controller ID to refresh, or empty for all controllers
     */
    void RefreshButtonMaps(const std::string& strDeviceName = "");

    // implementation of IControllerHelper
    virtual JOYSTICK_FEATURE_TYPE FeatureType(const std::string& strControllerId, const std::string &featureName) override;

  private:
    CPeripheralJoystick* m_peripheralLib;

    DatabaseVector                 m_databases;
    std::unique_ptr<CButtonMapper> m_buttonMapper;
    CJoystickFamilyManager         m_familyManager;
  };
}
