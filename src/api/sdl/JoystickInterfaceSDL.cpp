/*
 *  Copyright (C) 2016-2017 Sam Lantinga
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickInterfaceSDL.h"
#include "JoystickSDL.h"
#include "api/JoystickTypes.h"
#include "log/Log.h"

#include <SDL2/SDL.h>

using namespace JOYSTICK;

EJoystickInterface CJoystickInterfaceSDL::Type(void) const
{
  return EJoystickInterface::SDL;
}

bool CJoystickInterfaceSDL::Initialize(void)
{
  return (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) == 0);
}

void CJoystickInterfaceSDL::Deinitialize(void)
{
  SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
}

bool CJoystickInterfaceSDL::ScanForJoysticks(JoystickVector& joysticks)
{
  const int count = SDL_NumJoysticks();

  if (m_bInitialScan)
    dsyslog("SDL: Initial scan: %d joystick%s", count, count == 1 ? "" : "s");

  for (int i = 0; i < count; i++)
  {
    if (!SDL_IsGameController(i))
    {
      if (m_bInitialScan)
        dsyslog("SDL: Joystick %d is not a game controller", i);
      continue;
    }

    joysticks.push_back(JoystickPtr(new CJoystickSDL(i)));
  }

  m_bInitialScan = false;

  return true;
}
