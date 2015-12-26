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
 */

#include "AnomalousTriggerFilter.h"
#include "Joystick.h"
#include "log/Log.h"
#include "settings/Settings.h"
#include "utils/CommonMacros.h"
#include "utils/StringUtils.h"

#include "platform/util/timeutils.h"

using namespace JOYSTICK;

#define ANALOG_EPSILON  0.0001f

CJoystick::CJoystick(const std::string& strProvider)
 : m_discoverTimeMs(PLATFORM::GetTimeMs()),
   m_activateTimeMs(-1),
   m_firstEventTimeMs(-1),
   m_lastEventTimeMs(-1)
{
  SetProvider(strProvider);
}

bool CJoystick::Equals(const CJoystick* rhs) const
{
  return rhs &&
         Type()          == rhs->Type()          &&
         Name()          == rhs->Name()          &&
         VendorID()      == rhs->VendorID()      &&
         ProductID()     == rhs->ProductID()     &&
         Provider()      == rhs->Provider()      &&
         RequestedPort() == rhs->RequestedPort() &&
         ButtonCount()   == rhs->ButtonCount()   &&
         HatCount()      == rhs->HatCount()      &&
         AxisCount()     == rhs->AxisCount();
}

void CJoystick::SetName(const std::string& strName)
{
  std::string strSanitizedFilename(strName);

  // Remove Bluetooth MAC address as seen in Sony Playstation controllers
  StringUtils::RemoveMACAddress(strSanitizedFilename);

  StringUtils::Trim(strSanitizedFilename);

  ADDON::Joystick::SetName(strSanitizedFilename);
}

bool CJoystick::Initialize(void)
{
  if (ButtonCount() == 0 && HatCount() == 0 && AxisCount() == 0)
  {
    esyslog("Failed to initialize %s joystick: no buttons, hats or axes", Provider().c_str());
    return false;
  }

  m_state.buttons.assign(ButtonCount(), JOYSTICK_STATE_BUTTON_UNPRESSED);
  m_state.hats.assign(HatCount(), JOYSTICK_STATE_HAT_UNPRESSED);
  m_state.axes.assign(AxisCount(), 0.0f);

  m_stateBuffer.buttons.assign(ButtonCount(), JOYSTICK_STATE_BUTTON_UNPRESSED);
  m_stateBuffer.hats.assign(HatCount(), JOYSTICK_STATE_HAT_UNPRESSED);
  m_stateBuffer.axes.assign(AxisCount(), 0.0f);

  // Filter for anomalous triggers
  for (unsigned int i = 0; i < AxisCount(); i++)
    m_axisFilters.push_back(new CAnomalousTriggerFilter(i));

  return true;
}

void CJoystick::Deinitialize(void)
{
  m_state.buttons.clear();
  m_state.hats.clear();
  m_state.axes.clear();

  m_stateBuffer.buttons.clear();
  m_stateBuffer.hats.clear();
  m_stateBuffer.axes.clear();

  for (std::vector<IJoystickAxisFilter*>::iterator it = m_axisFilters.begin(); it != m_axisFilters.end(); ++it)
    delete *it;
  m_axisFilters.clear();
}

bool CJoystick::GetEvents(std::vector<ADDON::PeripheralEvent>& events)
{
  if (ScanEvents())
  {
    GetButtonEvents(events);
    GetHatEvents(events);
    GetAxisEvents(events);

    UpdateTimers();

    return true;
  }

  return false;
}

void CJoystick::GetButtonEvents(std::vector<ADDON::PeripheralEvent>& events)
{
  const std::vector<JOYSTICK_STATE_BUTTON>& buttons = m_stateBuffer.buttons;

  for (unsigned int i = 0; i < buttons.size(); i++)
  {
    if (buttons[i] != m_state.buttons[i])
      events.push_back(ADDON::PeripheralEvent(Index(), i, buttons[i]));
  }

  m_state.buttons.assign(buttons.begin(), buttons.end());
}

void CJoystick::GetHatEvents(std::vector<ADDON::PeripheralEvent>& events)
{
  const std::vector<JOYSTICK_STATE_HAT>& hats = m_stateBuffer.hats;

  for (unsigned int i = 0; i < hats.size(); i++)
  {
    if (hats[i] != m_state.hats[i])
      events.push_back(ADDON::PeripheralEvent(Index(), i, hats[i]));
  }

  m_state.hats.assign(hats.begin(), hats.end());
}

void CJoystick::GetAxisEvents(std::vector<ADDON::PeripheralEvent>& events)
{
  const std::vector<JOYSTICK_STATE_AXIS>& axes = m_stateBuffer.axes;

  for (unsigned int i = 0; i < axes.size(); i++)
  {
    if (axes[i] != 0.0f || m_state.axes[i] != 0.0f)
      events.push_back(ADDON::PeripheralEvent(Index(), i, axes[i]));
  }

  m_state.axes.assign(axes.begin(), axes.end());
}

void CJoystick::SetButtonValue(unsigned int buttonIndex, JOYSTICK_STATE_BUTTON buttonValue)
{
  if (m_activateTimeMs < 0)
    m_activateTimeMs = PLATFORM::GetTimeMs();

  if (buttonIndex < ButtonCount())
    m_stateBuffer.buttons[buttonIndex] = buttonValue;
}

void CJoystick::SetHatValue(unsigned int hatIndex, JOYSTICK_STATE_HAT hatValue)
{
  if (m_activateTimeMs < 0)
    m_activateTimeMs = PLATFORM::GetTimeMs();

  if (hatIndex < HatCount())
    m_stateBuffer.hats[hatIndex] = hatValue;
}

void CJoystick::SetAxisValue(unsigned int axisIndex, JOYSTICK_STATE_AXIS axisValue)
{
  if (m_activateTimeMs < 0)
    m_activateTimeMs = PLATFORM::GetTimeMs();

  axisValue = CONSTRAIN(-1.0f, axisValue, 1.0f);

  if (axisIndex < AxisCount())
    m_stateBuffer.axes[axisIndex] = ScaleDeadzone(m_axisFilters[axisIndex]->Filter(axisValue));
}

void CJoystick::SetAxisValue(unsigned int axisIndex, long value, long maxAxisAmount)
{
  SetAxisValue(axisIndex, (float)value / (float)maxAxisAmount);
}

void CJoystick::UpdateTimers(void)
{
  if (m_firstEventTimeMs < 0)
    m_firstEventTimeMs = PLATFORM::GetTimeMs();
  m_lastEventTimeMs = PLATFORM::GetTimeMs();
}

float CJoystick::NormalizeAxis(long value, long maxAxisAmount)
{
  return 1.0f * CONSTRAIN(-maxAxisAmount, value, maxAxisAmount) / maxAxisAmount;
}

float CJoystick::ScaleDeadzone(float value)
{
  const float deadzone = CSettings::Get().Deadzone();

  if (value > deadzone)
    return (float)(value - deadzone) / (float)(1.0f - deadzone);
  else if (value < -deadzone)
    return (float)(value + deadzone) / (float)(1.0f - deadzone);

  return 0.0f;
}
