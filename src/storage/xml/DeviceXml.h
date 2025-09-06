/*
 *  Copyright (C) 2015-2021 Garrett Brown
 *  Copyright (C) 2015-2021 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "storage/StorageTypes.h"

#include <string>

namespace tinyxml2
{
class XMLElement;
}

namespace JOYSTICK
{
  class CDevice;
  class CDeviceConfiguration;

  struct AxisConfiguration;
  struct ButtonConfiguration;

  class CDeviceXml
  {
  public:
    static bool Serialize(const CDevice& record, tinyxml2::XMLElement* pElement);
    static bool Deserialize(const tinyxml2::XMLElement* pElement, CDevice& record);

    static bool SerializeConfig(const CDeviceConfiguration& config, tinyxml2::XMLElement* pElement);
    static bool DeserializeConfig(const tinyxml2::XMLElement* pElement, CDeviceConfiguration& config);

    static bool SerializeAppearance(const std::string& controllerId, tinyxml2::XMLElement* pElement);
    static bool DeserializeAppearance(const tinyxml2::XMLElement* pElement, std::string& controllerId);

    static bool SerializeAxis(unsigned int index, const AxisConfiguration& axisConfig, tinyxml2::XMLElement* pElement);
    static bool DeserializeAxis(const tinyxml2::XMLElement* pElement, unsigned int& index, AxisConfiguration& axisConfig);

    static bool SerializeButton(unsigned int index, const ButtonConfiguration& buttonConfig, tinyxml2::XMLElement* pElement);
    static bool DeserializeButton(const tinyxml2::XMLElement* pElement, unsigned int& index, ButtonConfiguration& buttonConfig);
  };
}
