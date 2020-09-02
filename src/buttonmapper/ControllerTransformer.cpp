/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ControllerTransformer.h"
#include "ButtonMapUtils.h"
#include "StringRegistry.h"
#include "storage/Device.h"
#include "utils/CommonMacros.h"

#include <kodi/addon-instance/peripheral/PeripheralUtils.h>

#include <algorithm>

using namespace JOYSTICK;

// --- Utility definitions -----------------------------------------------------

namespace JOYSTICK
{
  struct FeatureMapProperties
  {
    unsigned int occurrences;
    unsigned int primitiveCount;
  };

  bool operator<(const FeatureMapProperties& lhs, const FeatureMapProperties& rhs)
  {
    if (lhs.occurrences < rhs.occurrences) return true;
    if (lhs.occurrences > rhs.occurrences) return false;

    if (lhs.primitiveCount < rhs.primitiveCount) return true;
    if (lhs.primitiveCount > rhs.primitiveCount) return false;

    return false;
  }
}

// --- CControllerTransformer --------------------------------------------------

CControllerTransformer::CControllerTransformer(CJoystickFamilyManager& familyManager) :
  m_familyManager(familyManager),
  m_controllerIds(new CStringRegistry)
{
}

CControllerTransformer::~CControllerTransformer() = default;

void CControllerTransformer::OnAdd(const DevicePtr& driverInfo, const ButtonMap& buttonMap)
{
  // Santity check
  if (m_observedDevices.size() > 200)
    return;

  // Skip devices we've already encountered.
  if (m_observedDevices.find(driverInfo) != m_observedDevices.end())
    return;

  m_observedDevices.insert(driverInfo);

  for (auto itTo = buttonMap.begin(); itTo != buttonMap.end(); ++itTo)
  {
    // Only allow controller map items where "from" compares before "to"
    for (auto itFrom = buttonMap.begin(); itFrom->first < itTo->first; ++itFrom)
    {
      AddControllerMap(itFrom->first, itFrom->second, itTo->first, itTo->second);
    }
  }
}

DevicePtr CControllerTransformer::CreateDevice(const CDevice& deviceInfo)
{
  DevicePtr result = std::make_shared<CDevice>(deviceInfo);

  for (const auto& device : m_observedDevices)
  {
    if (*device == deviceInfo)
    {
      result->Configuration() = device->Configuration();
      break;
    }
  }

  return result;
}

void CControllerTransformer::AddControllerMap(const std::string& controllerFrom, const FeatureVector& featuresFrom,
                                              const std::string& controllerTo, const FeatureVector& featuresTo)
{
  const bool bSwap = (controllerFrom >= controllerTo);

  const unsigned int fromController = m_controllerIds->RegisterString(controllerFrom);
  const unsigned int toController = m_controllerIds->RegisterString(controllerTo);

  ControllerTranslation key = { bSwap ? toController : fromController,
                                bSwap ? fromController : toController };

  FeatureMaps& featureMaps = m_controllerMap[key];

  FeatureMap featureMap = CreateFeatureMap(bSwap ? featuresTo : featuresFrom,
                                           bSwap ? featuresFrom : featuresTo);

  auto it = featureMaps.find(featureMap);

  if (it == featureMaps.end())
  {
    featureMaps.insert(std::make_pair(std::move(featureMap), 1));
  }
  else
  {
    ++it->second;
  }
}

FeatureMap CControllerTransformer::CreateFeatureMap(const FeatureVector& featuresFrom, const FeatureVector& featuresTo)
{
  FeatureMap featureMap;

  for (const kodi::addon::JoystickFeature& featureFrom : featuresFrom)
  {
    for (JOYSTICK_FEATURE_PRIMITIVE primitiveIndex : ButtonMapUtils::GetPrimitives(featureFrom.Type()))
    {
      const kodi::addon::DriverPrimitive& targetPrimitive = featureFrom.Primitive(primitiveIndex);

      if (targetPrimitive.Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN)
        continue;

      JOYSTICK_FEATURE_PRIMITIVE toPrimitiveIndex;

      auto itFeatureTo = std::find_if(featuresTo.begin(), featuresTo.end(),
        [&targetPrimitive, &toPrimitiveIndex](const kodi::addon::JoystickFeature& featureTo)
        {
          for (JOYSTICK_FEATURE_PRIMITIVE toIndex : ButtonMapUtils::GetPrimitives(featureTo.Type()))
          {
            if (targetPrimitive == featureTo.Primitive(toIndex))
            {
              toPrimitiveIndex = toIndex;
              return true;
            }
          }
          return false;
        });

      if (itFeatureTo != featuresTo.end())
      {
        FeaturePrimitive fromPrimitive = { featureFrom, primitiveIndex };
        FeaturePrimitive toPrimitive = { *itFeatureTo, toPrimitiveIndex };

        featureMap.insert(std::make_pair(std::move(fromPrimitive), std::move(toPrimitive)));
      }
    }
  }

  return featureMap;
}

void CControllerTransformer::TransformFeatures(const kodi::addon::Joystick& driverInfo,
                                               const std::string& fromController,
                                               const std::string& toController,
                                               const FeatureVector& features,
                                               FeatureVector& transformedFeatures)
{
  const bool bSwap = (fromController >= toController);

  const unsigned int controllerFrom = m_controllerIds->RegisterString(fromController);
  const unsigned int controllerTo = m_controllerIds->RegisterString(toController);

  ControllerTranslation key = { bSwap ? controllerTo : controllerFrom,
                                bSwap ? controllerFrom : controllerTo };

  const FeatureMaps& featureMaps = m_controllerMap[key];

  const FeatureMap& featureMap = GetFeatureMap(featureMaps);

  for (const kodi::addon::JoystickFeature& sourceFeature : features)
  {
    for (JOYSTICK_FEATURE_PRIMITIVE primitiveIndex : ButtonMapUtils::GetPrimitives(sourceFeature.Type()))
    {
      const kodi::addon::DriverPrimitive& sourcePrimitive = sourceFeature.Primitive(primitiveIndex);

      if (sourcePrimitive.Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN)
        continue;

      kodi::addon::JoystickFeature targetFeature;
      JOYSTICK_FEATURE_PRIMITIVE targetPrimitive;

      if (TranslatePrimitive(sourceFeature, primitiveIndex, targetFeature, targetPrimitive, featureMap, bSwap))
        SetPrimitive(transformedFeatures, targetFeature, targetPrimitive, sourcePrimitive);
    }
  }
}

const FeatureMap& CControllerTransformer::GetFeatureMap(const FeatureMaps& featureMaps)
{
  static const FeatureMap empty;

  std::map<FeatureMapProperties, const FeatureMap*> sortedFeatureMaps;

  for (const auto& it : featureMaps)
  {
    const FeatureMap& featureMap = it.first;
    unsigned int occurrenceCount = it.second;

    FeatureMapProperties props = { static_cast<unsigned int>(featureMap.size()), occurrenceCount };

    sortedFeatureMaps[props] = &featureMap;
  }

  if (!sortedFeatureMaps.empty())
  {
    const FeatureMap* featureMap = sortedFeatureMaps.rbegin()->second;
    return *featureMap;
  }

  return empty;
}

bool CControllerTransformer::TranslatePrimitive(const kodi::addon::JoystickFeature& sourceFeature,
                                                JOYSTICK_FEATURE_PRIMITIVE sourcePrimitive,
                                                kodi::addon::JoystickFeature& targetFeature,
                                                JOYSTICK_FEATURE_PRIMITIVE& targetPrimitive,
                                                const FeatureMap& featureMap,
                                                bool bSwap)
{
  auto itFeatureMap = std::find_if(featureMap.begin(), featureMap.end(),
    [&sourceFeature, sourcePrimitive, bSwap](const std::pair<FeaturePrimitive, FeaturePrimitive>& featureEntry)
    {
      if (bSwap)
      {
        return sourceFeature.Name() == featureEntry.second.feature.Name() &&
               sourcePrimitive == featureEntry.second.primitive;
      }
      else
      {
        return sourceFeature.Name() == featureEntry.first.feature.Name() &&
               sourcePrimitive == featureEntry.first.primitive;
      }
    });

  if (itFeatureMap != featureMap.end())
  {
    targetFeature = bSwap ? itFeatureMap->first.feature :
                            itFeatureMap->second.feature;
    targetPrimitive = bSwap ? itFeatureMap->first.primitive :
                              itFeatureMap->second.primitive;
    return true;
  }

  return false;
}

void CControllerTransformer::SetPrimitive(FeatureVector& features,
                                          const kodi::addon::JoystickFeature& feature,
                                          JOYSTICK_FEATURE_PRIMITIVE index,
                                          const kodi::addon::DriverPrimitive& primitive)
{
  auto itFeature = std::find_if(features.begin(), features.end(),
    [&feature](const kodi::addon::JoystickFeature& targetFeature)
    {
      return feature.Name() == targetFeature.Name();
    });

  if (itFeature == features.end())
  {
    kodi::addon::JoystickFeature newFeature(feature.Name(), feature.Type());
    newFeature.SetPrimitive(index, primitive);
    features.emplace_back(std::move(newFeature));
  }
  else
  {
    itFeature->SetPrimitive(index, primitive);
  }
}
