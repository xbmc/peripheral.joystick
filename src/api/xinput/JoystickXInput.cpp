/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickXInput.h"
#include "JoystickInterfaceXInput.h"
#include "XInputDLL.h"
#include "api/JoystickTypes.h"

#include <Xinput.h>

using namespace JOYSTICK;

#define XINPUT_ALIAS  "Xbox 360-compatible controller"
#define BUTTON_COUNT  15
#define HAT_COUNT     0 // hats are treated as buttons
#define AXIS_COUNT    6
#define MAX_AXIS      32768
#define MAX_TRIGGER   255
#define MAX_MOTOR     65535

CJoystickXInput::CJoystickXInput(unsigned int controllerID)
 : CJoystick(EJoystickInterface::XINPUT),
   m_controllerID(controllerID),
   m_dwPacketNumber(0)
{
  SetName(XINPUT_ALIAS);
  SetRequestedPort(m_controllerID);
  SetButtonCount(BUTTON_COUNT);
  SetHatCount(HAT_COUNT);
  SetAxisCount(AXIS_COUNT);
  SetMotorCount(MOTOR_COUNT);

  m_motorSpeeds[MOTOR_LEFT] = 0.0f;
  m_motorSpeeds[MOTOR_RIGHT] = 0.0f;

  SetSupportsPowerOff(true);
}

bool CJoystickXInput::Equals(const CJoystick* rhs) const
{
  if (rhs == nullptr)
    return false;

  const CJoystickXInput* rhsXInput = dynamic_cast<const CJoystickXInput*>(rhs);
  if (rhsXInput == nullptr)
    return false;

  return m_controllerID == rhsXInput->m_controllerID;
}

void CJoystickXInput::PowerOff()
{
  if (CXInputDLL::Get().Version() == "1.3")
    CXInputDLL::Get().PowerOff(m_controllerID);
}

bool CJoystickXInput::ScanEvents(void)
{
  if (CXInputDLL::Get().Version() == "1.3")
  {
    XINPUT_STATE_EX controllerState;

    if (!CXInputDLL::Get().GetStateWithGuide(m_controllerID, controllerState))
      return false;

    m_dwPacketNumber = controllerState.dwPacketNumber;

    SetButtonValue(0,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A)              ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(1,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B)              ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(2,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X)              ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(3,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)              ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(4,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)  ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(5,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(6,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)           ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(7,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START)          ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(8,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)     ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(9,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)    ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(10, (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)        ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(11, (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)     ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(12, (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)      ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(13, (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)      ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(14, (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_GUIDE)          ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);

    SetAxisValue(0, (long)controllerState.Gamepad.sThumbLX, MAX_AXIS);
    SetAxisValue(1, (long)controllerState.Gamepad.sThumbLY, MAX_AXIS);
    SetAxisValue(2, (long)controllerState.Gamepad.sThumbRX, MAX_AXIS);
    SetAxisValue(3, (long)controllerState.Gamepad.sThumbRY, MAX_AXIS);
    SetAxisValue(4, (long)controllerState.Gamepad.bLeftTrigger, MAX_TRIGGER);
    SetAxisValue(5, (long)controllerState.Gamepad.bRightTrigger, MAX_TRIGGER);
  }
  else
  {
    XINPUT_STATE controllerState;

    if (!CXInputDLL::Get().GetState(m_controllerID, controllerState))
      return false;

    m_dwPacketNumber = controllerState.dwPacketNumber;

    SetButtonValue(0,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A)              ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(1,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B)              ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(2,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X)              ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(3,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)              ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(4,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)  ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(5,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(6,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)           ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(7,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START)          ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(8,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)     ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(9,  (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)    ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(10, (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)        ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(11, (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)     ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(12, (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)      ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
    SetButtonValue(13, (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)      ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);

    SetAxisValue(0, (long)controllerState.Gamepad.sThumbLX, MAX_AXIS);
    SetAxisValue(1, (long)controllerState.Gamepad.sThumbLY, MAX_AXIS);
    SetAxisValue(2, (long)controllerState.Gamepad.sThumbRX, MAX_AXIS);
    SetAxisValue(3, (long)controllerState.Gamepad.sThumbRY, MAX_AXIS);
    SetAxisValue(4, (long)controllerState.Gamepad.bLeftTrigger, MAX_TRIGGER);
    SetAxisValue(5, (long)controllerState.Gamepad.bRightTrigger, MAX_TRIGGER);
  }

  return true;
}

bool CJoystickXInput::SetMotor(unsigned int motorIndex, float magnitude)
{
  bool bSuccess = false;

  if (motorIndex < MOTOR_COUNT && 0.0f <= magnitude && magnitude <= 1.0f)
  {
    m_motorSpeeds[motorIndex] = magnitude;

    XINPUT_VIBRATION vibrationState;

    vibrationState.wLeftMotorSpeed = static_cast<WORD>(m_motorSpeeds[MOTOR_LEFT] * MAX_MOTOR);
    vibrationState.wRightMotorSpeed = static_cast<WORD>(m_motorSpeeds[MOTOR_RIGHT] * MAX_MOTOR);

    // TODO: Only dispatch after both left and right events have been received
    bSuccess = CXInputDLL::Get().SetState(m_controllerID, vibrationState);
  }

  return bSuccess;
}
