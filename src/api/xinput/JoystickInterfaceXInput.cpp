/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickInterfaceXInput.h"
#include "JoystickXInput.h"
#include "XInputDLL.h"
#include "api/JoystickTypes.h"

#include <array>
#include <cstring>
#include <utility>
#include <Xinput.h>

using namespace JOYSTICK;

#define MAX_JOYSTICKS 4

ButtonMap CJoystickInterfaceXInput::m_buttonMap = {
    std::make_pair("game.controller.default", FeatureVector{
        kodi::addon::JoystickFeature("leftmotor", JOYSTICK_FEATURE_TYPE_MOTOR),
        kodi::addon::JoystickFeature("rightmotor", JOYSTICK_FEATURE_TYPE_MOTOR),
    }),
    std::make_pair("game.controller.ps", FeatureVector{
        kodi::addon::JoystickFeature("strongmotor", JOYSTICK_FEATURE_TYPE_MOTOR),
        kodi::addon::JoystickFeature("weakmotor", JOYSTICK_FEATURE_TYPE_MOTOR),
    }),
};

EJoystickInterface CJoystickInterfaceXInput::Type(void) const
{
  return EJoystickInterface::XINPUT;
}

bool CJoystickInterfaceXInput::Initialize(void)
{
  return CXInputDLL::Get().Load();
}

void CJoystickInterfaceXInput::Deinitialize(void)
{
  CXInputDLL::Get().Unload();
}

bool CJoystickInterfaceXInput::SupportsPowerOff(void) const
{
  return CXInputDLL::Get().SupportsPowerOff();
}

bool CJoystickInterfaceXInput::ScanForJoysticks(JoystickVector& joysticks)
{
  // No need to memset, only checking for controller existence
  XINPUT_STATE controllerState;
  XINPUT_STATE_EX controllerStateWithGuide;

  for (unsigned int i = 0; i < MAX_JOYSTICKS; i++)
  {
    if (CXInputDLL::Get().HasGuideButton())
    {
      if (!CXInputDLL::Get().GetStateWithGuide(i, controllerStateWithGuide))
        continue;
    }
    else
    {
      if (!CXInputDLL::Get().GetState(i, controllerState))
        continue;
    }

    joysticks.push_back(JoystickPtr(new CJoystickXInput(i)));
  }

  return true;
}

const ButtonMap& CJoystickInterfaceXInput::GetButtonMap()
{
  auto& dflt = m_buttonMap["game.controller.default"];
  dflt[CJoystickXInput::MOTOR_LEFT].SetPrimitive(JOYSTICK_MOTOR_PRIMITIVE, kodi::addon::DriverPrimitive::CreateMotor(CJoystickXInput::MOTOR_LEFT));
  dflt[CJoystickXInput::MOTOR_RIGHT].SetPrimitive(JOYSTICK_MOTOR_PRIMITIVE, kodi::addon::DriverPrimitive::CreateMotor(CJoystickXInput::MOTOR_RIGHT));

  auto& ps = m_buttonMap["game.controller.ps"];
  ps[CJoystickXInput::MOTOR_LEFT].SetPrimitive(JOYSTICK_MOTOR_PRIMITIVE, kodi::addon::DriverPrimitive::CreateMotor(CJoystickXInput::MOTOR_LEFT));
  ps[CJoystickXInput::MOTOR_RIGHT].SetPrimitive(JOYSTICK_MOTOR_PRIMITIVE, kodi::addon::DriverPrimitive::CreateMotor(CJoystickXInput::MOTOR_RIGHT));

  return m_buttonMap;
}
