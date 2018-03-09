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

#include "JoystickManager.h"
#include "IJoystickInterface.h"
#include "Joystick.h"
#include "JoystickTranslator.h"
#include "JoystickUtils.h"

#if defined(HAVE_DIRECT_INPUT)
  #include "directinput/JoystickInterfaceDirectInput.h"
#endif
#if defined(HAVE_XINPUT)
  #include "xinput/JoystickInterfaceXInput.h"
#endif
#if defined(HAVE_SDL_GAMEPAD)
  #include "sdl/JoystickInterfaceSDL.h"
#endif
#if defined(HAVE_LINUX_JOYSTICK)
  #include "linux/JoystickInterfaceLinux.h"
#endif
#if defined(HAVE_COCOA)
  #include "cocoa/JoystickInterfaceCocoa.h"
#endif
#if defined(HAVE_UDEV)
  #include "udev/JoystickInterfaceUdev.h"
#endif

#include "log/Log.h"
#include "settings/Settings.h"
#include "utils/CommonMacros.h"

#include <algorithm>
#include <iterator>

using namespace JOYSTICK;
using namespace P8PLATFORM;

// --- Utility functions -------------------------------------------------------

namespace JOYSTICK
{
  struct ScanResultEqual
  {
    ScanResultEqual(const JoystickPtr& needle) : m_needle(needle) { }

    bool operator()(const JoystickPtr& rhs)
    {
      if (m_needle)
        return m_needle->Equals(rhs.get());

      return m_needle == rhs;
    }

  private:
    JoystickPtr const m_needle;
  };

  template <class T>
  void safe_delete(T*& pVal)
  {
      delete pVal;
      pVal = NULL;
  }

  template <class T>
  void safe_delete_vector(std::vector<T*>& vec)
  {
    for (typename std::vector<T*>::iterator it = vec.begin(); it != vec.end(); ++it)
      delete *it;
    vec.clear();
  }
}

// --- CJoystickManager --------------------------------------------------------

CJoystickManager::CJoystickManager(void)
  : m_scanner(NULL),
    m_nextJoystickIndex(0),
    m_bChanged(false)
{
}

CJoystickManager& CJoystickManager::Get(void)
{
  static CJoystickManager _instance;
  return _instance;
}

const std::vector<EJoystickInterface>& CJoystickManager::GetSupportedInterfaces()
{
  static std::vector<EJoystickInterface> supportedInterfaces;

  // Supported interfaces in order of priority
  if (supportedInterfaces.empty())
  {
#if defined(HAVE_DIRECT_INPUT)
    supportedInterfaces.push_back(EJoystickInterface::DIRECTINPUT);
#endif
#if defined(HAVE_XINPUT)
    supportedInterfaces.push_back(EJoystickInterface::XINPUT);
#endif

  // Linux
#if defined(HAVE_SDL_GAMEPAD)
    supportedInterfaces.push_back(EJoystickInterface::SDL);
#endif
#if defined(HAVE_LINUX_JOYSTICK)
    supportedInterfaces.push_back(EJoystickInterface::LINUX);
#endif
#if defined(HAVE_UDEV)
    supportedInterfaces.push_back(EJoystickInterface::UDEV);
#endif

  // OSX
#if defined(HAVE_COCOA)
    supportedInterfaces.push_back(EJoystickInterface::COCOA);
#endif
  }

  return supportedInterfaces;
}

IJoystickInterface* CJoystickManager::CreateInterface(EJoystickInterface iface)
{
  switch (iface)
  {
#if defined(HAVE_COCOA)
  case EJoystickInterface::COCOA: return new CJoystickInterfaceCocoa;
#endif
#if defined(HAVE_DIRECT_INPUT)
  case EJoystickInterface::DIRECTINPUT: return new CJoystickInterfaceDirectInput;
#endif
#if defined(HAVE_LINUX_JOYSTICK)
  case EJoystickInterface::LINUX: return new CJoystickInterfaceLinux;
#endif
#if defined(HAVE_SDL_GAMEPAD)
  case EJoystickInterface::SDL: return new CJoystickInterfaceSDL;
#endif
#if defined(HAVE_UDEV)
  case EJoystickInterface::UDEV: return new CJoystickInterfaceUdev;
#endif
#if defined(HAVE_XINPUT)
  case EJoystickInterface::XINPUT: return new CJoystickInterfaceXInput;
#endif
  default:
    break;
  }

  return nullptr;
}

bool CJoystickManager::Initialize(IScannerCallback* scanner)
{
  CLockObject lock(m_interfacesMutex);

  m_scanner = scanner;

  const std::vector<EJoystickInterface>& interfaces = GetSupportedInterfaces();

  for (auto interfaceType : interfaces)
  {
    auto pInterface = CreateInterface(interfaceType);
    if (pInterface)
      m_interfaces.push_back(pInterface);
  }

  if (m_interfaces.empty())
    dsyslog("No joystick APIs in use");

  return true;
}

void CJoystickManager::Deinitialize(void)
{
  {
    CLockObject lock(m_joystickMutex);
    m_joysticks.clear();
  }

  {
    CLockObject lock(m_interfacesMutex);
    for (auto pInterface : m_interfaces)
      SetEnabled(pInterface->Type(), false);
    safe_delete_vector(m_interfaces);
  }

  m_scanner = NULL;
}

bool CJoystickManager::SupportsRumble(void) const
{
  CLockObject lock(m_interfacesMutex);
  for (auto pInterface : m_enabledInterfaces)
  {
    if (pInterface->SupportsRumble())
      return true;
  }

  return false;
}

bool CJoystickManager::SupportsPowerOff(void) const
{
  CLockObject lock(m_interfacesMutex);
  for (auto pInterface : m_enabledInterfaces)
  {
    if (pInterface->SupportsPowerOff())
      return true;
  }

  return false;
}

bool CJoystickManager::HasInterface(EJoystickInterface iface) const
{
  CLockObject lock(m_interfacesMutex);
  for (auto pInterface : m_interfaces)
  {
    if (pInterface->Type() == iface)
      return true;
  }

  return false;
}

void CJoystickManager::SetEnabled(EJoystickInterface iface, bool bEnabled)
{
  CLockObject lock(m_interfacesMutex);

  for (auto pInterface : m_interfaces)
  {
    if (pInterface->Type() == iface)
    {
      if (bEnabled && !IsEnabled(pInterface))
      {
        isyslog("Enabling joystick interface \"%s\"", JoystickTranslator::GetInterfaceProvider(iface).c_str());
        if (pInterface->Initialize())
        {
          m_enabledInterfaces.insert(pInterface);
          SetChanged(true);
        }
        else
          esyslog("Failed to initialize interface %s", JoystickTranslator::GetInterfaceProvider(iface).c_str());
      }
      else if (!bEnabled && IsEnabled(pInterface))
      {
        isyslog("Disabling joystick interface \"%s\"", JoystickTranslator::GetInterfaceProvider(iface).c_str());
        pInterface->Deinitialize();
        m_enabledInterfaces.erase(pInterface);
        SetChanged(true);
      }
      break;
    }
  }
}

bool CJoystickManager::IsEnabled(IJoystickInterface* iface)
{
  CLockObject lock(m_interfacesMutex);
  return m_enabledInterfaces.find(iface) != m_enabledInterfaces.end();
}

bool CJoystickManager::PerformJoystickScan(JoystickVector& joysticks)
{
  JoystickVector scanResults;
  {
    CLockObject lock(m_interfacesMutex);
    // Scan for joysticks (this can take a while, don't block)
    for (auto pInterface : m_enabledInterfaces)
      pInterface->ScanForJoysticks(scanResults);
  }

  CLockObject lock(m_joystickMutex);

  // Unregister removed joysticks
  for (int i = (int)m_joysticks.size() - 1; i >= 0; i--)
  {
    if (std::find_if(scanResults.begin(), scanResults.end(), ScanResultEqual(m_joysticks.at(i))) == scanResults.end())
      m_joysticks.erase(m_joysticks.begin() + i);
  }

  // Register new joysticks
  for (JoystickVector::iterator itJoystick = scanResults.begin(); itJoystick != scanResults.end(); ++itJoystick)
  {
    if (std::find_if(m_joysticks.begin(), m_joysticks.end(), ScanResultEqual(*itJoystick)) == m_joysticks.end())
    {
      if ((*itJoystick)->Initialize())
      {
        (*itJoystick)->SetIndex(m_nextJoystickIndex++);

        isyslog("Initialized joystick %u: \"%s\", axes: %u, hats: %u, buttons: %u",
                (*itJoystick)->Index(), (*itJoystick)->Name().c_str(),
                (*itJoystick)->AxisCount(), (*itJoystick)->HatCount(), (*itJoystick)->ButtonCount());

        m_joysticks.push_back(*itJoystick);
      }
    }
  }

  joysticks = m_joysticks;

  // Work around bug on linux: Don't return disconnected Xbox 360 controllers
  joysticks.erase(std::remove_if(joysticks.begin(), joysticks.end(),
    [](const JoystickPtr& joystick)
    {
      return CJoystickUtils::IsGhostJoystick(*joystick) &&
             !joystick->IsActive();
    }), joysticks.end());

  return true;
}

JoystickPtr CJoystickManager::GetJoystick(unsigned int index) const
{
  CLockObject lock(m_joystickMutex);

  for (JoystickVector::const_iterator it = m_joysticks.begin(); it != m_joysticks.end(); ++it)
  {
    if ((*it)->Index() == index)
      return *it;
  }

  return JoystickPtr();
}

JoystickVector CJoystickManager::GetJoysticks(const kodi::addon::Joystick& joystickInfo) const
{
  JoystickVector result;

  CLockObject lock(m_joystickMutex);

  for (const auto& joystick : m_joysticks)
  {
    if (joystick->Name() == joystickInfo.Name() &&
        joystick->Provider() == joystickInfo.Provider())
    {
      result.push_back(joystick);
    }
  }

  return result;
}

bool CJoystickManager::GetEvents(std::vector<kodi::addon::PeripheralEvent>& events)
{
  CLockObject lock(m_joystickMutex);

  for (JoystickVector::iterator it = m_joysticks.begin(); it != m_joysticks.end(); ++it)
    (*it)->GetEvents(events);

  return true;
}

bool CJoystickManager::SendEvent(const kodi::addon::PeripheralEvent& event)
{
  bool bHandled = false;

  CLockObject lock(m_joystickMutex);

  for (const JoystickPtr& joystick : m_joysticks)
  {
    if (joystick->Index() == event.PeripheralIndex())
    {
      bHandled = joystick->SendEvent(event);
      if (bHandled)
        break;
    }
  }

  return bHandled;
}

void CJoystickManager::ProcessEvents()
{
  CLockObject lock(m_joystickMutex);

  for (const JoystickPtr& joystick : m_joysticks)
    joystick->ProcessEvents();
}

void CJoystickManager::SetChanged(bool bChanged)
{
  CLockObject lock(m_changedMutex);
  m_bChanged = bChanged;
}

void CJoystickManager::TriggerScan(void)
{
  bool bChanged;
  {
    CLockObject lock(m_changedMutex);
    bChanged = m_bChanged;
    m_bChanged = false;
  }

  if (bChanged && m_scanner)
    m_scanner->TriggerScan();
}

const ButtonMap& CJoystickManager::GetButtonMap(const std::string& provider)
{
  static ButtonMap empty;

  CLockObject lock(m_interfacesMutex);

  for (auto pInterface : m_interfaces)
  {
    if (pInterface->Provider() == provider)
      return pInterface->GetButtonMap();
  }

  return empty;
}
