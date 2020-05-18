/*
 *  Copyright (C) 2017-2020 Garrett Brown
 *  Copyright (C) 2017-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "IJoystickInterface.h"
#include "JoystickTranslator.h"

using namespace JOYSTICK;

std::string IJoystickInterface::Provider(void) const
{
  return JoystickTranslator::GetInterfaceProvider(Type());
}
