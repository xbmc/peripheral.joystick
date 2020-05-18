/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
