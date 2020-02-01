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

#include "api/Joystick.h"

#include <windows.h>

namespace JOYSTICK
{
  class CJoystickXInput : public CJoystick
  {
  public:
    enum
    {
      MOTOR_LEFT = 0,
      MOTOR_RIGHT = 1,
      MOTOR_COUNT = 2,
    };

    CJoystickXInput(unsigned int controllerID);
    virtual ~CJoystickXInput(void) { }

    virtual bool Equals(const CJoystick* rhs) const override;

    virtual void PowerOff() override;

  protected:
    virtual bool ScanEvents(void) override;
    virtual bool SetMotor(unsigned int motorIndex, float magnitude) override;

  private:
    unsigned int m_controllerID;   // XInput port, in the range (0, 3)
    DWORD        m_dwPacketNumber; // If unchanged, controller state hasn't changed (currently ignored)
    float        m_motorSpeeds[MOTOR_COUNT];
  };
}
