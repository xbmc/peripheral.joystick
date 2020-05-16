/*
 *  Copyright (C) 2016-2017 Sam Lantinga
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "api/Joystick.h"

typedef struct _SDL_GameController SDL_GameController;

namespace JOYSTICK
{
  class CJoystickSDL : public CJoystick
  {
  public:
    CJoystickSDL(unsigned int index);
    virtual ~CJoystickSDL(void) { Deinitialize(); }

    // implementation of CJoystick
    virtual bool Equals(const CJoystick* rhs) const override;
    virtual bool Initialize(void) override;
    virtual void Deinitialize(void) override;

  protected:
    virtual bool ScanEvents(void) override;

  private:
    // Construction parameters
    const unsigned int m_index;

    // SDL parameters
    SDL_GameController *m_pController;
  };
}
