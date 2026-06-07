/*
 *  Copyright (C) 2016-2021 Garrett Brown
 *  Copyright (C) 2016-2021 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

/*
 * This is a derivative work based on udev_joypad.c in the RetroArch project.
 */

/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2015 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2016 - Daniel De Matteis
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "api/IJoystickInterface.h"

#include <atomic>
#include <thread>

struct udev;
struct udev_device;
struct udev_monitor;

namespace JOYSTICK
{
  class CJoystickInterfaceUdev : public IJoystickInterface
  {
  public:
    CJoystickInterfaceUdev();
    virtual ~CJoystickInterfaceUdev() { Deinitialize(); }

    // implementation of IJoystickInterface
    virtual EJoystickInterface Type() const override;
    virtual bool Initialize() override;
    virtual void Deinitialize() override;
    virtual bool SupportsRumble(void) const { return true; }
    virtual bool ScanForJoysticks(JoystickVector& joysticks) override;
    virtual const ButtonMap& GetButtonMap() override;

  private:
    /*!
     * \brief Hotplug thread that watches the udev monitor
     */
    void MonitorThreadProcess();

    udev*         m_udev;
    udev_monitor* m_udev_mon;

    int m_stopFd{-1}; //!< eventfd used to wake the monitor thread on shutdown
    std::thread m_monitorThread;
    std::atomic<bool> m_bStop{false};

    static ButtonMap m_buttonMap;
  };
}
