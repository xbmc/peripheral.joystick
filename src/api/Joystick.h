/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "JoystickTypes.h"

#include <kodi/addon-instance/PeripheralUtils.h>

#include <string>
#include <vector>

namespace JOYSTICK
{
  class CJoystick : public kodi::addon::Joystick
  {
  public:
    CJoystick(EJoystickInterface interfaceType);

    virtual ~CJoystick(void) { Deinitialize(); }

    /*!
     * Compare joystick properties
     */
    virtual bool Equals(const CJoystick* rhs) const;

    /*!
     * Override subclass to sanitize name (strip trailing whitespace, etc)
     */
    void SetName(const std::string& strName);

    /*!
     * Check if this joystick has received any input
     */
    bool IsActive(void) const { return m_isActive; }

    /*!
     * Initialize the joystick object. Joystick will be initialized before the
     * first call to GetEvents().
     */
    virtual bool Initialize(void);

    /*!
     * Deinitialize the joystick object. GetEvents() will not be called after
     * deinitialization.
     */
    virtual void Deinitialize(void);

    /*!
     * Get events that have occurred since the last call to GetEvents()
     */
    virtual bool GetEvents(std::vector<kodi::addon::PeripheralEvent>& events);

    /*!
     * Send an event to a joystick
     */
    virtual bool SendEvent(const kodi::addon::PeripheralEvent& event);

    /*!
     * Process events sent to the joystick
     */
    virtual void ProcessEvents() { }

    /*!
     * Tries to power off the joystick.
     */
    virtual void PowerOff() { }

  protected:
    /*!
     * Implemented by derived class to scan for events
     */
    virtual bool ScanEvents(void) = 0;

    virtual bool SetMotor(unsigned int motorIndex, float magnitude) { return false; }

    virtual void SetButtonValue(unsigned int buttonIndex, JOYSTICK_STATE_BUTTON buttonValue);
    virtual void SetHatValue(unsigned int hatIndex, JOYSTICK_STATE_HAT hatValue);
    virtual void SetAxisValue(unsigned int axisIndex, JOYSTICK_STATE_AXIS axisValue);
    void SetAxisValue(unsigned int axisIndex, long value, long maxAxisAmount);

  private:
    void Activate();

    void GetButtonEvents(std::vector<kodi::addon::PeripheralEvent>& events);
    void GetHatEvents(std::vector<kodi::addon::PeripheralEvent>& events);
    void GetAxisEvents(std::vector<kodi::addon::PeripheralEvent>& events);

    struct JoystickAxis
    {
      JOYSTICK_STATE_AXIS state = 0.0f;
      bool bSeen = false;
    };

    struct JoystickState
    {
      std::vector<JOYSTICK_STATE_BUTTON> buttons;
      std::vector<JOYSTICK_STATE_HAT>    hats;
      std::vector<JoystickAxis>          axes;
    };

    JoystickState                     m_state;
    JoystickState                     m_stateBuffer;
    bool m_isActive = false;
  };
}
