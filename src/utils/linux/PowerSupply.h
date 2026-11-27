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
     * \brief Read the battery the kernel reports for an input device
     *
     * \param sysfsPath The input device's directory in sysfs, such as
     *                  /sys/class/input/event5
     * \param battery   The battery, if the device has one
     *
     * \return True if the kernel reports a battery for the device
     */
    static bool GetBattery(const std::string& sysfsPath, JOYSTICK_BATTERY& battery);

  private:
    static bool ReadBattery(const std::string& powerSupplyPath, JOYSTICK_BATTERY& battery);
    static std::string ReadAttribute(const std::string& path);
  };
}
