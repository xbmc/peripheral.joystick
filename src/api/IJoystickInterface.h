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

#include <string>

namespace JOYSTICK
{
  class CJoystick;

  class IJoystickInterface
  {
  public:
    virtual ~IJoystickInterface(void) { }

    /*!
     * \brief Get a short name for the interface
     */
    virtual EJoystickInterface Type(void) const = 0;

    /*!
     * \brief Convenience function to translate interface type to provider string
     */
    std::string Provider(void) const;

    /*!
     * \brief Initialize the interface
     */
    virtual bool Initialize(void) { return true; }

    /*!
     * \brief Deinitialize the interface
     */
    virtual void Deinitialize(void) { }

    /*!
     * \brief Check if rumble is supported by this interface
     */
    virtual bool SupportsRumble(void) const { return false; }

    /*!
     * \brief Check if controller power-off is supported by this interface
     */
    virtual bool SupportsPowerOff(void) const { return false; }

    /*!
     * \brief Scan the interface for joysticks
     *
     * \param results (out) the discovered joysticks; must be deallocated
     *
     * \return true if the scan completed successfully, even if no results are found
     */
    virtual bool ScanForJoysticks(JoystickVector& joysticks) = 0;

    /*!
     * \brief Get the button map known to the interface
     *
     * \return A button map populated with hard-coded features for the interface
     */
    virtual const ButtonMap& GetButtonMap()
    {
      static ButtonMap empty;
      return empty;
    }
  };
}
