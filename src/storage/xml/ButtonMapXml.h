/*
 *  Copyright (C) 2015-2021 Garrett Brown
 *  Copyright (C) 2015-2021 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "storage/ButtonMap.h"

#include <string>

namespace tinyxml2
{
class XMLElement;
}

namespace kodi
{
namespace addon
{
  struct DriverPrimitive;
  class JoystickFeature;
}
}

namespace JOYSTICK
{
  class CAnomalousTrigger;
  class CButtonMap;
  class IControllerHelper;

  class CButtonMapXml : public CButtonMap
  {
  public:
    CButtonMapXml(const std::string& strResourcePath, IControllerHelper *controllerHelper);
    CButtonMapXml(const std::string& strResourcePath, const DevicePtr& device, IControllerHelper *controllerHelper);

    virtual ~CButtonMapXml(void) { }

  protected:
    // implementation of CButtonMap
    virtual bool Load(void) override;
    virtual bool Save(void) const override;

  private:
    bool SerializeButtonMaps(tinyxml2::XMLElement* pElement) const;

    bool Serialize(const FeatureVector& features, tinyxml2::XMLElement* pElement) const;
    bool Deserialize(const tinyxml2::XMLElement* pElement, FeatureVector& features, const std::string& controllerId) const;

    static bool IsValid(const kodi::addon::JoystickFeature& feature);
    static bool SerializeFeature(tinyxml2::XMLElement* pElement,
                                 const kodi::addon::DriverPrimitive& primitive,
                                 const char* tagName);
    static bool SerializePrimitiveTag(tinyxml2::XMLElement* pElement,
                                      const kodi::addon::DriverPrimitive& primitive,
                                      const char* tagName);
    static void SerializePrimitive(tinyxml2::XMLElement* pElement, const kodi::addon::DriverPrimitive& primitive);
    static bool DeserializePrimitive(const tinyxml2::XMLElement* pElement, kodi::addon::DriverPrimitive& primitive);
  };
}
