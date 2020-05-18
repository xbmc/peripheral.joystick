/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
