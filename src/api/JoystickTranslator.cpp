/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickTranslator.h"

#include <algorithm>

using namespace JOYSTICK;

namespace JOYSTICK
{
  struct SJoystickInterface
  {
    EJoystickInterface type;
    const char* provider;
  };

  static const std::vector<SJoystickInterface> Interfaces =
  {
    {
      EJoystickInterface::COCOA,
      "cocoa",
    },
    {
      EJoystickInterface::DIRECTINPUT,
      "directinput",
    },
    {
      EJoystickInterface::LINUX,
      "linux",
    },
    {
      EJoystickInterface::SDL,
      "sdl",
    },
    {
      EJoystickInterface::UDEV,
      "udev",
    },
    {
      EJoystickInterface::XINPUT,
      "xinput",
    },
  };

  struct FindByType
  {
    FindByType(EJoystickInterface type) : m_type(type) { }

    bool operator()(const SJoystickInterface& iface)
    {
      return iface.type == m_type;
    }

  private:
    EJoystickInterface m_type;
  };

  struct FindByProvider
  {
    FindByProvider(const std::string& provider) : m_provider(provider) { }

    bool operator()(const SJoystickInterface& iface)
    {
      return iface.provider == m_provider;
    }

  private:
    const std::string m_provider;
  };
}

// --- JoystickTranslator ------------------------------------------------------

std::string JoystickTranslator::GetInterfaceProvider(EJoystickInterface iface)
{
  std::string provider;

  auto it = std::find_if(Interfaces.begin(), Interfaces.end(), FindByType(iface));
  if (it != Interfaces.end())
    provider = it->provider;

  return provider;
}

EJoystickInterface JoystickTranslator::GetInterfaceType(const std::string& provider)
{
  EJoystickInterface type = EJoystickInterface::NONE;

  auto it = std::find_if(Interfaces.begin(), Interfaces.end(), FindByProvider(provider));
  if (it != Interfaces.end())
    type = it->type;

  return type;
}

JOYSTICK_DRIVER_HAT_DIRECTION JoystickTranslator::TranslateHatDir(const std::string& hatDir)
{
  if (hatDir == "up")    return JOYSTICK_DRIVER_HAT_UP;
  if (hatDir == "down")  return JOYSTICK_DRIVER_HAT_DOWN;
  if (hatDir == "right") return JOYSTICK_DRIVER_HAT_RIGHT;
  if (hatDir == "left")  return JOYSTICK_DRIVER_HAT_LEFT;

  return JOYSTICK_DRIVER_HAT_UNKNOWN;
}

const char* JoystickTranslator::TranslateHatDir(JOYSTICK_DRIVER_HAT_DIRECTION hatDir)
{
  switch (hatDir)
  {
    case JOYSTICK_DRIVER_HAT_UP:    return "up";
    case JOYSTICK_DRIVER_HAT_DOWN:  return "down";
    case JOYSTICK_DRIVER_HAT_RIGHT: return "right";
    case JOYSTICK_DRIVER_HAT_LEFT:  return "left";
    default:
      break;
  }
  return "";
}

JOYSTICK_DRIVER_SEMIAXIS_DIRECTION JoystickTranslator::TranslateSemiAxisDir(char axisSign)
{
  switch (axisSign)
  {
  case '+': return JOYSTICK_DRIVER_SEMIAXIS_POSITIVE;
  case '-': return JOYSTICK_DRIVER_SEMIAXIS_NEGATIVE;
  default:
    break;
  }
  return JOYSTICK_DRIVER_SEMIAXIS_UNKNOWN;
}

const char* JoystickTranslator::TranslateSemiAxisDir(JOYSTICK_DRIVER_SEMIAXIS_DIRECTION dir)
{
  switch (dir)
  {
    case JOYSTICK_DRIVER_SEMIAXIS_POSITIVE: return "+";
    case JOYSTICK_DRIVER_SEMIAXIS_NEGATIVE: return "-";
    default:
      break;
  }
  return "";
}


JOYSTICK_DRIVER_RELPOINTER_DIRECTION JoystickTranslator::TranslateRelPointerDir(const std::string relPointerDir)
{
  if (relPointerDir == "+x") return JOYSTICK_DRIVER_RELPOINTER_RIGHT;
  if (relPointerDir == "-x") return JOYSTICK_DRIVER_RELPOINTER_LEFT;
  if (relPointerDir == "-y") return JOYSTICK_DRIVER_RELPOINTER_UP;
  if (relPointerDir == "+y") return JOYSTICK_DRIVER_RELPOINTER_DOWN;

  return JOYSTICK_DRIVER_RELPOINTER_UNKNOWN;
}

const char* JoystickTranslator::TranslateRelPointerDir(JOYSTICK_DRIVER_RELPOINTER_DIRECTION dir)
{
  switch (dir)
  {
  case JOYSTICK_DRIVER_RELPOINTER_RIGHT: return "+x";
  case JOYSTICK_DRIVER_RELPOINTER_LEFT:  return "-x";
  case JOYSTICK_DRIVER_RELPOINTER_UP:    return "-y";
  case JOYSTICK_DRIVER_RELPOINTER_DOWN:  return "+y";
  default:
    break;
  }

  return "";
}
