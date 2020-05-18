/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <set>
#include <string>

namespace kodi
{
namespace addon
{
  struct DriverPrimitive;
  class Joystick;
}
}

namespace JOYSTICK
{
  class CStorageUtils
  {
  public:
    static bool EnsureDirectoryExists(const std::string& path);

    /*!
     * \brief Utility function: Build a filename out of the record's properties
     *
     * \return A sensible filename, lacking an extension (which can be added by
     *         the caller)
     *
     * The filename is derived from driver properties. An example joystick
     * filename is:
     *
     * Gamepad_F310_v1133_p1133_15b_6a
     *
     * where:
     *
     *   -  "Gamepad_F301" is the name reported by the driver
     *   -  "v1133_p1133" is the USB VID/PID if known
     *   -  "15b_6a" is the button/hat/axis count if known
     *
     * An example keyboard filename is:
     *
     * Keyboard_1
     *
     * where:
     *
     *   -  "Keyboard" is the name given to the keyboard by Kodi's peripheral subsystem
     *   - `"1" is the player number (for arcade cabinets that use keyboard drivers)
     */
    static std::string RootFileName(const kodi::addon::Joystick& device);

    /*!
     * From PeripheralTypes.h of Kodi
     */
    static int HexStringToInt(const char* strHex);

    /*!
     * From PeripheralTypes.h of Kodi
     */
    static std::string FormatHexString(int iVal);

    static std::string PrimitiveToString(const kodi::addon::DriverPrimitive& primitive);

  private:
    static std::set<std::string> m_existingDirs; // Cache list of existing dirs
  };
}
