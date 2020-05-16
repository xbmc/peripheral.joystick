/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "api/IJoystickInterface.h"

#include <stdint.h>
#include <string>

namespace JOYSTICK
{
  class CJoystickInterfaceLinux : public IJoystickInterface
  {
  public:
    CJoystickInterfaceLinux(void) { }
    virtual ~CJoystickInterfaceLinux(void) { }

    // implementation of IJoystickInterface
    virtual EJoystickInterface Type(void) const override;
    virtual bool ScanForJoysticks(JoystickVector& joysticks) override;
  };
}
