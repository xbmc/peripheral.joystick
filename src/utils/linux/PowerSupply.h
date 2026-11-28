/*
 *  Copyright (C) 2026 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/Peripheral.h>

#include <string>

namespace JOYSTICK
{
  class CPowerSupply
  {
  public:
    /*!
     * \brief Read the battery or wired state the kernel reports for an input device
     *
     * \param sysfsPath The input device's directory in sysfs, such as
     *                  /sys/class/input/event5
     * \param battery   The battery state and charge, if known
     *
     * \return True if battery information or a wired connection is available
     */
    static bool GetBattery(const std::string& sysfsPath, JOYSTICK_BATTERY& battery);

  private:
    static bool ReadBattery(const std::string& powerSupplyPath, JOYSTICK_BATTERY& battery);
    static std::string ReadAttribute(const std::string& path);
  };
}
