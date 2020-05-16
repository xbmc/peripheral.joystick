/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
