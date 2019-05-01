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

#include "ButtonMap.h"
#include "Device.h"
#include "DeviceConfiguration.h"
#include "StorageManager.h"
#include "StorageUtils.h"
#include "buttonmapper/ButtonMapUtils.h"
#include "log/Log.h"

#include <kodi/addon-instance/PeripheralUtils.h>

#include <algorithm>
#include <chrono>

using namespace JOYSTICK;

static constexpr std::chrono::seconds RESOURCE_LIFETIME = std::chrono::seconds(2);

CButtonMap::CButtonMap(const std::string& strResourcePath, IControllerHelper *controllerHelper) :
  m_strResourcePath(strResourcePath),
  m_device(std::move(std::make_shared<CDevice>())),
  m_bModified(false),
  m_controllerHelper(controllerHelper)
{
}

CButtonMap::CButtonMap(const std::string& strResourcePath, const DevicePtr& device, IControllerHelper *controllerHelper) :
  m_strResourcePath(strResourcePath),
  m_device(device),
  m_bModified(false),
  m_controllerHelper(controllerHelper)
{
}

bool CButtonMap::IsValid(void) const
{
  return m_device->IsValid();
}

const ButtonMap& CButtonMap::GetButtonMap()
{
  if (!m_bModified)
    Refresh();

  return m_buttonMap;
}

void CButtonMap::MapFeatures(const std::string& controllerId, const FeatureVector& features)
{
  // Create a backup to allow revert
  if (m_originalButtonMap.empty())
    m_originalButtonMap = m_buttonMap;

  // Update axis configurations
  m_device->Configuration().SetAxisConfigs(features);

  // Merge new features
  FeatureVector& myFeatures = m_buttonMap[controllerId];
  for (const auto& newFeature : features)
  {
    MergeFeature(newFeature, myFeatures, controllerId);
    m_bModified = true;
  }

  std::sort(myFeatures.begin(), myFeatures.end(),
    [](const kodi::addon::JoystickFeature& lhs, const kodi::addon::JoystickFeature& rhs)
    {
      return lhs.Name() < rhs.Name();
    });
}

bool CButtonMap::SaveButtonMap()
{
  if (Save())
  {
    m_timestamp = std::chrono::steady_clock::now();
    m_originalButtonMap.clear();
    m_bModified = false;
    return true;
  }

  return false;
}

bool CButtonMap::RevertButtonMap()
{
  if (!m_originalButtonMap.empty())
  {
    m_buttonMap = m_originalButtonMap;
    return true;
  }

  return false;
}

bool CButtonMap::ResetButtonMap(const std::string& controllerId)
{
  FeatureVector& features = m_buttonMap[controllerId];

  if (!features.empty())
  {
    features.clear();
    return SaveButtonMap();
  }

  return false;
}

bool CButtonMap::Refresh(void)
{
  const std::chrono::steady_clock::time_point expires = m_timestamp + RESOURCE_LIFETIME;
  const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

  if (now >= expires)
  {
    if (!Load())
      return false;

    for (auto it = m_buttonMap.begin(); it != m_buttonMap.end(); ++it)
    {
      // Transfer axis configs from device configuration to features' primitives
      m_device->Configuration().GetAxisConfigs(it->second);

      Sanitize(it->second, it->first);
    }

    m_timestamp = now;
    m_originalButtonMap.clear();
  }

  return true;
}

void CButtonMap::MergeFeature(const kodi::addon::JoystickFeature& feature, FeatureVector& features, const std::string& controllerId)
{
  // Find existing feature with the same name being updated
  auto itUpdating = std::find_if(features.begin(), features.end(),
    [&feature](const kodi::addon::JoystickFeature& existingFeature)
    {
      return existingFeature.Name() == feature.Name();
    });

  if (itUpdating != features.end())
  {
    // Find existing feature with the same primitives
    auto itConflicting = std::find_if(features.begin(), features.end(),
      [&feature](const kodi::addon::JoystickFeature& existingFeature)
      {
        return ButtonMapUtils::PrimitivesEqual(existingFeature, feature);
      });

    // Assign conflicting primitives to the primitives of the feature being updated
    if (itConflicting != features.end())
      itConflicting->Primitives() = itUpdating->Primitives();

    // Erase the feature being updated and place it at the front of the list
    features.erase(itUpdating);
  }

  features.insert(features.begin(), feature);

  Sanitize(features, controllerId);
}

void CButtonMap::Sanitize(FeatureVector& features, const std::string& controllerId)
{
  // Loop through features and reset duplicate primitives
  for (unsigned int iFeature = 0; iFeature < features.size(); ++iFeature)
  {
    auto& feature = features[iFeature];

    // Loop through feature's primitives
    auto& primitives = feature.Primitives();
    for (unsigned int iPrimitive = 0; iPrimitive < primitives.size(); ++iPrimitive)
    {
      auto& primitive = primitives[iPrimitive];

      if (primitive.Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN)
        continue;

      bool bFound = false;

      // Search for prior feature with the primitive
      kodi::addon::JoystickFeature existingFeature;

      for (unsigned int iExistingFeature = 0; iExistingFeature < iFeature; ++iExistingFeature)
      {
        const auto& existingPrimitives = features[iExistingFeature].Primitives();

        bFound = std::find_if(existingPrimitives.begin(), existingPrimitives.end(),
          [&primitive](const kodi::addon::DriverPrimitive& existing)
          {
            return ButtonMapUtils::PrimitivesConflict(primitive, existing);
          }) != existingPrimitives.end();

        if (bFound)
        {
          existingFeature = features[iExistingFeature];
          break;
        }
      }

      if (!bFound)
      {
        // Search for primitive in prior primitives
        for (unsigned int iExistingPrimitive = 0; iExistingPrimitive < iPrimitive; ++iExistingPrimitive)
        {
          if (ButtonMapUtils::PrimitivesConflict(primitives[iExistingPrimitive], primitive))
          {
            existingFeature = feature;
            bFound = true;
            break;
          }
        }
      }

      // Invalid the primitive if it has already been seen
      if (bFound)
      {
        esyslog("%s: %s (%s) conflicts with %s (%s)",
            controllerId.c_str(),
            CStorageUtils::PrimitiveToString(primitive).c_str(),
            existingFeature.Name().c_str(),
            CStorageUtils::PrimitiveToString(primitive).c_str(),
            feature.Name().c_str());

        primitive = kodi::addon::DriverPrimitive();
      }
    }
  }

  // Erase invalid features
  features.erase(std::remove_if(features.begin(), features.end(),
    [&controllerId](const kodi::addon::JoystickFeature& feature)
    {
      auto& primitives = feature.Primitives();

      // Find valid primitive
      auto it = std::find_if(primitives.begin(), primitives.end(),
        [](const kodi::addon::DriverPrimitive& primitive)
        {
          return primitive.Type() != JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN;
        });

      const bool bIsValid = (it != primitives.end());

      if (!bIsValid)
      {
        dsyslog("%s: Removing %s from button map", controllerId.c_str(), feature.Name().c_str());
        return true;
      }

      return false;
    }), features.end());
}
