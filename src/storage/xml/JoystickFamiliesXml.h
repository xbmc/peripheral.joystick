/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "buttonmapper/ButtonMapTypes.h"

class TiXmlElement;

namespace JOYSTICK
{
  class CJoystickFamiliesXml
  {
  public:
    static bool LoadFamilies(const std::string& path, JoystickFamilyMap& result);

  private:
    static bool Deserialize(const TiXmlElement* pFamily, JoystickFamilyMap& result);
    static bool DeserializeJoysticks(const TiXmlElement* pJoystick, std::set<std::string>& family);
  };
}
