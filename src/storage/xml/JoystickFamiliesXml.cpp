/*
 *  Copyright (C) 2016-2021 Garrett Brown
 *  Copyright (C) 2016-2021 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickFamiliesXml.h"

#include "JoystickFamilyDefinitions.h"
#include "log/Log.h"

#include <tinyxml2.h>

using namespace JOYSTICK;

bool CJoystickFamiliesXml::LoadFamilies(const std::string& path, JoystickFamilyMap& result)
{
  tinyxml2::XMLDocument xmlFile;
  xmlFile.LoadFile(path.c_str());
  if (xmlFile.Error())
  {
    esyslog("Error opening %s: %s", path.c_str(), xmlFile.ErrorName());
    return false;
  }

  tinyxml2::XMLElement* pRootElement = xmlFile.RootElement();
  if (!pRootElement || pRootElement->NoChildren() || std::string(pRootElement->Value()) != JOYSTICK_FAMILIES_XML_ELEM_FAMILIES)
  {
    esyslog("Can't find root <%s> tag", JOYSTICK_FAMILIES_XML_ELEM_FAMILIES);
    return false;
  }

  const tinyxml2::XMLElement* pFamily = pRootElement->FirstChildElement(JOYSTICK_FAMILIES_XML_ELEM_FAMILY);

  if (pFamily == nullptr)
  {
    esyslog("Can't find <%s> tag", JOYSTICK_FAMILIES_XML_ELEM_FAMILY);
    return false;
  }

  return Deserialize(pFamily, result);
}

bool CJoystickFamiliesXml::Deserialize(const tinyxml2::XMLElement* pFamily, JoystickFamilyMap& result)
{
  // For logging purposes
  unsigned int totalJoystickCount = 0;

  while (pFamily != nullptr)
  {
    const char* familyName = pFamily->Attribute(JOYSTICK_FAMILIES_XML_ATTR_FAMILY_NAME);
    if (!familyName)
    {
      esyslog("<%s> tag has no attribute \"%s\"", JOYSTICK_FAMILIES_XML_ELEM_FAMILY,
          JOYSTICK_FAMILIES_XML_ATTR_FAMILY_NAME);
      return false;
    }

    std::set<std::string>& family = result[familyName];

    const tinyxml2::XMLElement* pJoystick = pFamily->FirstChildElement(JOYSTICK_FAMILIES_XML_ELEM_JOYSTICK);

    if (pJoystick == nullptr)
    {
      esyslog("Joystick family \"%s\": Can't find <%s> tag", familyName, JOYSTICK_FAMILIES_XML_ELEM_JOYSTICK);
      return false;
    }

    if (!DeserializeJoysticks(pJoystick, family))
      return false;

    totalJoystickCount += static_cast<unsigned int>(family.size());

    pFamily = pFamily->NextSiblingElement(JOYSTICK_FAMILIES_XML_ELEM_FAMILY);
  }

  dsyslog("Loaded %d joystick families with %d total joysticks", result.size(), totalJoystickCount);

  return true;
}

bool CJoystickFamiliesXml::DeserializeJoysticks(const tinyxml2::XMLElement* pJoystick, std::set<std::string>& family)
{
  while (pJoystick != nullptr)
  {
    const char* joystickName = pJoystick->GetText();
    if (joystickName)
      family.insert(joystickName);

    pJoystick = pJoystick->NextSiblingElement(JOYSTICK_FAMILIES_XML_ELEM_JOYSTICK);
  }

  return true;
}
