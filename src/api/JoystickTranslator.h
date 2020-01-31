/*
 *      Copyright (C) 2015-2020 Garrett Brown
 *      Copyright (C) 2015-2020 Team Kodi
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
