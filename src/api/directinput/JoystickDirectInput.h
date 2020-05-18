/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "api/Joystick.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace JOYSTICK
{
  class CJoystickDirectInput : public CJoystick
  {
  public:
    CJoystickDirectInput(GUID                           deviceGuid,
                         LPDIRECTINPUTDEVICE8           joystickDevice,
                         const TCHAR                    *strName);

    virtual ~CJoystickDirectInput(void);

    virtual bool Equals(const CJoystick* rhs) const override;

    virtual bool Initialize(void) override;

  protected:
    virtual bool ScanEvents(void) override;

  private:
    static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE *pdidoi, VOID *pContext);

    GUID m_deviceGuid;
    LPDIRECTINPUTDEVICE8 m_joystickDevice;
    bool m_bAcquired = false;
  };
}
