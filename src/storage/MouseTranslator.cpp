/*
 *      Copyright (C) 2018-2020 Garrett Brown
 *      Copyright (C) 2018-2020 Team Kodi
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
