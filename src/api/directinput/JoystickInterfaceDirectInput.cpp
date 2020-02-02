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

#include "JoystickInterfaceDirectInput.h"
#include "JoystickDirectInput.h"
#include "api/JoystickTypes.h"
#include "log/Log.h"
#include "utils/CommonMacros.h"

#include <kodi/addon-instance/Peripheral.h>

#pragma comment(lib, "Dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace JOYSTICK;

namespace JOYSTICK
{
  struct EnumWindowsCallbackArgs
  {
    DWORD pid;
    HWND* handle;
  };
}

CJoystickInterfaceDirectInput::CJoystickInterfaceDirectInput(void)
  : m_hWnd(NULL),
    m_pDirectInput(NULL)
{ }

EJoystickInterface CJoystickInterfaceDirectInput::Type(void) const
{
  return EJoystickInterface::DIRECTINPUT;
}

bool CJoystickInterfaceDirectInput::Initialize(void)
{
  // Defer initialization until we have the main window handle
  return true;
}

bool CJoystickInterfaceDirectInput::InitializeDirectInput(void)
{
  if (m_pDirectInput == NULL)
  {
    if (m_hWnd == NULL)
      m_hWnd = GetMainWindowHandle();

    if (m_hWnd != NULL)
    {
      HRESULT hr;

      hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<VOID**>(&m_pDirectInput), NULL);
      if (FAILED(hr) || m_pDirectInput == NULL)
        esyslog("%s: Failed to create DirectInput", __FUNCTION__);
    }
  }

  return m_pDirectInput != NULL;
}

void CJoystickInterfaceDirectInput::Deinitialize(void)
{
  SAFE_RELEASE(m_pDirectInput);
  m_hWnd = NULL;
}

bool CJoystickInterfaceDirectInput::ScanForJoysticks(JoystickVector& joysticks)
{
  if (InitializeDirectInput())
  {
    HRESULT hr;

    hr = m_pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
    if (FAILED(hr))
    {
      esyslog("%s: Joystick enumeration failed", __FUNCTION__);
      return false;
    }

    GetScanResults(joysticks);
  }

  return true;
}

BOOL CALLBACK CJoystickInterfaceDirectInput::EnumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)
{
  // Skip verified XInput devices
  if (IsXInputDevice(&pdidInstance->guidProduct))
    return DIENUM_CONTINUE;

  CJoystickInterfaceDirectInput* context = static_cast<CJoystickInterfaceDirectInput*>(pContext);

  LPDIRECTINPUTDEVICE8 pJoystick = NULL;

  // Obtain an interface to the enumerated joystick.
  HRESULT hr = context->m_pDirectInput->CreateDevice(pdidInstance->guidInstance, &pJoystick, NULL);
  if (FAILED(hr) || pJoystick == NULL)
  {
    esyslog("%s: Failed to CreateDevice: %s", __FUNCTION__, pdidInstance->tszProductName);
    return DIENUM_CONTINUE;
  }

  // Set the data format to "simple joystick" - a predefined data format.
  // A data format specifies which controls on a device we are interested in,
  // and how they should be reported. This tells DInput that we will be
  // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
  hr = pJoystick->SetDataFormat(&c_dfDIJoystick2);
  if (FAILED(hr))
  {
    esyslog("%s: Failed to SetDataFormat on: %s", __FUNCTION__, pdidInstance->tszProductName);
    return DIENUM_CONTINUE;
  }

  if (!context->m_hWnd)
    return DIENUM_CONTINUE;

  // Set the cooperative level to let DInput know how this device should
  // interact with the system and with other DInput applications.
  hr = pJoystick->SetCooperativeLevel(context->m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
  if (FAILED(hr))
  {
    esyslog("%s: Failed to SetCooperativeLevel on: %s", __FUNCTION__, pdidInstance->tszProductName);
    return DIENUM_CONTINUE;
  }

  const TCHAR *strName = pdidInstance->tszProductName ? pdidInstance->tszProductName : TEXT("");

  context->AddScanResult(JoystickPtr(new CJoystickDirectInput(pdidInstance->guidInstance, pJoystick, strName)));

  return DIENUM_CONTINUE;
}

//-----------------------------------------------------------------------------
// This implementation has been taken from
// https://github.com/rheit/zdoom/blob/master/src/win32/i_dijoy.cpp
// The theory of operation is the same as for the code provided by Microsoft at
// http://msdn.microsoft.com/en-us/library/windows/desktop/ee417014(v=vs.85).aspx,
// except that we use the Raw Input device list to find the device ID instead of WMI.
// This is a huge order of magnitude faster than WMI (around 10000 times faster!)
//-----------------------------------------------------------------------------
bool CJoystickInterfaceDirectInput::IsXInputDevice(const GUID *pGuidProductFromDirectInput)
{
  UINT nDevices;
  if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0U)
    return false;

  RAWINPUTDEVICELIST *devices;
  if ((devices = (RAWINPUTDEVICELIST *)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL)
    return false;

  UINT numDevices;
  if ((numDevices = GetRawInputDeviceList(devices, &nDevices, sizeof(RAWINPUTDEVICELIST))) == (UINT)-1)
  {
    free(devices);
    return false;
  }

  bool isXInput = false;
  for (UINT i = 0; i < numDevices; ++i)
  {
    // I am making the assumption here that all possible XInput devices will
    // report themselves as generic HID devices and not as keyboards or mice
    if (devices[i].dwType == RIM_TYPEHID)
    {
      RID_DEVICE_INFO rdi;
      UINT cbSize = rdi.cbSize = sizeof(rdi);
      if ((INT)GetRawInputDeviceInfoA(devices[i].hDevice, RIDI_DEVICEINFO, &rdi, &cbSize) >= 0)
      {
        if (MAKELONG(rdi.hid.dwVendorId, rdi.hid.dwProductId) == (LONG)pGuidProductFromDirectInput->Data1)
        {
          char name[256];
          UINT namelen = sizeof(name);
          UINT reslen = GetRawInputDeviceInfoA(devices[i].hDevice, RIDI_DEVICENAME, name, &namelen);
          if (reslen != (UINT)-1)
          {
            isXInput = strstr(name, "IG_") != NULL;
            break;
          }
        }
      }
    }
  }
  free(devices);

  return isXInput;
}

HWND CJoystickInterfaceDirectInput::GetMainWindowHandle(void)
{
  HWND hWnd = NULL;

  EnumWindowsCallbackArgs args = { ::GetCurrentProcessId(), &hWnd };
  if (::EnumWindows(&EnumWindowsCallback, (LPARAM)&args) == FALSE)
    esyslog("Failed to get main window handle");

  return hWnd;
}

BOOL CALLBACK CJoystickInterfaceDirectInput::EnumWindowsCallback(HWND hnd, LPARAM lParam)
{
  EnumWindowsCallbackArgs* args = reinterpret_cast<EnumWindowsCallbackArgs*>(lParam);

  DWORD windowPID;
  (void)::GetWindowThreadProcessId(hnd, &windowPID);
  if (windowPID == args->pid && IsWindow(hnd) && IsWindowVisible(hnd))
    *args->handle = hnd;

  return TRUE;
}
