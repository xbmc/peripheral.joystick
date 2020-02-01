/*
 *      Copyright (C) 2015-2020 Garrett Brown
 *      Copyright (C) 2015-2020 Team Kodi
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
