/*
 *  Copyright (C) 2018-2020 Garrett Brown
 *  Copyright (C) 2018-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
