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

#include "XInputDLL.h"
#include "log/Log.h"

using namespace JOYSTICK;

CXInputDLL::CXInputDLL(void)
 : m_dll(NULL),
   m_getState(NULL),
   m_getStateEx(NULL),
   m_setState(NULL),
   m_getCaps(NULL),
   m_getBatteryInfo(NULL),
   m_powerOff(NULL)
{
}

CXInputDLL& CXInputDLL::Get(void)
{
  static CXInputDLL _instance;
  return _instance;
}

bool CXInputDLL::Load(void)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  m_strVersion = "1.4";
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY != WINAPI_FAMILY_APP)
  dsyslog("Attempting to load XInput1_4.dll...");
  m_dll = LoadLibrary(TEXT("XInput1_4.dll"));  // 1.4 Ships with Windows 8
  if (!m_dll)
  {
    m_strVersion = "1.3";
    dsyslog("Attempting to load XInput1_3.dll...");
    m_dll = LoadLibrary(TEXT("XInput1_3.dll"));  // 1.3 Ships with Vista and Win7, can be installed as a redistributable component
    if (!m_dll)
    {
      dsyslog("Attempting to load bin\\XInput1_3.dll...");
      m_dll = LoadLibrary(TEXT("bin\\XInput1_3.dll"));
    }
  }

  if (!m_dll)
  {
    esyslog("Failed to load XInput DLL");
  }
  else
#endif // ! WINAPI_FAMILY_APP
  {
    dsyslog("Loaded XInput DLL version %s", m_strVersion.c_str());

    // 100 is the ordinal for _XInputGetStateEx, which returns the same struct as
    // XinputGetState, but with extra data in wButtons for the guide button, we think...
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY != WINAPI_FAMILY_APP)
    if (HasGuideButton())
      m_getStateEx = (FnXInputGetStateEx)GetProcAddress(m_dll, reinterpret_cast<LPCSTR>(100));
    else
      m_getState = (FnXInputGetState)GetProcAddress(m_dll, "XInputGetState");
    m_setState = (FnXInputSetState)GetProcAddress(m_dll, "XInputSetState");
    m_getCaps  = (FnXInputGetCapabilities)GetProcAddress(m_dll, "XInputGetCapabilities");
    m_getBatteryInfo = (FnXInputGetBatteryInformation)GetProcAddress(m_dll, "XInputGetBatteryInformation");
    if (SupportsPowerOff())
      m_powerOff = (FnXInputPowerOffController)GetProcAddress(m_dll, reinterpret_cast<LPCSTR>(103));
#else
    m_getState = XInputGetState;
    m_setState = XInputSetState;
    m_getCaps = XInputGetCapabilities;
    m_getBatteryInfo = XInputGetBatteryInformation;
#endif
    if ((m_getState || m_getStateEx) && m_setState && m_getCaps && m_getBatteryInfo)
    {
      dsyslog("Loaded XInput symbols (GetState=%p, GetStateEx=%p, SetState=%p, GetCaps=%p, GetBatteryInformation=%p, PowerOff=%p)",
              m_getState, m_getStateEx, m_setState, m_getCaps, m_getBatteryInfo, m_powerOff);
      return true;
    }
    else
    {
      esyslog("Failed to load one or more symbols (GetState=%p, GetStateEx=%p, SetState=%p, GetCaps=%p, GetBatteryInformation=%p, PowerOff=%p)",
              m_getState, m_getStateEx, m_setState, m_getCaps, m_getBatteryInfo, m_powerOff);
    }
  }

  Unload();
  return false;
}

void CXInputDLL::Unload(void)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY != WINAPI_FAMILY_APP)
  if (m_dll)
    FreeLibrary(m_dll);
#endif

  m_strVersion.clear();
  m_getState = NULL;
  m_getStateEx = NULL;
  m_setState = NULL;
  m_getCaps  = NULL;
  m_getBatteryInfo  = NULL;
  m_powerOff = NULL;
  m_dll      = NULL;
}

bool CXInputDLL::HasGuideButton(void) const
{
  return m_strVersion == "1.3";
}

bool CXInputDLL::SupportsPowerOff(void) const
{
  return m_strVersion == "1.3";
}

bool CXInputDLL::GetState(unsigned int controllerId, XINPUT_STATE& state)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  if (!m_getState)
    return false;

  DWORD result = m_getState(controllerId, &state);
  if (result != ERROR_SUCCESS)
  {
    if (result != ERROR_DEVICE_NOT_CONNECTED)
      esyslog("Failed to get XInput state on port %u (result=%u)", controllerId, result);
    return false;
  }

  return true;
}

bool CXInputDLL::GetStateWithGuide(unsigned int controllerId, XINPUT_STATE_EX& state)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  if (!m_getStateEx)
    return false;

  DWORD result = m_getStateEx(controllerId, &state);
  if (result != ERROR_SUCCESS)
  {
    if (result != ERROR_DEVICE_NOT_CONNECTED)
      esyslog("Failed to get XInput state on port %u (result=%u)", controllerId, result);
    return false;
  }

  return true;
}

bool CXInputDLL::SetState(unsigned int controllerId, XINPUT_VIBRATION& vibration)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  if (!m_setState)
    return false;

  DWORD result = m_setState(controllerId, &vibration);
  if (result != ERROR_SUCCESS)
  {
    if (result == ERROR_DEVICE_NOT_CONNECTED)
      dsyslog("No XInput devices on port %u", controllerId);
    else
      esyslog("Failed to set XInput state on port %u (result=%u)", controllerId, result);
    return false;
  }

  return true;
}

bool CXInputDLL::GetCapabilities(unsigned int controllerId, XINPUT_CAPABILITIES& caps)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  if (!m_getCaps)
    return false;

  // currently no other value than XINPUT_FLAG_GAMEPAD is supported for the second parameter
  DWORD result = m_getCaps(controllerId, XINPUT_FLAG_GAMEPAD, &caps);
  if (result != ERROR_SUCCESS)
  {
    if (result == ERROR_DEVICE_NOT_CONNECTED)
      dsyslog("No XInput devices on port %u", controllerId);
    else
      esyslog("Failed to get XInput capabilities on port %u (result=%u)", controllerId, result);
    return false;
  }

  return true;
}

bool CXInputDLL::GetBatteryInformation(unsigned int controllerId, BatteryDeviceType deviceType, XINPUT_BATTERY_INFORMATION& battery)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  if (!m_getBatteryInfo)
    return false;

  BYTE devType;
  switch (deviceType)
  {
    case BatteryDeviceType::Controller:
      devType = BATTERY_DEVTYPE_GAMEPAD;
      break;

    case BatteryDeviceType::Headset:
      devType = BATTERY_DEVTYPE_HEADSET;
      break;

    default:
      esyslog("Invalid battery device type %d", deviceType);
      return false;
  }

  DWORD result = m_getBatteryInfo(controllerId, devType, &battery);
  if (result != ERROR_SUCCESS)
  {
    if (result == ERROR_DEVICE_NOT_CONNECTED)
      dsyslog("No XInput devices on port %u", controllerId);
    else
      esyslog("Failed to get XInput battery information on port %u (result=%u)", controllerId, result);
    return false;
  }

  return true;
}

bool CXInputDLL::PowerOff(unsigned int controllerId)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  if (!m_powerOff)
    return false;

  DWORD result = m_powerOff(controllerId);
  if (result != ERROR_SUCCESS)
  {
    if (result == ERROR_DEVICE_NOT_CONNECTED)
      dsyslog("No XInput devices on port %u", controllerId);
    else
      esyslog("Failed to power off XInput device on port %u (result=%u)", controllerId, result);
    return false;
  }

  return true;
}
