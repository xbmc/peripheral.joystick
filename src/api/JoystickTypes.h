/*
 *      Copyright (C) 2014-2020 Garrett Brown
 *      Copyright (C) 2014-2020 Team Kodi
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

#include <memory>
#include <vector>

namespace JOYSTICK
{
  /*!
   * \brief Joystick interface types
   *
   * Priority of interfaces is determined by JoystickUtils::GetDrivers().
   */
  enum class EJoystickInterface
  {
    NONE,
    COCOA,
    DIRECTINPUT,
    LINUX,
    SDL,
    UDEV,
    XINPUT,
  };

  class CJoystick;
  typedef std::shared_ptr<CJoystick> JoystickPtr;
  typedef std::vector<JoystickPtr>   JoystickVector;
}
