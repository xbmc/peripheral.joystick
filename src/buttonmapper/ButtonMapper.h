/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "ButtonMapTypes.h"
#include "storage/StorageTypes.h"

#include <memory>
#include <string>

class CPeripheralJoystick;

namespace kodi
{
namespace addon
{
  class Joystick;
}
}

namespace JOYSTICK
{
  class CControllerTransformer;
  class CJoystickFamilyManager;
  class IDatabaseCallbacks;

  class CButtonMapper
  {
  public:
    CButtonMapper(CPeripheralJoystick* peripheralLib);
    ~CButtonMapper();

    bool Initialize(CJoystickFamilyManager& familyManager);
    void Deinitialize();

    IDatabaseCallbacks* GetCallbacks();

    bool GetFeatures(const kodi::addon::Joystick& joystick, const std::string& strDeviceId, FeatureVector& features);

    void RegisterDatabase(const DatabasePtr& database);
    void UnregisterDatabase(const DatabasePtr& database);

  private:
    ButtonMap GetButtonMap(const kodi::addon::Joystick& joystick) const;
    static void MergeButtonMap(ButtonMap& accumulatedMap, const ButtonMap& newFeatures);
    static void MergeFeatures(FeatureVector& features, const FeatureVector& newFeatures);
    bool GetFeatures(const kodi::addon::Joystick& joystick, ButtonMap buttonMap, const std::string& controllerId, FeatureVector& features);
    void DeriveFeatures(const kodi::addon::Joystick& joystick, const std::string& toController, const ButtonMap& buttonMap, FeatureVector& transformedFeatures);

    DatabaseVector    m_databases;
    std::unique_ptr<CControllerTransformer> m_controllerTransformer;

    CPeripheralJoystick* m_peripheralLib;
  };
}
