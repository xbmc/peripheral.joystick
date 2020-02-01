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
