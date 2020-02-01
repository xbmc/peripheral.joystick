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
#pragma once

#include "storage/IDatabase.h"

namespace JOYSTICK
{
  class CDatabaseJoystickAPI : public IDatabase
  {
  public:
    CDatabaseJoystickAPI(IDatabaseCallbacks* callbacks) : IDatabase(callbacks) { }

    virtual ~CDatabaseJoystickAPI(void) { }

    // implementation of IDatabase
    virtual const ButtonMap& GetButtonMap(const kodi::addon::Joystick& driverInfo) override;
    virtual bool MapFeatures(const kodi::addon::Joystick& driverInfo, const std::string& controllerId, const FeatureVector& features) override;
    virtual bool GetIgnoredPrimitives(const kodi::addon::Joystick& driverInfo, PrimitiveVector& primitives) override;
    virtual bool SetIgnoredPrimitives(const kodi::addon::Joystick& driverInfo, const PrimitiveVector& primitives) override;
    virtual bool SaveButtonMap(const kodi::addon::Joystick& driverInfo) override;
    virtual bool RevertButtonMap(const kodi::addon::Joystick& driverInfo) override;
    virtual bool ResetButtonMap(const kodi::addon::Joystick& driverInfo, const std::string& controllerId) override;
  };
}
