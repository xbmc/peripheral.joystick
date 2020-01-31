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
