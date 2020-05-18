/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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

    std::recursive_mutex m_deviceDiscoveryMutex;
    std::recursive_mutex m_deviceInputMutex;
  };
}
