/*
 *  Copyright (C) 2018-2020 Garrett Brown
 *  Copyright (C) 2018-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "StringRegistry.h"

#include <algorithm>
#include <iterator>

using namespace JOYSTICK;

unsigned int CStringRegistry::RegisterString(const std::string& str)
{
  unsigned int existingHandle;

  // Match an existing string to avoid extra malloc
  if (FindString(str, existingHandle))
    return existingHandle;

  // Append string
  m_strings.emplace_back(str);

  return static_cast<unsigned int>(m_strings.size()) - 1;
}

const std::string &CStringRegistry::GetString(unsigned int handle)
{
  if (handle < m_strings.size())
    return m_strings[handle];

  const static std::string empty;
  return empty;
}

bool CStringRegistry::FindString(const std::string &str, unsigned int &handle) const
{
  auto it = std::find(m_strings.begin(), m_strings.end(), str);
  if (it != m_strings.end())
  {
    handle = static_cast<unsigned int>(std::distance(m_strings.begin(), it));
    return true;
  }

  return false;
}
