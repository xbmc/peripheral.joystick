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
