/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "JoystickTypes.h"
#include "buttonmapper/ButtonMapTypes.h"

#include <kodi/addon-instance/PeripheralUtils.h>

#include <mutex>
#include <set>
#include <vector>

namespace JOYSTICK
{
  class IJoystickInterface;

  class IScannerCallback
  {
  public:
    virtual ~IScannerCallback(void) { }

    /*!
     * \brief Trigger a scan for joysticks
     */
    virtual void TriggerScan(void) = 0;
  };

  class CJoystickManager
  {
  private:
    CJoystickManager(void);

  public:
    static CJoystickManager& Get(void);
    virtual ~CJoystickManager(void) { Deinitialize(); }

    static const std::vector<EJoystickInterface>& GetSupportedInterfaces();

    static IJoystickInterface* CreateInterface(EJoystickInterface iface);

    /*!
     * \brief Initialize the joystick manager
     *
     * \param scanner The callback used to trigger a scan
     */
    bool Initialize(IScannerCallback* scanner);

    /*!
     * \brief Deinitialize the joystick manager
     */
    void Deinitialize(void);

    /*!
     * \brief Return true if an interface supports rumble
     */
    bool SupportsRumble(void) const;

    /*!
     * \brief Return true if an interface supports controller power-off
     */
    bool SupportsPowerOff(void) const;

    /*!
     * \brief Check if the given interface is managed by this system
     *
     * \param iface The interface type
     *
     * \return true if the interface is present
     */
    bool HasInterface(EJoystickInterface iface) const;

    /*!
     * \brief Set the state of the specified interface
     *
     * \param iface The interface type
     * \param bEnabled True to enable interface, false to disable interface
     */
    void SetEnabled(EJoystickInterface iface, bool bEnabled);

    /*!
     * \brief Check the state of the specified interface
     *
     * \param iface The interface to check
     *
     * \return true if the interface is present and enabled, false otherwise
     */
    bool IsEnabled(IJoystickInterface* iface);

    /*!
     * \brief Scan the available interfaces for joysticks
     *
     * \param joysticks The discovered joysticks; must be deallocated
     *
     * \return true if the scan succeeded (even if there are no joysticks)
     */
    bool PerformJoystickScan(JoystickVector& joysticks);

    JoystickPtr GetJoystick(unsigned int index) const;

    JoystickVector GetJoysticks(const kodi::addon::Joystick& joystickInfo) const;

    /*!
    * \brief Get all events that have occurred since the last call to GetEvents()
    */
    bool GetEvents(std::vector<kodi::addon::PeripheralEvent>& events);

    /*!
     * \brief Send an event to a joystick
     *
     * \param event The event
     *
     * \return True if the event was handled
     */
    bool SendEvent(const kodi::addon::PeripheralEvent& event);

    /*!
     * \brief Process events that have arrived since the last call to ProcessEvents()
     */
    void ProcessEvents();

    /*!
     * \brief Set the flag for changed interfaces
     */
    void SetChanged(bool bChanged);

    /*!
     * \brief Trigger a scan for joysticks through the callback, if changed
     */
    void TriggerScan(void);

    /*!
     * \brief Get the button map known to the interface
     *
     * \param provider Name of the joystick interface
     *
     * \return A button map populated with hard-coded features for the interface
     */
    const ButtonMap& GetButtonMap(const std::string& provider);

  private:
    IScannerCallback*                m_scanner;
    std::vector<IJoystickInterface*> m_interfaces;
    std::set<IJoystickInterface*>    m_enabledInterfaces;
    JoystickVector                   m_joysticks;
    unsigned int                     m_nextJoystickIndex;
    bool                             m_bChanged;
    mutable std::recursive_mutex m_changedMutex;
    mutable std::recursive_mutex m_interfacesMutex;
    mutable std::recursive_mutex m_joystickMutex;
  };
}
