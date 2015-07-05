/*
 *      Copyright (C) 2014-2015 Garrett Brown
 *      Copyright (C) 2014-2015 Team XBMC
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "api/JoystickInterfaceCallback.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <set>
#include <vector>

namespace JOYSTICK
{
  class CJoystickInterfaceDirectInput : public CJoystickInterfaceCallback
  {
  public:
    CJoystickInterfaceDirectInput(void);
    virtual ~CJoystickInterfaceDirectInput(void) { Deinitialize(); }

    // implementation of IJoystickInterface
    virtual const char* Name(void) const;
    virtual bool Initialize(void);
    virtual void Deinitialize(void);
    virtual bool ScanForJoysticks(std::vector<CJoystick*>& joysticks);

  private:
    static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext);
    static bool IsXInputDevice(const GUID *pGuidProductFromDirectInput); // TODO: Move to XInput
    static std::set<HWND> GetMainWindowHandles(void);
    static BOOL CALLBACK EnumWindowsCallback(HWND hnd, LPARAM lParam);

    HWND                    m_hWnd;         // Main window
    LPDIRECTINPUT8          m_pDirectInput; // DirectInput handle, we hold onto it and release it when freeing resources
    bool                    m_bInitialized; // Whether DirectInput has been initialized or not
  };
}
