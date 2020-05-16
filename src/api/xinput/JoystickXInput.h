/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
