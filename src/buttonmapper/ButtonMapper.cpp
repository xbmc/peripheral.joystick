/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ButtonMapper.h"
#include "addon.h"
#include "ControllerTransformer.h"
#include "storage/IDatabase.h"

#include <kodi/addon-instance/peripheral/PeripheralUtils.h>

#include <algorithm>
#include <iterator>

using namespace JOYSTICK;

CButtonMapper::CButtonMapper(CPeripheralJoystick* peripheralLib) :
  m_peripheralLib(peripheralLib)
{
}

CButtonMapper::~CButtonMapper()
{
}

bool CButtonMapper::Initialize(CJoystickFamilyManager& familyManager)
{
  m_controllerTransformer.reset(new CControllerTransformer(familyManager));
  return true;
}

void CButtonMapper::Deinitialize()
{
  m_controllerTransformer.reset();
  m_databases.clear();
}

IDatabaseCallbacks* CButtonMapper::GetCallbacks()
{
  return m_controllerTransformer.get();
}

bool CButtonMapper::GetFeatures(const kodi::addon::Joystick& joystick,
                                const std::string& strControllerId,
                                FeatureVector& features)
{
  // Accumulate available button maps for this device
  ButtonMap accumulatedMap = GetButtonMap(joystick);

  GetFeatures(joystick, std::move(accumulatedMap), strControllerId, features);

  return !features.empty();
}

ButtonMap CButtonMapper::GetButtonMap(const kodi::addon::Joystick& joystick) const
{
  ButtonMap accumulatedMap;

  for (DatabaseVector::const_iterator it = m_databases.begin(); it != m_databases.end(); ++it)
  {
    const ButtonMap& buttonMap = (*it)->GetButtonMap(joystick);
    MergeButtonMap(accumulatedMap, buttonMap);
  }

  return accumulatedMap;
}

void CButtonMapper::MergeButtonMap(ButtonMap& accumulatedMap, const ButtonMap& newFeatures)
{
  for (auto it = newFeatures.begin(); it != newFeatures.end(); ++it)
  {
    const std::string& controllerId = it->first;
    const FeatureVector& features = it->second;

    MergeFeatures(accumulatedMap[controllerId], features);
  }
}

void CButtonMapper::MergeFeatures(FeatureVector& features, const FeatureVector& newFeatures)
{
  for (const kodi::addon::JoystickFeature& newFeature : newFeatures)
  {
    // Check for duplicate feature name
    bool bFound = std::find_if(features.begin(), features.end(),
      [&newFeature](const kodi::addon::JoystickFeature& feature)
      {
        return feature.Name() == newFeature.Name();
      }) != features.end();

    // Check for duplicate driver primitives
    if (!bFound)
    {
      const auto& newPrimitives = newFeature.Primitives();

      bFound = std::find_if(features.begin(), features.end(),
        [&newPrimitives](const kodi::addon::JoystickFeature& feature)
        {
          for (const auto& primitive : feature.Primitives())
          {
            if (primitive.Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN)
              continue;

            if (std::find(newPrimitives.begin(), newPrimitives.end(), primitive) != newPrimitives.end())
              return true; // Found primitive
          }
          return false; // Didn't find primitive
        }) != features.end();
    }

    if (!bFound)
      features.push_back(newFeature);
  }
}

bool CButtonMapper::GetFeatures(const kodi::addon::Joystick& joystick, ButtonMap buttonMap, const std::string& controllerId, FeatureVector& features)
{
  // Try to get a button map for the specified controller profile
  auto itController = buttonMap.find(controllerId);
  if (itController != buttonMap.end())
    features = std::move(itController->second);

  bool bNeedsFeatures = false;

  if (features.empty())
  {
    bNeedsFeatures = true;
  }
  else if (m_peripheralLib)
  {
    unsigned int featureCount = m_peripheralLib->FeatureCount(controllerId);
    if (featureCount > 0)
      bNeedsFeatures = (features.size() < featureCount);
  }

  // Try to derive a button map from relations between controller profiles
  if (bNeedsFeatures)
  {
    FeatureVector derivedFeatures;
    DeriveFeatures(joystick, controllerId, buttonMap, derivedFeatures);
    MergeFeatures(features, derivedFeatures);
  }

  return !features.empty();
}

void CButtonMapper::DeriveFeatures(const kodi::addon::Joystick& joystick, const std::string& toController, const ButtonMap& buttonMap, FeatureVector& transformedFeatures)
{
  if (!m_controllerTransformer)
    return;

  // Search the button map for the controller with the highest count of features defined
  unsigned int maxFeatures = 0;
  auto maxFeaturesIt = buttonMap.end();

  for (auto it = buttonMap.begin(); it != buttonMap.end(); ++it)
  {
    const unsigned int featureCount = static_cast<unsigned int>(it->second.size());
    if (featureCount > maxFeatures)
    {
      maxFeatures = featureCount;
      maxFeaturesIt = it;
    }
  }

  if (maxFeaturesIt != buttonMap.end())
  {
    // Transform the controller profile with the most features to the specified controller
    const std::string& fromController = maxFeaturesIt->first;
    const FeatureVector& features = maxFeaturesIt->second;

    m_controllerTransformer->TransformFeatures(joystick, fromController, toController, features, transformedFeatures);
  }
}

void CButtonMapper::RegisterDatabase(const DatabasePtr& database)
{
  if (std::find(m_databases.begin(), m_databases.end(), database) == m_databases.end())
    m_databases.push_back(database);
}

void CButtonMapper::UnregisterDatabase(const DatabasePtr& database)
{
  m_databases.erase(std::remove(m_databases.begin(), m_databases.end(), database), m_databases.end());
}
