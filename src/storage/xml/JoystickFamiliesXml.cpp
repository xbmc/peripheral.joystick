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

#include "JoystickFamiliesXml.h"
#include "JoystickFamilyDefinitions.h"
#include "log/Log.h"

#include <tinyxml.h>

using namespace JOYSTICK;

bool CJoystickFamiliesXml::LoadFamilies(const std::string& path, JoystickFamilyMap& result)
{
  TiXmlDocument xmlFile;
  if (!xmlFile.LoadFile(path))
  {
    esyslog("Error opening %s: %s", path.c_str(), xmlFile.ErrorDesc());
    return false;
  }

  TiXmlElement* pRootElement = xmlFile.RootElement();
  if (!pRootElement || pRootElement->NoChildren() || pRootElement->ValueStr() != JOYSTICK_FAMILIES_XML_ELEM_FAMILIES)
  {
    esyslog("Can't find root <%s> tag", JOYSTICK_FAMILIES_XML_ELEM_FAMILIES);
    return false;
  }

  const TiXmlElement* pFamily = pRootElement->FirstChildElement(JOYSTICK_FAMILIES_XML_ELEM_FAMILY);

  if (pFamily == nullptr)
  {
    esyslog("Can't find <%s> tag", JOYSTICK_FAMILIES_XML_ELEM_FAMILY);
    return false;
  }

  return Deserialize(pFamily, result);
}

bool CJoystickFamiliesXml::Deserialize(const TiXmlElement* pFamily, JoystickFamilyMap& result)
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

    const TiXmlElement* pJoystick = pFamily->FirstChildElement(JOYSTICK_FAMILIES_XML_ELEM_JOYSTICK);

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

bool CJoystickFamiliesXml::DeserializeJoysticks(const TiXmlElement* pJoystick, std::set<std::string>& family)
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
