/*
 *      Copyright (C) 2014-2020 Garrett Brown
 *      Copyright (C) 2014-2020 Team Kodi
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

#include "JoystickInterfaceCocoa.h"
#include "JoystickCocoa.h"
#include "api/JoystickManager.h"
#include "api/JoystickTypes.h"

#include <algorithm>

using namespace JOYSTICK;

// --- MatchingDictionary ------------------------------------------------------

static CFMutableDictionaryRef MatchingDictionary(UInt32 inUsagePage, UInt32 inUsage)
{
  CFMutableDictionaryRef dict = CFDictionaryCreateMutable(kCFAllocatorDefault,
                                                          0,
                                                          &kCFTypeDictionaryKeyCallBacks,
                                                          &kCFTypeDictionaryValueCallBacks);
  if (!dict)
    return NULL;

  CFNumberRef number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &inUsagePage);
  if (!number)
  {
    CFRelease(dict);
    return NULL;
  }

  CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsagePageKey), number);
  CFRelease(number);

  number = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &inUsage);
  if (!number)
  {
    CFRelease(dict);
    return NULL;
  }

  CFDictionarySetValue(dict, CFSTR(kIOHIDDeviceUsageKey), number);
  CFRelease(number);

  return dict;
}

// --- CJoystickInterfaceCocoa -------------------------------------------------

CJoystickInterfaceCocoa::CJoystickInterfaceCocoa(void)
  : m_manager(NULL)
{
}

EJoystickInterface CJoystickInterfaceCocoa::Type(void) const
{
  return EJoystickInterface::COCOA;
}

bool CJoystickInterfaceCocoa::Initialize(void)
{
  Deinitialize();

  IOHIDManagerRef manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

  CFMutableDictionaryRef criteria_arr[2];
  criteria_arr[0] = MatchingDictionary(GENERIC_DESKTOP_USAGE_PAGE, JOYSTICK_USAGE_NUMBER);
  if (!criteria_arr[0])
  {
    CFRelease(manager);
    return false;
  }

  criteria_arr[1] = MatchingDictionary(GENERIC_DESKTOP_USAGE_PAGE, GAMEPAD_USAGE_NUMBER);
  if (!criteria_arr[1])
  {
    CFRelease(criteria_arr[0]);
    CFRelease(manager);
    return false;
  }

  CFArrayRef criteria = CFArrayCreate(kCFAllocatorDefault, (const void**)criteria_arr, 2, NULL);
  if (!criteria)
  {
    CFRelease(criteria_arr[1]);
    CFRelease(criteria_arr[0]);
    CFRelease(manager);
    return false;
  }

  IOHIDManagerSetDeviceMatchingMultiple(manager, criteria);
  CFRelease(criteria);
  CFRelease(criteria_arr[1]);
  CFRelease(criteria_arr[0]);

  IOHIDManagerRegisterDeviceMatchingCallback(manager, DeviceAddedCallback, this);
  IOHIDManagerRegisterDeviceRemovalCallback(manager, DeviceRemovedCallback, this);
  IOHIDManagerRegisterInputValueCallback(manager, InputValueChangedCallback, this);

  IOHIDManagerScheduleWithRunLoop(manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

  IOReturn rv = IOHIDManagerOpen(manager, kIOHIDOptionsTypeNone);
  if (rv != kIOReturnSuccess)
  {
    CFRelease(manager);
    return false;
  }

  m_manager = manager;

  return true;
}

void CJoystickInterfaceCocoa::Deinitialize(void)
{
  if (m_manager)
  {
    IOHIDManagerClose(m_manager, 0);
    CFRelease(m_manager);
    m_manager = NULL;
  }
}

bool CJoystickInterfaceCocoa::ScanForJoysticks(JoystickVector& joysticks)
{
  std::lock_guard<std::recursive_mutex> lock(m_deviceDiscoveryMutex);

  for (auto it = m_discoveredDevices.begin(); it != m_discoveredDevices.end(); ++it)
    joysticks.push_back(JoystickPtr(new CJoystickCocoa(*it, this)));

  return true;
}

void CJoystickInterfaceCocoa::DeviceAdded(IOHIDDeviceRef device)
{
  bool bDeviceAdded = false;

  {
    std::lock_guard<std::recursive_mutex> lock(m_deviceDiscoveryMutex);

    if (std::find(m_discoveredDevices.begin(), m_discoveredDevices.end(), device) == m_discoveredDevices.end())
    {
      m_discoveredDevices.push_back(device);
      bDeviceAdded = true;
    }
  }

  if (bDeviceAdded)
  {
    CJoystickManager::Get().SetChanged(true);
    CJoystickManager::Get().TriggerScan();
  }
}

void CJoystickInterfaceCocoa::DeviceRemoved(IOHIDDeviceRef device)
{
  {
    std::lock_guard<std::recursive_mutex> lock(m_deviceDiscoveryMutex);
    m_discoveredDevices.erase(std::remove(m_discoveredDevices.begin(), m_discoveredDevices.end(), device), m_discoveredDevices.end());
  }

  CJoystickManager::Get().SetChanged(true);
  CJoystickManager::Get().TriggerScan();
}

void CJoystickInterfaceCocoa::InputValueChanged(IOHIDValueRef newValue)
{
  IOHIDElementRef element = IOHIDValueGetElement(newValue);
  IOHIDDeviceRef device = IOHIDElementGetDevice(element);

  std::lock_guard<std::recursive_mutex> lock(m_deviceInputMutex);

  for (std::vector<DeviceHandle>::iterator it = m_registeredDevices.begin(); it != m_registeredDevices.end(); ++it)
  {
    if (it->first == device)
      it->second->InputValueChanged(newValue);
  }
}

void CJoystickInterfaceCocoa::RegisterInputCallback(ICocoaInputCallback* callback, IOHIDDeviceRef device)
{
  std::lock_guard<std::recursive_mutex> lock(m_deviceInputMutex);

  m_registeredDevices.push_back(std::make_pair(device, callback));
}

void CJoystickInterfaceCocoa::UnregisterInputCallback(ICocoaInputCallback* callback)
{
  std::lock_guard<std::recursive_mutex> lock(m_deviceInputMutex);

  for (std::vector<DeviceHandle>::iterator it = m_registeredDevices.begin(); it != m_registeredDevices.end(); )
  {
    if (it->second == callback)
      it = m_registeredDevices.erase(it);
    else
      ++it;
  }
}

void CJoystickInterfaceCocoa::DeviceAddedCallback(void* data, IOReturn result,
                                                  void* sender, IOHIDDeviceRef device)
{
  CJoystickInterfaceCocoa* interface = static_cast<CJoystickInterfaceCocoa*>(data);
  interface->DeviceAdded(device);
}

void CJoystickInterfaceCocoa::DeviceRemovedCallback(void* data, IOReturn result,
                                                    void* sender, IOHIDDeviceRef device)
{
  CJoystickInterfaceCocoa* interface = static_cast<CJoystickInterfaceCocoa*>(data);
  interface->DeviceRemoved(device);
}

void CJoystickInterfaceCocoa::InputValueChangedCallback(void* data,
                                                        IOReturn result,
                                                        void* sender,
                                                        IOHIDValueRef newValue)
{
  CJoystickInterfaceCocoa* interface = static_cast<CJoystickInterfaceCocoa*>(data);
  interface->InputValueChanged(newValue);
}
