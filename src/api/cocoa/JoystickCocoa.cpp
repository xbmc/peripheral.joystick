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

#include "JoystickCocoa.h"
#include "api/JoystickTypes.h"
#include "utils/CommonMacros.h"

#include <assert.h>

using namespace JOYSTICK;

#define MAX_JOYSTICK_BUTTONS  512

CJoystickCocoa::CJoystickCocoa(IOHIDDeviceRef device, CJoystickInterfaceCocoa* api)
 : CJoystick(EJoystickInterface::COCOA),
   m_api(api),
   m_device(device),
   m_bInitialized(false)
{
  assert(m_api != nullptr);

  m_api->RegisterInputCallback(this, m_device);

  // Must initialize in the constructor to fill out joystick properties
  Initialize();
}

CJoystickCocoa::~CJoystickCocoa(void)
{
  m_api->UnregisterInputCallback(this);
}

bool CJoystickCocoa::Equals(const CJoystick* rhs) const
{
  const CJoystickCocoa* joystick = dynamic_cast<const CJoystickCocoa*>(rhs);

  return joystick && m_device == joystick->m_device;
}

bool CJoystickCocoa::Initialize(void)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  if (!m_bInitialized)
  {
    CFArrayRef elements = IOHIDDeviceCopyMatchingElements(m_device, NULL, kIOHIDOptionsTypeNone);

    CFIndex n = CFArrayGetCount(elements);
    for (CFIndex i = 0; i < n; i++)
    {
      IOHIDElementRef element = (IOHIDElementRef)CFArrayGetValueAtIndex(elements, i);
      uint32_t usagePage = IOHIDElementGetUsagePage(element);
      uint32_t usage = IOHIDElementGetUsage(element);

      if (usagePage == GENERIC_DESKTOP_USAGE_PAGE &&
          usage     >= AXIS_MIN_USAGE_NUMBER &&
          usage     <= AXIS_MAX_USAGE_NUMBER)
      {
        CFIndex min = IOHIDElementGetLogicalMin(element);
        CFIndex max = IOHIDElementGetLogicalMax(element);

        CocoaAxis axis = { element, min, max };
        m_axes.push_back(axis);
      }
      else if (usagePage == BUTTON_USAGE_PAGE)
      {
        int buttonId = (int)usage - 1;
        if (0 <= buttonId && buttonId < MAX_JOYSTICK_BUTTONS)
        {
          if (buttonId >= (int)m_buttons.size())
            m_buttons.resize(buttonId + 1);

          m_buttons[buttonId] = element;
        }
      }
      else
      {
        // TODO: handle other usage pages
      }
    }

    SetButtonCount(m_buttons.size());
    SetAxisCount(m_axes.size());

    // Gather some identifying information
    CFStringRef productRef = (CFStringRef)IOHIDDeviceGetProperty(m_device, CFSTR(kIOHIDProductKey));
    CFNumberRef vendorIdRef = (CFNumberRef)IOHIDDeviceGetProperty(m_device, CFSTR(kIOHIDVendorIDKey));
    CFNumberRef productIdRef = (CFNumberRef)IOHIDDeviceGetProperty(m_device, CFSTR(kIOHIDProductIDKey));

    if (productRef)
    {
      char product_name[128] = { };
      CFStringGetCString(productRef, product_name, sizeof(product_name), kCFStringEncodingASCII);
      SetName(product_name);
    }

    if (vendorIdRef && productIdRef)
    {
      int vendorId = 0;
      int productId = 0;

      CFNumberGetValue(vendorIdRef, kCFNumberIntType, &vendorId);
      CFNumberGetValue(productIdRef, kCFNumberIntType, &productId);

      SetVendorID(vendorId);
      SetProductID(productId);
    }

    if (CJoystick::Initialize())
      m_bInitialized = true;
  }

  return m_bInitialized;
}

void CJoystickCocoa::Deinitialize(void)
{
  std::lock_guard<std::mutex> lock(m_mutex);

  CJoystick::Deinitialize();

  m_buttons.clear();
  m_axes.clear();

  m_bInitialized = false;
}

bool CJoystickCocoa::GetEvents(std::vector<kodi::addon::PeripheralEvent>& events)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  return CJoystick::GetEvents(events);
}

bool CJoystickCocoa::ScanEvents(void)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_bInitialized; // Events arrive asynchronously
}

void CJoystickCocoa::SetButtonValue(unsigned int buttonIndex, JOYSTICK_STATE_BUTTON buttonValue)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  CJoystick::SetButtonValue(buttonIndex, buttonValue);
}

void CJoystickCocoa::SetHatValue(unsigned int hatIndex, JOYSTICK_STATE_HAT hatValue)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  CJoystick::SetHatValue(hatIndex, hatValue);
}

void CJoystickCocoa::SetAxisValue(unsigned int axisIndex, JOYSTICK_STATE_AXIS axisValue)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  CJoystick::SetAxisValue(axisIndex, axisValue);
}

void CJoystickCocoa::InputValueChanged(IOHIDValueRef value)
{
  IOHIDElementRef element = IOHIDValueGetElement(value);

  for (unsigned int i = 0; i < m_axes.size(); i++)
  {
    if (m_axes[i].element == element)
    {
      float d = IOHIDValueGetIntegerValue(value);
      float val = 2.0f * (d - m_axes[i].min) / (float)(m_axes[i].max - m_axes[i].min) - 1.0f;
      SetAxisValue(i, val);
      return;
    }
  }

  for (unsigned int i = 0; i < m_buttons.size(); i++)
  {
    if (m_buttons[i] == element)
    {
      bool bPressed = IOHIDValueGetIntegerValue(value) != 0;
      SetButtonValue(i, bPressed ? JOYSTICK_STATE_BUTTON_PRESSED : JOYSTICK_STATE_BUTTON_UNPRESSED);
      return;
    }
  }
}
