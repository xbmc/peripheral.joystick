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

#include "JoystickFamily.h"
#include "storage/xml/JoystickFamiliesXml.h"
#include "storage/xml/JoystickFamilyDefinitions.h"

using namespace JOYSTICK;

// --- CJoystickFamily ---------------------------------------------------------

CJoystickFamily::CJoystickFamily(const std::string& familyName) :
  m_familyName(familyName)
{
}

CJoystickFamily::CJoystickFamily(const CJoystickFamily& other) :
  m_familyName(other.m_familyName)
{
}

bool CJoystickFamily::operator<(const CJoystickFamily& other) const
{
  return m_familyName < other.m_familyName;
}

// --- CJoystickFamilyManager --------------------------------------------------

bool CJoystickFamilyManager::Initialize(const std::string& addonPath)
{
  std::string path = addonPath + "/" JOYSTICK_FAMILIES_FOLDER "/" JOYSTICK_FAMILIES_RESOURCE;
  return LoadFamilies(path);
}

bool CJoystickFamilyManager::LoadFamilies(const std::string& path)
{
  CJoystickFamiliesXml::LoadFamilies(path, m_families);

  return !m_families.empty();
}

const std::string& CJoystickFamilyManager::GetFamily(const std::string& name, const std::string& provider) const
{
  static std::string empty;

  for (auto it = m_families.begin(); it != m_families.end(); ++it)
  {
    const std::set<JoystickName>& joystickNames = it->second;
    if (joystickNames.find(name) != joystickNames.end())
      return it->first;
  }

  return empty;
}
