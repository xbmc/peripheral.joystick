/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "ButtonMapTypes.h"

#include <map>
#include <set>
#include <string>

namespace JOYSTICK
{
  // --- CJoystickFamily -------------------------------------------------------

  class CJoystickFamily
  {
  public:
    CJoystickFamily(const std::string& familyName);
    CJoystickFamily(const CJoystickFamily& other);

    bool operator<(const CJoystickFamily& other) const;

    const std::string& FamilyName() const { return m_familyName; }
    bool IsValid() const { return !m_familyName.empty(); }

  private:
    const std::string m_familyName;
  };

  // --- CJoystickFamilyManager ------------------------------------------------

  class CJoystickFamilyManager
  {
  public:
    CJoystickFamilyManager() = default;

    bool Initialize(const std::string& addonPath);
    void Deinitialize() { m_families.clear(); }

    const std::string& GetFamily(const std::string& name, const std::string& provider) const;

  private:
    bool LoadFamilies(const std::string& path);

    JoystickFamilyMap m_families;
  };
}
