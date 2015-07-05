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

#include "JoystickDirectInput.h"
#include "JoystickInterfaceDirectInput.h"
#include "log/Log.h"
#include "utils/CommonMacros.h"

using namespace JOYSTICK;

#define AXIS_MIN     -32768  /* minimum value for axis coordinate */
#define AXIS_MAX      32767  /* maximum value for axis coordinate */

#define JOY_POV_360  JOY_POVBACKWARD * 2
#define JOY_POV_NE   (JOY_POVFORWARD + JOY_POVRIGHT) / 2
#define JOY_POV_SE   (JOY_POVRIGHT + JOY_POVBACKWARD) / 2
#define JOY_POV_SW   (JOY_POVBACKWARD + JOY_POVLEFT) / 2
#define JOY_POV_NW   (JOY_POVLEFT + JOY_POV_360) / 2

CJoystickDirectInput::CJoystickDirectInput(GUID                           deviceGuid,
                                           LPDIRECTINPUTDEVICE8           joystickDevice,
                                           const std::string&             strName,
                                           CJoystickInterfaceDirectInput* api)
 : CJoystick(api),
   m_deviceGuid(deviceGuid),
   m_joystickDevice(joystickDevice)
{
  SetName(strName);
}

bool CJoystickDirectInput::Equals(const CJoystick* rhs) const
{
  if (rhs == nullptr)
    return false;

  const CJoystickDirectInput* rhsDirectInput = dynamic_cast<const CJoystickDirectInput*>(rhs);
  if (rhsDirectInput == nullptr)
    return false;

  return m_deviceGuid == rhsDirectInput->m_deviceGuid;
}

bool CJoystickDirectInput::Initialize(void)
{
  HRESULT hr;

  // Get capabilities
  DIDEVCAPS diDevCaps;
  diDevCaps.dwSize = sizeof(DIDEVCAPS);
  hr = m_joystickDevice->GetCapabilities(&diDevCaps);
  if (FAILED(hr))
  {
    esyslog("%s: Failed to GetCapabilities for: %s", __FUNCTION__, Name().c_str());
    return false;
  }

  SetButtonCount(diDevCaps.dwButtons);
  SetHatCount(diDevCaps.dwPOVs);
  SetAxisCount(diDevCaps.dwAxes);

  // Get vendor and product ID
  DIPROPDWORD diDevProperty;
  diDevProperty.diph.dwSize = sizeof(DIPROPDWORD);
  diDevProperty.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  diDevProperty.diph.dwObj = 0; // device property
  diDevProperty.diph.dwHow = DIPH_DEVICE;

  hr = m_joystickDevice->GetProperty(DIPROP_VIDPID, &diDevProperty.diph);
  if (FAILED(hr))
  {
    esyslog("%s: Failed to GetProperty for: %s", __FUNCTION__, Name().c_str());
    return false;
  }

  SetVendorID(LOWORD(diDevProperty.dwData));
  SetProductID(HIWORD(diDevProperty.dwData));

  // Initialize axes
  // Enumerate the joystick objects. The callback function enables user
  // interface elements for objects that are found, and sets the min/max
  // values properly for discovered axes.
  hr = m_joystickDevice->EnumObjects(EnumObjectsCallback, m_joystickDevice, DIDFT_ALL);
  if (FAILED(hr))
  {
    esyslog("%s: Failed to enumerate objects", __FUNCTION__);
    return false;
  }

  return CJoystick::Initialize();
}

//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a
//       joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL CALLBACK CJoystickDirectInput::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext)
{
  LPDIRECTINPUTDEVICE8 pJoy = static_cast<LPDIRECTINPUTDEVICE8>(pContext);

  // For axes that are returned, set the DIPROP_RANGE property for the
  // enumerated axis in order to scale min/max values.
  if (pdidoi->dwType & DIDFT_AXIS)
  {
    DIPROPRANGE diprg;
    diprg.diph.dwSize = sizeof(DIPROPRANGE);
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    diprg.diph.dwHow = DIPH_BYID;
    diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
    diprg.lMin = AXIS_MIN;
    diprg.lMax = AXIS_MAX;

    // Set the range for the axis
    HRESULT hr = pJoy->SetProperty(DIPROP_RANGE, &diprg.diph);
    if (FAILED(hr))
      esyslog("%s : Failed to set property on %s", __FUNCTION__, pdidoi->tszName);
  }
  return DIENUM_CONTINUE;
}

bool CJoystickDirectInput::ScanEvents(void)
{
  HRESULT     hr;
  DIJOYSTATE2 js; // DInput joystick state

  hr = m_joystickDevice->Poll();

  if (FAILED(hr))
  {
    int i = 0;
    // DInput is telling us that the input stream has been interrupted. We
    // aren't tracking any state between polls, so we don't have any special
    // reset that needs to be done. We just re-acquire and try again 10 times.
    do
    {
      hr = m_joystickDevice->Acquire();
    } while (hr == DIERR_INPUTLOST && i++ < 10);

    // hr may be DIERR_OTHERAPPHASPRIO or other errors. This may occur when the
    // app is minimized or in the process of switching, so just try again later.
    return false;
  }

  // Get the input's device state
  hr = m_joystickDevice->GetDeviceState(sizeof(DIJOYSTATE2), &js);
  if (FAILED(hr))
    return false; // The device should have been acquired during the Poll()

  // Gamepad buttons
  for (unsigned int b = 0; b < ButtonCount(); b++)
    SetButtonValue(b, (js.rgbButtons[b] & 0x80) ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);

  // Gamepad hats
  for (unsigned int h = 0; h < HatCount(); h++)
  {
    JOYSTICK_STATE_HAT hatState = JOYSTICK_STATE_HAT_UNPRESSED;

    const bool bCentered = ((js.rgdwPOV[h] & 0xFFFF) == 0xFFFF);
    if (!bCentered)
    {
      if ((JOY_POV_NW <= js.rgdwPOV[h] && js.rgdwPOV[h] <= JOY_POV_360) || js.rgdwPOV[h] <= JOY_POV_NE)
        hatState = JOYSTICK_STATE_HAT_UP;
      else if (JOY_POV_SE <= js.rgdwPOV[h] && js.rgdwPOV[h] <= JOY_POV_SW)
        hatState = JOYSTICK_STATE_HAT_DOWN;

      if (JOY_POV_NE <= js.rgdwPOV[h] && js.rgdwPOV[h] <= JOY_POV_SE)
        hatState = (JOYSTICK_STATE_HAT)(hatState | JOYSTICK_STATE_HAT_RIGHT);
      else if (JOY_POV_SW <= js.rgdwPOV[h] && js.rgdwPOV[h] <= JOY_POV_NW)
        hatState = (JOYSTICK_STATE_HAT)(hatState | JOYSTICK_STATE_HAT_LEFT);
    }

    SetHatValue(h, hatState);
  }

  // Gamepad axes
  const long amounts[] = { js.lX, js.lY, js.lZ, js.lRx, js.lRy, js.lRz, js.rglSlider[0], js.rglSlider[1] };
  for (unsigned int a = 0; a < ARRAY_SIZE(amounts); a++)
    SetAxisValue(a, amounts[a], AXIS_MAX);

  return true;
}
