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
