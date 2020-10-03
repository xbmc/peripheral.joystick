/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Joystick.h"
#include "JoystickManager.h"
#include "JoystickTranslator.h"
#include "JoystickUtils.h"
#include "log/Log.h"
#include "settings/Settings.h"
#include "utils/CommonMacros.h"

#include <kodi/tools/StringUtils.h>

using namespace JOYSTICK;

#define ANALOG_EPSILON  0.0001f

CJoystick::CJoystick(EJoystickInterface interfaceType)
{
  SetProvider(JoystickTranslator::GetInterfaceProvider(interfaceType));
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
  std::string strSanitizedFilename = kodi::tools::StringUtils::MakeSafeString(strName);

  // Remove Bluetooth MAC address as seen in Sony Playstation controllers
  strSanitizedFilename = kodi::tools::StringUtils::RemoveMACAddress(strSanitizedFilename);

  kodi::addon::Joystick::SetName(strSanitizedFilename);
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
  m_state.axes.resize(AxisCount());

  m_stateBuffer.buttons.assign(ButtonCount(), JOYSTICK_STATE_BUTTON_UNPRESSED);
  m_stateBuffer.hats.assign(HatCount(), JOYSTICK_STATE_HAT_UNPRESSED);
  m_stateBuffer.axes.resize(AxisCount());

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
}

bool CJoystick::GetEvents(std::vector<kodi::addon::PeripheralEvent>& events)
{
  if (ScanEvents())
  {
    GetButtonEvents(events);
    GetHatEvents(events);
    GetAxisEvents(events);

    return true;
  }

  return false;
}

bool CJoystick::SendEvent(const kodi::addon::PeripheralEvent& event)
{
  bool bHandled = false;

  switch (event.Type())
  {
    case PERIPHERAL_EVENT_TYPE_SET_MOTOR:
    {
      bHandled = SetMotor(event.DriverIndex(), event.MotorState());
      break;
    }
    default:
      break;
  }

  return bHandled;
}

void CJoystick::Activate()
{
  if (!IsActive())
  {
    m_isActive = true;

    if (CJoystickUtils::IsGhostJoystick(*this))
    {
      CJoystickManager::Get().SetChanged(true);
      CJoystickManager::Get().TriggerScan();
    }
  }
}

void CJoystick::GetButtonEvents(std::vector<kodi::addon::PeripheralEvent>& events)
{
  const std::vector<JOYSTICK_STATE_BUTTON>& buttons = m_stateBuffer.buttons;

  for (unsigned int i = 0; i < buttons.size(); i++)
  {
    if (buttons[i] != m_state.buttons[i])
      events.push_back(kodi::addon::PeripheralEvent(Index(), i, buttons[i]));
  }

  m_state.buttons.assign(buttons.begin(), buttons.end());
}

void CJoystick::GetHatEvents(std::vector<kodi::addon::PeripheralEvent>& events)
{
  const std::vector<JOYSTICK_STATE_HAT>& hats = m_stateBuffer.hats;

  for (unsigned int i = 0; i < hats.size(); i++)
  {
    if (hats[i] != m_state.hats[i])
      events.push_back(kodi::addon::PeripheralEvent(Index(), i, hats[i]));
  }

  m_state.hats.assign(hats.begin(), hats.end());
}

void CJoystick::GetAxisEvents(std::vector<kodi::addon::PeripheralEvent>& events)
{
  const std::vector<JoystickAxis>& axes = m_stateBuffer.axes;

  for (unsigned int i = 0; i < axes.size(); i++)
  {
    if (axes[i].bSeen)
      events.push_back(kodi::addon::PeripheralEvent(Index(), i, axes[i].state));
  }

  m_state.axes.assign(axes.begin(), axes.end());
}

void CJoystick::SetButtonValue(unsigned int buttonIndex, JOYSTICK_STATE_BUTTON buttonValue)
{
  Activate();

  if (buttonIndex < m_stateBuffer.buttons.size())
    m_stateBuffer.buttons[buttonIndex] = buttonValue;
}

void CJoystick::SetHatValue(unsigned int hatIndex, JOYSTICK_STATE_HAT hatValue)
{
  Activate();

  if (hatIndex < m_stateBuffer.hats.size())
    m_stateBuffer.hats[hatIndex] = hatValue;
}

void CJoystick::SetAxisValue(unsigned int axisIndex, JOYSTICK_STATE_AXIS axisValue)
{
  Activate();

  axisValue = CONSTRAIN(-1.0f, axisValue, 1.0f);

  if (axisIndex < m_stateBuffer.axes.size())
  {
    m_stateBuffer.axes[axisIndex].state = axisValue;
    m_stateBuffer.axes[axisIndex].bSeen = true;
  }
}

void CJoystick::SetAxisValue(unsigned int axisIndex, long value, long maxAxisAmount)
{
  if (maxAxisAmount != 0)
    SetAxisValue(axisIndex, (float)value / (float)maxAxisAmount);
  else
    SetAxisValue(axisIndex, 0.0f);
}
