/*
 *      Copyright (C) 2018 Garrett Brown
 *      Copyright (C) 2018 Team Kodi
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

#include <string>
#include <vector>

namespace JOYSTICK
{
  class CStringRegistry
  {
  public:
    CStringRegistry() = default;

    unsigned int RegisterString(const std::string& str);

    const std::string &GetString(unsigned int handle);

  private:
    bool FindString(const std::string &str, unsigned int &handle) const;

    std::vector<std::string> m_strings;
  };
}
