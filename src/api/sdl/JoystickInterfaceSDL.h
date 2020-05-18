/*
 *  Copyright (C) 2016-2017 Sam Lantinga
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "api/IJoystickInterface.h"

namespace JOYSTICK
{
  class CJoystickInterfaceSDL : public IJoystickInterface
  {
  public:
    CJoystickInterfaceSDL(void) { }
    virtual ~CJoystickInterfaceSDL(void) { Deinitialize(); }

    // Implementation of IJoystickInterface
    virtual EJoystickInterface Type(void) const override;
    virtual bool Initialize(void) override;
    virtual void Deinitialize(void) override;
    virtual bool ScanForJoysticks(JoystickVector& joysticks) override;

  private:
    bool m_bInitialScan = true;
  };
}
