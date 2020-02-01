/*
 *      Copyright (C) 2014-2020 Garrett Brown
 *      Copyright (C) 2014-2020 Team Kodi
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
