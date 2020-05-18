/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "JoystickInterfaceCocoa.h"
#include "api/Joystick.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDBase.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <mutex>

namespace JOYSTICK
{
  class CJoystickInterfaceCocoa;

  class CJoystickCocoa : public CJoystick, public ICocoaInputCallback
  {
  public:
    CJoystickCocoa(IOHIDDeviceRef device, CJoystickInterfaceCocoa* api);
    virtual ~CJoystickCocoa(void);

    // implementation of CJoystick
    virtual bool Equals(const CJoystick* rhs) const override;
    virtual bool Initialize(void) override;
    virtual void Deinitialize(void) override;
    virtual bool GetEvents(std::vector<kodi::addon::PeripheralEvent>& events) override;

    // implementation of ICocoaInputCallback
    virtual void InputValueChanged(IOHIDValueRef value) override;

  protected:
    // implementation of CJoystick
    virtual bool ScanEvents(void) override;
    virtual void SetButtonValue(unsigned int buttonIndex, JOYSTICK_STATE_BUTTON buttonValue) override;
    virtual void SetHatValue(unsigned int hatIndex, JOYSTICK_STATE_HAT hatValue) override;
    virtual void SetAxisValue(unsigned int axisIndex, JOYSTICK_STATE_AXIS axisValue) override;

  private:
    IOHIDDeviceRef m_device;
    bool           m_bInitialized;

    struct CocoaAxis
    {
      IOHIDElementRef element;
      CFIndex         min;
      CFIndex         max;
    };

    CJoystickInterfaceCocoa* const m_api;
    std::vector<IOHIDElementRef> m_buttons;
    std::vector<CocoaAxis>       m_axes;
    std::recursive_mutex m_mutex;
  };
}
