/*
 *      Copyright (C) 2016-2017 Garrett Brown
 *      Copyright (C) 2016-2017 Team Kodi
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

#include "ButtonMapTypes.h"
#include "JoystickFamily.h"
#include "storage/IDatabase.h"

#include <kodi/addon-instance/Peripheral.h>

#include <string>

namespace kodi
{
namespace addon
{
  struct DriverPrimitive;
  class Joystick;
}
}

namespace JOYSTICK
{
  class CJoystickFamilyManager;
  class CStringRegistry;

  class CControllerTransformer : public IDatabaseCallbacks
  {
  public:
    CControllerTransformer(CJoystickFamilyManager& familyManager);

    virtual ~CControllerTransformer();

    // implementation of IDatabaseCallbacks
    virtual void OnAdd(const DevicePtr& driverInfo, const ButtonMap& buttonMap) override;
    virtual DevicePtr CreateDevice(const CDevice& deviceInfo) override;

    void TransformFeatures(const kodi::addon::Joystick& driverInfo,
                           const std::string& fromController,
                           const std::string& toController,
                           const FeatureVector& features,
                           FeatureVector& transformedFeatures);

  private:
    void AddControllerMap(const std::string& controllerFrom, const FeatureVector& featuresFrom,
                          const std::string& controllerTo, const FeatureVector& featuresTo);

    static FeatureMap CreateFeatureMap(const FeatureVector& featuresFrom, const FeatureVector& featuresTo);

    static const FeatureMap& GetFeatureMap(const FeatureMaps& featureMaps);

    static bool TranslatePrimitive(const kodi::addon::JoystickFeature& sourceFeature,
                                   JOYSTICK_FEATURE_PRIMITIVE sourcePrimitive,
                                   kodi::addon::JoystickFeature& targetFeature,
                                   JOYSTICK_FEATURE_PRIMITIVE& targetPrimitive,
                                   const FeatureMap& featureMap,
                                   bool bSwap);

    static void SetPrimitive(FeatureVector& features,
                             const kodi::addon::JoystickFeature& feature,
                             JOYSTICK_FEATURE_PRIMITIVE index,
                             const kodi::addon::DriverPrimitive& primitive);

    ControllerMap           m_controllerMap;
    DeviceSet               m_observedDevices;
    CJoystickFamilyManager& m_familyManager;
    std::unique_ptr<CStringRegistry> m_controllerIds;
  };
}
