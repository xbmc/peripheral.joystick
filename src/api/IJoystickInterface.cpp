/*
 *      Copyright (C) 2017-2020 Garrett Brown
 *      Copyright (C) 2017-2020 Team XBMC
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

#include "IJoystickInterface.h"
#include "JoystickTranslator.h"

using namespace JOYSTICK;

std::string IJoystickInterface::Provider(void) const
{
  return JoystickTranslator::GetInterfaceProvider(Type());
}
