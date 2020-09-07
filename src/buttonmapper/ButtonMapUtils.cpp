/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ButtonMapUtils.h"

#include <kodi/addon-instance/peripheral/PeripheralUtils.h>

#include <array>
#include <map>

#include <array>
#include <map>

using namespace JOYSTICK;

bool ButtonMapUtils::PrimitivesEqual(const kodi::addon::JoystickFeature& lhs, const kodi::addon::JoystickFeature& rhs)
{
  bool bEqual = false;

  if (lhs.Type() == rhs.Type())
  {
    bEqual = true;

    std::vector<JOYSTICK_FEATURE_PRIMITIVE> primitives = GetPrimitives(lhs.Type());

    for (auto primitive : primitives)
    {
      if (!(lhs.Primitive(primitive) == rhs.Primitive(primitive)))
      {
        bEqual = false;
        break;
      }
    }
  }

  return bEqual;
}

bool ButtonMapUtils::PrimitivesConflict(const kodi::addon::DriverPrimitive& lhs, const kodi::addon::DriverPrimitive& rhs)
{
  if (lhs.Type() != JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN &&
      lhs.Type() == rhs.Type())
  {
    switch (lhs.Type())
    {
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON:
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR:
    {
      if (lhs.DriverIndex() == rhs.DriverIndex())
        return true;
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION:
    {
      if (lhs.DriverIndex() == rhs.DriverIndex() &&
          lhs.HatDirection() == rhs.HatDirection())
        return true;
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS:
    {
      if (lhs.DriverIndex() == rhs.DriverIndex())
      {
        std::array<float, 2> points = { { -0.5f, 0.5f } };
        for (auto point : points)
        {
          if (SemiAxisIntersects(lhs, point) && SemiAxisIntersects(rhs, point))
            return true;
        }
      }
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_KEY:
    {
      if (lhs.Keycode() == rhs.Keycode())
        return true;
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOUSE_BUTTON:
    {
      if (lhs.MouseIndex() == rhs.MouseIndex())
        return true;
      break;
    }
    case JOYSTICK_DRIVER_PRIMITIVE_TYPE_RELPOINTER_DIRECTION:
    {
      if (lhs.RelPointerDirection() == rhs.RelPointerDirection())
        return true;
      break;
    }
    default:
      return true;
    }
  }

  return false;
}

bool ButtonMapUtils::SemiAxisIntersects(const kodi::addon::DriverPrimitive& semiaxis, float point)
{
  if (semiaxis.Type() == JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS)
  {
    int endpoint1 = semiaxis.Center();
    int endpoint2 = semiaxis.Center() + semiaxis.Range() * semiaxis.SemiAxisDirection();

    if (endpoint1 <= endpoint2)
      return endpoint1 <= point && point <= endpoint2;
    else
      return endpoint2 <= point && point <= endpoint1;
  }
  return false;
}

const std::vector<JOYSTICK_FEATURE_PRIMITIVE>& ButtonMapUtils::GetPrimitives(JOYSTICK_FEATURE_TYPE featureType)
{
  static const std::map<JOYSTICK_FEATURE_TYPE, std::vector<JOYSTICK_FEATURE_PRIMITIVE>> m_primitiveMap = {
    {
      JOYSTICK_FEATURE_TYPE_SCALAR, {
        JOYSTICK_SCALAR_PRIMITIVE,
      }
    },
    {
      JOYSTICK_FEATURE_TYPE_ANALOG_STICK, {
        JOYSTICK_ANALOG_STICK_UP,
        JOYSTICK_ANALOG_STICK_DOWN,
        JOYSTICK_ANALOG_STICK_RIGHT,
        JOYSTICK_ANALOG_STICK_LEFT,
      }
    },
    {
      JOYSTICK_FEATURE_TYPE_ACCELEROMETER, {
        JOYSTICK_ACCELEROMETER_POSITIVE_X,
        JOYSTICK_ACCELEROMETER_POSITIVE_Y,
        JOYSTICK_ACCELEROMETER_POSITIVE_Z,
      }
    },
    {
      JOYSTICK_FEATURE_TYPE_MOTOR, {
        JOYSTICK_MOTOR_PRIMITIVE,
      }
    },
    {
      JOYSTICK_FEATURE_TYPE_RELPOINTER, {
        JOYSTICK_RELPOINTER_UP,
        JOYSTICK_RELPOINTER_DOWN,
        JOYSTICK_RELPOINTER_RIGHT,
        JOYSTICK_RELPOINTER_LEFT,
      }
    },
    {
      JOYSTICK_FEATURE_TYPE_WHEEL, {
        JOYSTICK_WHEEL_RIGHT,
        JOYSTICK_WHEEL_LEFT,
      }
    },
    {
      JOYSTICK_FEATURE_TYPE_THROTTLE, {
        JOYSTICK_THROTTLE_UP,
        JOYSTICK_THROTTLE_DOWN,
      }
    },
    {
      JOYSTICK_FEATURE_TYPE_KEY, {
        JOYSTICK_KEY_PRIMITIVE,
      }
    },
  };

  auto itPair = m_primitiveMap.find(featureType);
  if (itPair != m_primitiveMap.end())
    return itPair->second;

  static const std::vector<JOYSTICK_FEATURE_PRIMITIVE> empty;
  return empty;
}
