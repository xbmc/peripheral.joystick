/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/Peripheral.h>

namespace kodi
{
namespace addon
{
  struct DriverPrimitive;
  class JoystickFeature;
}
}

namespace JOYSTICK
{
  class ButtonMapUtils
  {
  public:
    /*!
     * \brief Check if two features having matching primitives
     */
    static bool PrimitivesEqual(const kodi::addon::JoystickFeature& lhs, const kodi::addon::JoystickFeature& rhs);

    /*!
     * \brief Check if two primitives conflict with each other
     */
    static bool PrimitivesConflict(const kodi::addon::DriverPrimitive& lhs, const kodi::addon::DriverPrimitive& rhs);

    /*!
     * \brief Check if a point intersects the range covered by a semiaxis,
     *        including its endpoints
     */
    static bool SemiAxisIntersects(const kodi::addon::DriverPrimitive& semiaxis, float point);

    /*!
     * \brief Get a list of all primitives belonging to this feature
     */
    static const std::vector<JOYSTICK_FEATURE_PRIMITIVE>& GetPrimitives(JOYSTICK_FEATURE_TYPE featureTypes);
  };
}
