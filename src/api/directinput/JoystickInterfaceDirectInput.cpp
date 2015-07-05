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

#include "JoystickInterfaceDirectInput.h"
#include "JoystickDirectInput.h"
#include "api/JoystickTypes.h"
#include "log/Log.h"
#include "utils/CommonMacros.h"

#include "libKODI_peripheral.h"

// For getting the GUIDs of XInput devices
#include <wbemidl.h>
#include <oleauto.h>

#pragma comment(lib, "Dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace JOYSTICK;

namespace JOYSTICK
{
  struct EnumWindowsCallbackArgs
  {
    DWORD pid;
    std::set<HWND> handles;
  };
}

CJoystickInterfaceDirectInput::CJoystickInterfaceDirectInput(void)
  : m_hWnd(NULL),
    m_pDirectInput(NULL),
    m_bInitialized(false)
{ }

const char* CJoystickInterfaceDirectInput::Name(void) const
{
  return INTERFACE_DIRECTINPUT;
}

bool CJoystickInterfaceDirectInput::Initialize(void)
{
  if (m_bInitialized)
    return true;

  // get the main window
  if (m_hWnd == NULL)
  {
    // get all window handles of our process
    std::set<HWND> hWnds = GetMainWindowHandles();
    if (hWnds.empty())
      return true; // return true even if it failed because we can still initialize in ScanForJoysticks()

    // check if one of the windows is visible
    for (std::set<HWND>::const_iterator hwnd = hWnds.begin(); hwnd != hWnds.end(); ++hwnd)
    {
      if (IsWindowVisible(*hwnd))
      {
        m_hWnd = *hwnd;
        break;
      }
    }

    if (m_hWnd == NULL)
      return true; // return true even if it failed because we can still initialize in ScanForJoysticks()
  }

  HRESULT hr;

  hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<VOID**>(&m_pDirectInput), NULL);
  if (FAILED(hr) || m_pDirectInput == NULL)
  {
    esyslog("%s: Failed to create DirectInput", __FUNCTION__);
    return false;
  }

  m_bInitialized = true;
  return true;
}

void CJoystickInterfaceDirectInput::Deinitialize(void)
{
  SAFE_RELEASE(m_pDirectInput);

  m_hWnd = NULL;
  m_bInitialized = false;
}

bool CJoystickInterfaceDirectInput::ScanForJoysticks(std::vector<CJoystick*>& joysticks)
{
  if (!Initialize() || !m_bInitialized)
    return false;

  HRESULT hr;

  hr = m_pDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
  if (FAILED(hr))
  {
    esyslog("%s: Joystick enumeration failed", __FUNCTION__);
    return false;
  }

  GetScanResults(joysticks);

  return true;
}

BOOL CALLBACK CJoystickInterfaceDirectInput::EnumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)
{
  HRESULT hr;

  // Skip verified XInput devices
  if (IsXInputDevice(&pdidInstance->guidProduct))
    return DIENUM_CONTINUE;

  CJoystickInterfaceDirectInput* context = static_cast<CJoystickInterfaceDirectInput*>(pContext);

  LPDIRECTINPUTDEVICE8 pJoystick = NULL;

  // Obtain an interface to the enumerated joystick.
  hr = context->m_pDirectInput->CreateDevice(pdidInstance->guidInstance, &pJoystick, NULL);
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

  // Set the cooperative level to let DInput know how this device should
  // interact with the system and with other DInput applications.
  hr = pJoystick->SetCooperativeLevel(context->m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
  if (FAILED(hr))
  {
    esyslog("%s: Failed to SetCooperativeLevel on: %s", __FUNCTION__, pdidInstance->tszProductName);
    return DIENUM_CONTINUE;
  }

  const std::string strName = pdidInstance->tszProductName ? pdidInstance->tszProductName : "";

  context->AddScanResult(new CJoystickDirectInput(pdidInstance->guidInstance, pJoystick, strName, context));

  return DIENUM_CONTINUE;
}

//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains
// "IG_" (ex. "VID_045E&PID_028E&IG_00"). If it does, then it's an XInput device.
// Unfortunately this information can not be found by just using DirectInput.
// See http://msdn.microsoft.com/en-us/library/windows/desktop/ee417014(v=vs.85).aspx
//-----------------------------------------------------------------------------
bool CJoystickInterfaceDirectInput::IsXInputDevice(const GUID* pGuidProductFromDirectInput)
{
  IWbemLocator*         pIWbemLocator = NULL;
  IEnumWbemClassObject* pEnumDevices = NULL;
  IWbemClassObject*     pDevices[20] = { 0 };
  IWbemServices*        pIWbemServices = NULL;
  BSTR                  bstrNamespace = NULL;
  BSTR                  bstrDeviceID = NULL;
  BSTR                  bstrClassName = NULL;
  DWORD                 uReturned = 0;
  bool                  bIsXinputDevice = false;
  UINT                  iDevice = 0;
  VARIANT               var;
  HRESULT               hr;

  // CoInit if needed
  hr = CoInitialize(NULL);
  bool bCleanupCOM = SUCCEEDED(hr);

  try
  {
    // Create WMI
    hr = CoCreateInstance(__uuidof(WbemLocator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IWbemLocator), (LPVOID*)&pIWbemLocator);
    if (FAILED(hr) || pIWbemLocator == NULL)
      throw hr;

    bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2");
    if (bstrNamespace == NULL)
      throw hr;

    bstrClassName = SysAllocString(L"Win32_PNPEntity");
    if (bstrClassName == NULL)
      throw hr;

    bstrDeviceID = SysAllocString(L"DeviceID");
    if (bstrDeviceID == NULL)
      throw hr;

    // Connect to WMI
    hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L, 0L, NULL, NULL, &pIWbemServices);
    if (FAILED(hr) || pIWbemServices == NULL)
      throw hr;

    // Switch security level to IMPERSONATE
    CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL,
      RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

    hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
    if (FAILED(hr) || pEnumDevices == NULL)
      throw hr;

    // Loop over all devices
    do
    {
      // Get 20 at a time
      hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
      if (FAILED(hr))
        throw hr;

      for (iDevice = 0; iDevice < uReturned; iDevice++)
      {
        // Don't compare IDs if we already found our XInput device
        if (!bIsXinputDevice)
        {
          // For each device, get its device ID
          hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
          if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL)
          {
            // Check if the device ID contains "IG_". If it does, then it's an XInput
            // device. This information can not be found from DirectInput.
            if (wcsstr(var.bstrVal, L"IG_"))
            {
              // If it does, then get the VID/PID from var.bstrVal
              DWORD dwPid = 0;
              DWORD dwVid = 0;
              WCHAR *strVid = wcsstr(var.bstrVal, L"VID_");
              if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1)
                dwVid = 0;
              WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
              if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1)
                dwPid = 0;

              // Compare the VID/PID to the DInput device
              DWORD dwVidPid = MAKELONG(dwVid, dwPid);
              if (dwVidPid == pGuidProductFromDirectInput->Data1)
                bIsXinputDevice = true;
            }
          }
        }

        SAFE_RELEASE(pDevices[iDevice]);
      }
    } while (uReturned);
  }
  catch (HRESULT hr_error)
  {
    esyslog("%s: Error while testing for XInput device! hr=%ld", __FUNCTION__, hr_error);
  }

  if (bstrNamespace)
    SysFreeString(bstrNamespace);
  if (bstrDeviceID)
    SysFreeString(bstrDeviceID);
  if (bstrClassName)
    SysFreeString(bstrClassName);
  for (iDevice = 0; iDevice < 20; iDevice++)
    SAFE_RELEASE(pDevices[iDevice]);
  SAFE_RELEASE(pEnumDevices);
  SAFE_RELEASE(pIWbemLocator);
  SAFE_RELEASE(pIWbemServices);

  if (bCleanupCOM)
    CoUninitialize();

  return bIsXinputDevice;
}

std::set<HWND> CJoystickInterfaceDirectInput::GetMainWindowHandles(void)
{
  EnumWindowsCallbackArgs args = { ::GetCurrentProcessId() };
  if (::EnumWindows(&EnumWindowsCallback, (LPARAM)&args) == FALSE)
    esyslog("Failed to get main window handle");

  return args.handles;
}

BOOL CALLBACK CJoystickInterfaceDirectInput::EnumWindowsCallback(HWND hnd, LPARAM lParam)
{
  EnumWindowsCallbackArgs* args = reinterpret_cast<EnumWindowsCallbackArgs*>(lParam);

  DWORD windowPID;
  (void)::GetWindowThreadProcessId(hnd, &windowPID);
  if (windowPID == args->pid && IsWindow(hnd))
    args->handles.insert(hnd);

  return TRUE;
}
