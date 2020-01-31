/*
 *      Copyright (C) 2017-2020 Garrett Brown
 *      Copyright (C) 2017-2020 Team Kodi
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
 */

#include "JoystickUtils.h"
#include "Joystick.h"
#include "JoystickTranslator.h"
#include "JoystickTypes.h"

using namespace JOYSTICK;

bool CJoystickUtils::IsGhostJoystick(const CJoystick& joystick)
{
  // Ghost joysticks observed on linux and udev
  if (joystick.Provider() == JoystickTranslator::GetInterfaceProvider(EJoystickInterface::LINUX) ||
      joystick.Provider() == JoystickTranslator::GetInterfaceProvider(EJoystickInterface::UDEV))
  {
    // Wireless receiver names
    if (joystick.Name() == "Xbox 360 Wireless Receiver" ||
        joystick.Name() == "Xbox 360 Wireless Receiver (XBOX)")
    {
      return true;
    }
  }

  return false;
}
