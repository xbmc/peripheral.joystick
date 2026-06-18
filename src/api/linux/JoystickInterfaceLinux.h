/*
 *  Copyright (C) 2014-2021 Garrett Brown
 *  Copyright (C) 2014-2021 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "api/IJoystickInterface.h"

#include <atomic>
#include <stdint.h>
#include <string>
#include <thread>

namespace JOYSTICK
{
  class CJoystickInterfaceLinux : public IJoystickInterface
  {
  public:
    CJoystickInterfaceLinux(void) = default;
    virtual ~CJoystickInterfaceLinux(void) { Deinitialize(); }

    // implementation of IJoystickInterface
    virtual EJoystickInterface Type(void) const override;
    virtual bool Initialize(void) override;
    virtual void Deinitialize(void) override;
    virtual bool ScanForJoysticks(JoystickVector& joysticks) override;

  private:
    /*!
     * \brief Hotplug thread that watches /dev/input with inotify
     */
    void MonitorThreadProcess();

    int m_inotifyFd{-1};
    int m_stopFd{-1}; //!< eventfd used to wake the monitor thread on shutdown
    std::thread m_monitorThread;
    std::atomic<bool> m_bStop{false};
  };
}
