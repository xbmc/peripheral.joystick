/*
 *  Copyright (C) 2016-2017 Sam Lantinga
 *  Copyright (C) 2014-2021 Garrett Brown
 *  Copyright (C) 2014-2021 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickSDL.h"
#include "api/JoystickTypes.h"
#include "log/Log.h"

#include <SDL2/SDL.h>

using namespace JOYSTICK;

#define MAX_AXIS      32768

CJoystickSDL::CJoystickSDL(unsigned int index) :
  CJoystick(EJoystickInterface::SDL),
  m_index(index),
  m_pController(nullptr)
{
  SetName("SDL Game Controller");
  SetButtonCount(SDL_CONTROLLER_BUTTON_MAX);
  SetAxisCount(SDL_CONTROLLER_AXIS_MAX);
}

bool CJoystickSDL::Equals(const CJoystick* rhs) const
{
  if (rhs == nullptr)
    return false;

  const CJoystickSDL* rhsSDL = dynamic_cast<const CJoystickSDL*>(rhs);
  if (rhsSDL == nullptr)
    return false;

  return m_index == rhsSDL->m_index;
}

bool CJoystickSDL::Initialize(void)
{
  bool bSuccess = false;

  if (CJoystick::Initialize())
  {
    if (m_pController == nullptr)
      m_pController = SDL_GameControllerOpen(m_index);

    if (m_pController != nullptr)
    {
      const char* controllerName = SDL_GameControllerNameForIndex(m_index);
      isyslog("%s %d initialized: \"%s\"", Name().c_str(), m_index,
          controllerName ? controllerName : "");
      bSuccess = true;
    }
  }

  return bSuccess;
}

void CJoystickSDL::Deinitialize(void)
{
  if (m_pController != nullptr)
  {
    SDL_GameControllerClose(m_pController);
    m_pController = nullptr;
  }

  CJoystick::Deinitialize();
}

#if defined(HAVE_PERIPHERAL_JOYSTICK_BATTERY)
bool CJoystickSDL::GetBattery(JOYSTICK_BATTERY& battery)
{
#if SDL_VERSION_ATLEAST(2, 0, 4)
  if (m_pController == nullptr)
    return false;

  SDL_Joystick* joystick = SDL_GameControllerGetJoystick(m_pController);
  if (joystick == nullptr)
    return false;

  switch (SDL_JoystickCurrentPowerLevel(joystick))
  {
    case SDL_JOYSTICK_POWER_WIRED:
      battery.state = JOYSTICK_BATTERY_STATE_WIRED;
      battery.level = -1;
      return true;
    case SDL_JOYSTICK_POWER_EMPTY:
      battery.level = 5;
      break;
    case SDL_JOYSTICK_POWER_LOW:
      battery.level = 20;
      break;
    case SDL_JOYSTICK_POWER_MEDIUM:
      battery.level = 70;
      break;
    case SDL_JOYSTICK_POWER_FULL:
    case SDL_JOYSTICK_POWER_MAX:
      battery.level = 100;
      break;
    default:
      return false;
  }

  battery.state = JOYSTICK_BATTERY_STATE_DISCHARGING;
  return true;
#else
  // SDL only reports the power level from 2.0.4
  static_cast<void>(battery);
  return false;
#endif
}
#endif

bool CJoystickSDL::ScanEvents(void)
{
  if (m_pController == nullptr)
    return false;

  SetButtonValue(0,  SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_A)             ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(1,  SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_B)             ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(2,  SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_X)             ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(3,  SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_Y)             ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(4,  SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER)  ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(5,  SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(6,  SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_BACK)          ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(7,  SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_START)         ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(8,  SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_LEFTSTICK)     ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(9,  SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_RIGHTSTICK)    ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(10, SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_DPAD_UP)       ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(11, SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)    ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(12, SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_DPAD_DOWN)     ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(13, SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_DPAD_LEFT)     ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
  SetButtonValue(14, SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_GUIDE)         ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);

  SetAxisValue(0, (long)SDL_GameControllerGetAxis(m_pController, SDL_CONTROLLER_AXIS_LEFTX), MAX_AXIS);
  SetAxisValue(1, (long)SDL_GameControllerGetAxis(m_pController, SDL_CONTROLLER_AXIS_LEFTY), MAX_AXIS);
  SetAxisValue(2, (long)SDL_GameControllerGetAxis(m_pController, SDL_CONTROLLER_AXIS_RIGHTX), MAX_AXIS);
  SetAxisValue(3, (long)SDL_GameControllerGetAxis(m_pController, SDL_CONTROLLER_AXIS_RIGHTY), MAX_AXIS);
  SetAxisValue(4, (long)SDL_GameControllerGetAxis(m_pController, SDL_CONTROLLER_AXIS_TRIGGERLEFT), MAX_AXIS);
  SetAxisValue(5, (long)SDL_GameControllerGetAxis(m_pController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT), MAX_AXIS);

  return true;
}
