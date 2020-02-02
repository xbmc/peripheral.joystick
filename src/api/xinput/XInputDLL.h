/*
 *      Copyright (C) 2014-2020 Garrett Brown
 *      Copyright (C) 2014-2020 Team Kodi
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

#include <mutex>
#include <string>
#include <windows.h>
#include <Xinput.h>

// Missing from XInput API
#define XINPUT_GAMEPAD_GUIDE  0x0400

namespace JOYSTICK
{
  struct XINPUT_GAMEPAD_EX
  {
    WORD  wButtons;
    BYTE  bLeftTrigger;
    BYTE  bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
    DWORD dwPaddingReserved;
  };

  struct XINPUT_STATE_EX
  {
    DWORD             dwPacketNumber;
    XINPUT_GAMEPAD_EX Gamepad;
  };

  class CXInputDLL
  {
  private:
    CXInputDLL(void);

  public:
    static CXInputDLL& Get(void);
    virtual ~CXInputDLL(void) { Unload(); }

    bool Load(void);
    void Unload(void);

    // Available after library is loaded successfully
    const std::string& Version(void) const { return m_strVersion; }

    bool HasGuideButton(void) const;
    bool SupportsPowerOff() const;

    bool GetState(unsigned int controllerId, XINPUT_STATE& state);
    bool GetStateWithGuide(unsigned int controllerId, XINPUT_STATE_EX& state);
    bool SetState(unsigned int controllerId, XINPUT_VIBRATION& vibration);
    bool GetCapabilities(unsigned int controllerId, XINPUT_CAPABILITIES& caps);

    enum class BatteryDeviceType
    {
      Controller,
      Headset
    };
    bool GetBatteryInformation(unsigned int controllerId, BatteryDeviceType deviceType, XINPUT_BATTERY_INFORMATION& battery);

    bool PowerOff(unsigned int controllerId);

  private:
    // Forward decl's for XInput API's we load dynamically and use if available
    // [in] Index of the gamer associated with the device
    // [out] Receives the current state
    typedef DWORD (WINAPI* FnXInputGetState)(DWORD dwUserIndex, XINPUT_STATE* pState);
    typedef DWORD (WINAPI* FnXInputGetStateEx)(DWORD dwUserIndex, XINPUT_STATE_EX* pState);

    // [in] Index of the gamer associated with the device
    // [in, out] The vibration information to send to the controller
    typedef DWORD (WINAPI* FnXInputSetState)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);

    // [in] Index of the gamer associated with the device
    // [in] Input flags that identify the device type
    // [out] Receives the capabilities
    typedef DWORD (WINAPI* FnXInputGetCapabilities)(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);

    // [in] Index of the gamer associated with the device
    // [in] Device associated with this user index to be queried. Must be BATTERY_DEVTYPE_GAMEPAD or BATTERY_DEVTYPE_HEADSET.
    typedef DWORD (WINAPI* FnXInputGetBatteryInformation)(DWORD dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION *pBatteryInformation);

    // [in] Index of the gamer associated with the device
    typedef DWORD (WINAPI* FnXInputPowerOffController)(DWORD dwUserIndex);

    HMODULE m_dll;
    std::string m_strVersion;
    FnXInputGetState m_getState;
    FnXInputGetStateEx m_getStateEx;
    FnXInputSetState m_setState;
    FnXInputGetCapabilities m_getCaps;
    FnXInputGetBatteryInformation m_getBatteryInfo;
    FnXInputPowerOffController m_powerOff;
    std::recursive_mutex m_mutex;
  };
}
