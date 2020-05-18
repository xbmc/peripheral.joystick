/*
 *  Copyright (C) 2018-2020 Garrett Brown
 *  Copyright (C) 2018-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/Peripheral.h>

#include <string>

namespace JOYSTICK
{
  /*!
   * \brief Helper functions for translating mouse IDs
   */
  class CMouseTranslator
  {
  public:
    static std::string SerializeMouseButton(JOYSTICK_DRIVER_MOUSE_INDEX buttonIndex);
    static JOYSTICK_DRIVER_MOUSE_INDEX DeserializeMouseButton(const std::string &buttonName);
  };
}
