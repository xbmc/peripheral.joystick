/*
 *  Copyright (C) 2018-2020 Garrett Brown
 *  Copyright (C) 2018-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "MouseTranslator.h"

using namespace JOYSTICK;

#define MOUSE_BUTTON_NAME_LEFT               "left"
#define MOUSE_BUTTON_NAME_RIGHT              "right"
#define MOUSE_BUTTON_NAME_MIDDLE             "middle"
#define MOUSE_BUTTON_NAME_BUTTON4            "button4"
#define MOUSE_BUTTON_NAME_BUTTON5            "button5"
#define MOUSE_BUTTON_NAME_WHEEL_UP           "wheelup"
#define MOUSE_BUTTON_NAME_WHEEL_DOWN         "wheeldown"
#define MOUSE_BUTTON_NAME_HORIZ_WHEEL_LEFT   "horizwheelleft"
#define MOUSE_BUTTON_NAME_HORIZ_WHEEL_RIGHT  "horizwheelright"

std::string CMouseTranslator::SerializeMouseButton(JOYSTICK_DRIVER_MOUSE_INDEX buttonIndex)
{
  switch (buttonIndex)
  {
  case JOYSTICK_DRIVER_MOUSE_INDEX_LEFT:              return MOUSE_BUTTON_NAME_LEFT;
  case JOYSTICK_DRIVER_MOUSE_INDEX_RIGHT:             return MOUSE_BUTTON_NAME_RIGHT;
  case JOYSTICK_DRIVER_MOUSE_INDEX_MIDDLE:            return MOUSE_BUTTON_NAME_MIDDLE;
  case JOYSTICK_DRIVER_MOUSE_INDEX_BUTTON4:           return MOUSE_BUTTON_NAME_BUTTON4;
  case JOYSTICK_DRIVER_MOUSE_INDEX_BUTTON5:           return MOUSE_BUTTON_NAME_BUTTON5;
  case JOYSTICK_DRIVER_MOUSE_INDEX_WHEEL_UP:          return MOUSE_BUTTON_NAME_WHEEL_UP;
  case JOYSTICK_DRIVER_MOUSE_INDEX_WHEEL_DOWN:        return MOUSE_BUTTON_NAME_WHEEL_DOWN;
  case JOYSTICK_DRIVER_MOUSE_INDEX_HORIZ_WHEEL_LEFT:  return MOUSE_BUTTON_NAME_HORIZ_WHEEL_LEFT;
  case JOYSTICK_DRIVER_MOUSE_INDEX_HORIZ_WHEEL_RIGHT: return MOUSE_BUTTON_NAME_HORIZ_WHEEL_RIGHT;
  default:
    break;
  }

  return "";
}

JOYSTICK_DRIVER_MOUSE_INDEX CMouseTranslator::DeserializeMouseButton(const std::string &buttonName)
{
  if (buttonName == MOUSE_BUTTON_NAME_LEFT)              return JOYSTICK_DRIVER_MOUSE_INDEX_LEFT;
  if (buttonName == MOUSE_BUTTON_NAME_RIGHT)             return JOYSTICK_DRIVER_MOUSE_INDEX_RIGHT;
  if (buttonName == MOUSE_BUTTON_NAME_MIDDLE)            return JOYSTICK_DRIVER_MOUSE_INDEX_MIDDLE;
  if (buttonName == MOUSE_BUTTON_NAME_BUTTON4)           return JOYSTICK_DRIVER_MOUSE_INDEX_BUTTON4;
  if (buttonName == MOUSE_BUTTON_NAME_BUTTON5)           return JOYSTICK_DRIVER_MOUSE_INDEX_BUTTON5;
  if (buttonName == MOUSE_BUTTON_NAME_WHEEL_UP)          return JOYSTICK_DRIVER_MOUSE_INDEX_WHEEL_UP;
  if (buttonName == MOUSE_BUTTON_NAME_WHEEL_DOWN)        return JOYSTICK_DRIVER_MOUSE_INDEX_WHEEL_DOWN;
  if (buttonName == MOUSE_BUTTON_NAME_HORIZ_WHEEL_LEFT)  return JOYSTICK_DRIVER_MOUSE_INDEX_HORIZ_WHEEL_LEFT;
  if (buttonName == MOUSE_BUTTON_NAME_HORIZ_WHEEL_RIGHT) return JOYSTICK_DRIVER_MOUSE_INDEX_HORIZ_WHEEL_RIGHT;

  return JOYSTICK_DRIVER_MOUSE_INDEX_UNKNOWN;
}
