/*
 *  Copyright (C) 2016-2021 Garrett Brown
 *  Copyright (C) 2016-2021 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "buttonmapper/ButtonMapTypes.h"

namespace tinyxml2
{
class XMLElement;
}

namespace JOYSTICK
{
  class CJoystickFamiliesXml
  {
  public:
    static bool LoadFamilies(const std::string& path, JoystickFamilyMap& result);

  private:
    static bool Deserialize(const tinyxml2::XMLElement* pFamily, JoystickFamilyMap& result);
    static bool DeserializeJoysticks(const tinyxml2::XMLElement* pJoystick, std::set<std::string>& family);
  };
}
