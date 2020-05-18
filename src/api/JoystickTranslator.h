/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "JoystickTypes.h"

#include <kodi/addon-instance/Peripheral.h>

#include <string>

namespace JOYSTICK
{
  class JoystickTranslator
  {
  public:
    static std::string GetInterfaceProvider(EJoystickInterface iface);
    static EJoystickInterface GetInterfaceType(const std::string& provider);

    static JOYSTICK_DRIVER_HAT_DIRECTION TranslateHatDir(const std::string& hatDir);
    static const char* TranslateHatDir(JOYSTICK_DRIVER_HAT_DIRECTION hatDir);

    static JOYSTICK_DRIVER_SEMIAXIS_DIRECTION TranslateSemiAxisDir(char axisSign);
    static const char* TranslateSemiAxisDir(JOYSTICK_DRIVER_SEMIAXIS_DIRECTION dir);

    static JOYSTICK_DRIVER_RELPOINTER_DIRECTION TranslateRelPointerDir(const std::string relPointerDir);
    static const char* TranslateRelPointerDir(JOYSTICK_DRIVER_RELPOINTER_DIRECTION dir);
  };
}
