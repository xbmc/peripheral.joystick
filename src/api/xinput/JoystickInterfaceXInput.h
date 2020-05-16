/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "api/IJoystickInterface.h"
#include "buttonmapper/ButtonMapTypes.h"

namespace JOYSTICK
{
  class CJoystickInterfaceXInput : public IJoystickInterface
  {
  public:
    CJoystickInterfaceXInput(void) { }
    virtual ~CJoystickInterfaceXInput(void) { Deinitialize(); }

    // Implementation of IJoystickInterface
    virtual EJoystickInterface Type(void) const override;
    virtual bool Initialize(void) override;
    virtual void Deinitialize(void) override;
    virtual bool SupportsRumble(void) const override { return true; }
    virtual bool SupportsPowerOff(void) const override;
    virtual bool ScanForJoysticks(JoystickVector& joysticks) override;
    virtual const ButtonMap& GetButtonMap() override;

  private:
    static ButtonMap m_buttonMap;
  };
}
