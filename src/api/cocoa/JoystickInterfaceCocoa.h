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

#include "api/IJoystickInterface.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDBase.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <mutex>
#include <utility>
#include <vector>

// These values can be found in the USB HID Usage Tables:
// http://www.usb.org/developers/hidpage
#define GENERIC_DESKTOP_USAGE_PAGE  0x01
#define JOYSTICK_USAGE_NUMBER       0x04
#define GAMEPAD_USAGE_NUMBER        0x05
#define AXIS_MIN_USAGE_NUMBER       0x30
#define AXIS_MAX_USAGE_NUMBER       0x35
#define BUTTON_USAGE_PAGE           0x09

namespace JOYSTICK
{
  class ICocoaInputCallback
  {
  public:
    virtual ~ICocoaInputCallback(void) { }

    virtual void InputValueChanged(IOHIDValueRef value) = 0;
  };

  class CJoystickInterfaceCocoa : public IJoystickInterface
  {
  public:
    CJoystickInterfaceCocoa(void);
    virtual ~CJoystickInterfaceCocoa(void) { Deinitialize(); }

    // implementation of IJoystickInterface
    virtual EJoystickInterface Type(void) const override;
    virtual bool Initialize(void) override;
    virtual void Deinitialize(void) override;
    virtual bool ScanForJoysticks(JoystickVector& joysticks) override;

    void RegisterInputCallback(ICocoaInputCallback* callback, IOHIDDeviceRef device);
    void UnregisterInputCallback(ICocoaInputCallback* callback);

    void DeviceAdded(IOHIDDeviceRef device);
    void DeviceRemoved(IOHIDDeviceRef device);
    void InputValueChanged(IOHIDValueRef value);

    static void DeviceAddedCallback(void* data, IOReturn result,
                                    void* sender, IOHIDDeviceRef device);
    static void DeviceRemovedCallback(void* data, IOReturn result,
                                      void* sender, IOHIDDeviceRef device);
    static void InputValueChangedCallback(void* data, IOReturn result,
                                          void* sender, IOHIDValueRef newValue);

  private:
    IOHIDManagerRef m_manager;

    typedef std::pair<IOHIDDeviceRef, ICocoaInputCallback*> DeviceHandle;

    std::vector<IOHIDDeviceRef> m_discoveredDevices;
    std::vector<DeviceHandle>   m_registeredDevices;

    std::mutex m_deviceDiscoveryMutex;
    std::mutex m_deviceInputMutex;
  };
}
