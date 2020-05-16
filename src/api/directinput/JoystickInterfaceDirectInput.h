/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "api/JoystickInterfaceCallback.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>

namespace JOYSTICK
{
  class CJoystickInterfaceDirectInput : public CJoystickInterfaceCallback
  {
  public:
    CJoystickInterfaceDirectInput(void);
    virtual ~CJoystickInterfaceDirectInput(void) { Deinitialize(); }

    // implementation of IJoystickInterface
    virtual EJoystickInterface Type(void) const override;
    virtual bool Initialize(void) override;
    virtual void Deinitialize(void) override;
    virtual bool ScanForJoysticks(JoystickVector& joysticks) override;

  private:
    bool InitializeDirectInput(void);

    static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext);
    static bool IsXInputDevice(const GUID *pGuidProductFromDirectInput);
    static HWND GetMainWindowHandle(void);
    static BOOL CALLBACK EnumWindowsCallback(HWND hnd, LPARAM lParam);

    HWND                    m_hWnd;         // Main window
    LPDIRECTINPUT8          m_pDirectInput; // DirectInput handle, we hold onto it and release it when freeing resources
  };
}
