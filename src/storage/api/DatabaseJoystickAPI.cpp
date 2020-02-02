/*
 *      Copyright (C) 2015-2020 Garrett Brown
 *      Copyright (C) 2015-2020 Team Kodi
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

#include "DatabaseJoystickAPI.h"
#include "api/JoystickManager.h"
#include "storage/Device.h"

using namespace JOYSTICK;

const ButtonMap& CDatabaseJoystickAPI::GetButtonMap(const kodi::addon::Joystick& driverInfo)
{
  return CJoystickManager::Get().GetButtonMap(driverInfo.Provider());
}

bool CDatabaseJoystickAPI::MapFeatures(const kodi::addon::Joystick& driverInfo, const std::string& controllerId, const FeatureVector& features)
{
  return false;
}

bool CDatabaseJoystickAPI::GetIgnoredPrimitives(const kodi::addon::Joystick& joystick, PrimitiveVector& primitives)
{
  return false;
}

bool CDatabaseJoystickAPI::SetIgnoredPrimitives(const kodi::addon::Joystick& joystick, const PrimitiveVector& primitives)
{
  return false;
}

bool CDatabaseJoystickAPI::SaveButtonMap(const kodi::addon::Joystick& driverInfo)
{
  return false;
}

bool CDatabaseJoystickAPI::RevertButtonMap(const kodi::addon::Joystick& driverInfo)
{
  return false;
}

bool CDatabaseJoystickAPI::ResetButtonMap(const kodi::addon::Joystick& driverInfo, const std::string& controllerId)
{
  return false;
}
