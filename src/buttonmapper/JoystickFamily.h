/*
 *      Copyright (C) 2016-2020 Garrett Brown
 *      Copyright (C) 2016-2020 Team Kodi
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
