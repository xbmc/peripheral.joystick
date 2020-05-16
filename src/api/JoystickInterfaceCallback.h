/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "IJoystickInterface.h"

#include <vector>

namespace JOYSTICK
{
  class CJoystickInterfaceCallback : public IJoystickInterface
  {
  public:
    CJoystickInterfaceCallback(void) { }
    virtual ~CJoystickInterfaceCallback(void) { }

  protected:
    // Helper functions to offer a buffer for device scanners that require static callbacks
    void AddScanResult(const JoystickPtr& joystick);
    void GetScanResults(JoystickVector& joysticks);

  private:
    JoystickVector m_scanResults;
  };
}
