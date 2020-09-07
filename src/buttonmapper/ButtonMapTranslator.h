/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/peripheral/PeripheralUtils.h>

#include <string>

namespace JOYSTICK
{
  class ButtonMapTranslator
  {
  public:
    /*!
     * \brief Canonical string serialization of the driver primitive
     */
    static std::string ToString(const kodi::addon::DriverPrimitive& primitive);

    /*!
     * \brief Deserialize string representation of driver primitive
     */
    static kodi::addon::DriverPrimitive ToDriverPrimitive(const std::string& primitive, JOYSTICK_DRIVER_PRIMITIVE_TYPE type);
  };
}
