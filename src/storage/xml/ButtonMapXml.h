/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "storage/ButtonMap.h"

#include <string>

class TiXmlElement;

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
    bool SerializeButtonMaps(TiXmlElement* pElement) const;

    bool Serialize(const FeatureVector& features, TiXmlElement* pElement) const;
    bool Deserialize(const TiXmlElement* pElement, FeatureVector& features, const std::string &controllerId) const;

    static bool IsValid(const kodi::addon::JoystickFeature& feature);
    static bool SerializeFeature(TiXmlElement* pElement, const kodi::addon::DriverPrimitive& primitive, const char* tagName);
    static bool SerializePrimitiveTag(TiXmlElement* pElement, const kodi::addon::DriverPrimitive& primitive, const char* tagName);
    static void SerializePrimitive(TiXmlElement* pElement, const kodi::addon::DriverPrimitive& primitive);
    static bool DeserializePrimitive(const TiXmlElement* pElement, kodi::addon::DriverPrimitive& primitive);
  };
}
