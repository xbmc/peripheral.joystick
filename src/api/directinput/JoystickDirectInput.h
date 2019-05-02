/*
 *      Copyright (C) 2014-2017 Garrett Brown
 *      Copyright (C) 2014-2017 Team Kodi
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
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
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
