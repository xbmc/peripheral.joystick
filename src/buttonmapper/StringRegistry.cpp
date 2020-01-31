/*
 *      Copyright (C) 2018-2020 Garrett Brown
 *      Copyright (C) 2018-2020 Team Kodi
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
