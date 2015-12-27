/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include <string>

namespace JOYSTICK
{
  class StringUtils
  {
  public:
    /*!
     * \brief Transform characters to create a safe URL
     * \param str The string to transform
     * \return The transformed string, with unsafe characters replaced by "_"
     *
     * Safe URLs are composed of the unreserved characters defined in
     * RFC 3986 section 2.3:
     *
     *   ALPHA / DIGIT / "-" / "." / "_" / "~"
     *
     * Characters outside of this set will be replaced by "_".
     */
    static std::string MakeSafeUrl(const std::string& str);

    /*!
     * \brief Removes a MAC address from a given string
     * \param str The string containing a MAC address
     * \return The string without the MAC address (for chaining)
     */
    static std::string& RemoveMACAddress(std::string& str);

    static std::string& Trim(std::string& str);
    static std::string& TrimLeft(std::string& str);
    static std::string& TrimRight(std::string& str);

    static std::string& Trim(std::string& str, const char* chars);
    static std::string& TrimLeft(std::string& str, const char* chars);
    static std::string& TrimRight(std::string& str, const char* chars);

    static bool EndsWith(const std::string& str, const std::string& suffix);
  };
}
