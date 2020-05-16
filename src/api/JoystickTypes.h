/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
