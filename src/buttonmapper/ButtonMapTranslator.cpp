/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ButtonMapTranslator.h"
#include "api/JoystickTranslator.h"
#include "storage/MouseTranslator.h"

#include <cstdlib>
#include <cctype>
#include <sstream>

using namespace JOYSTICK;

#define HAT_CHAR  'h'
#define MOTOR_CHAR  'm'

std::string ButtonMapTranslator::ToString(const kodi::addon::DriverPrimitive& primitive)
{
  std::stringstream strPrimitive;
  switch (primitive.Type())
  {
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON:
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR:
    {
      strPrimitive << primitive.DriverIndex();
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION:
    {
      strPrimitive << HAT_CHAR;
      strPrimitive << primitive.DriverIndex();
      strPrimitive << JoystickTranslator::TranslateHatDir(primitive.HatDirection());
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS:
    {
      const char* dir = JoystickTranslator::TranslateSemiAxisDir(primitive.SemiAxisDirection());
      if (*dir != '\0')
      {
        strPrimitive << dir;
        strPrimitive << primitive.DriverIndex();
      }
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_KEY:
    {
      strPrimitive << primitive.Keycode();
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOUSE_BUTTON:
    {
      strPrimitive << CMouseTranslator::SerializeMouseButton(primitive.MouseIndex());
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_RELPOINTER_DIRECTION:
    {
      strPrimitive << JoystickTranslator::TranslateRelPointerDir(primitive.RelPointerDirection());
      break;
    }
    default:
      break;
  }
  return strPrimitive.str();
}

kodi::addon::DriverPrimitive ButtonMapTranslator::ToDriverPrimitive(const std::string& strPrimitive, JOYSTICK_DRIVER_PRIMITIVE_TYPE type)
{
  kodi::addon::DriverPrimitive primitive;

  if (!strPrimitive.empty())
  {
    switch (type)
    {
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON:
    {
      if (std::isdigit(strPrimitive[0]))
        primitive = kodi::addon::DriverPrimitive::CreateButton(std::atoi(strPrimitive.c_str()));
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION:
    {
      if (strPrimitive[0] == HAT_CHAR)
      {
        unsigned int hatIndex = std::atoi(strPrimitive.substr(1).c_str());
        size_t dirPos = strPrimitive.find_first_not_of("0123456789", 1);
        if (dirPos != std::string::npos)
        {
          JOYSTICK_DRIVER_HAT_DIRECTION hatDir = JoystickTranslator::TranslateHatDir(strPrimitive.substr(dirPos));
          if (hatDir != JOYSTICK_DRIVER_HAT_UNKNOWN)
            primitive = kodi::addon::DriverPrimitive(hatIndex, hatDir);
        }
      }
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_RELPOINTER_DIRECTION:
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS:
    {
      // First try to deserialize relative pointer axis
      JOYSTICK_DRIVER_RELPOINTER_DIRECTION dir = JoystickTranslator::TranslateRelPointerDir(strPrimitive);
      if (dir != JOYSTICK_DRIVER_RELPOINTER_UNKNOWN)
      {
        primitive = kodi::addon::DriverPrimitive(dir);
      }
      else
      {
        // Next try to deserialize semiaxis
        JOYSTICK_DRIVER_SEMIAXIS_DIRECTION dir = JoystickTranslator::TranslateSemiAxisDir(strPrimitive[0]);
        if (dir != JOYSTICK_DRIVER_SEMIAXIS_UNKNOWN)
          primitive = kodi::addon::DriverPrimitive(std::atoi(strPrimitive.substr(1).c_str()), 0, dir, 1);
      }
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR:
    {
      if (std::isdigit(strPrimitive[0]))
        primitive = kodi::addon::DriverPrimitive::CreateMotor(std::atoi(strPrimitive.c_str()));
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_KEY:
    {
      primitive = kodi::addon::DriverPrimitive(strPrimitive);
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOUSE_BUTTON:
    {
      primitive = kodi::addon::DriverPrimitive::CreateMouseButton(CMouseTranslator::DeserializeMouseButton(strPrimitive));
      break;
    }
    default:
      break;
    }
  }

  return primitive;
}
