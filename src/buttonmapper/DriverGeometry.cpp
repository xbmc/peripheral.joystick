/*
 *      Copyright (C) 2016-2020 Garrett Brown
 *      Copyright (C) 2016-2020 Team Kodi
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

#include "DriverGeometry.h"

using namespace JOYSTICK;

CDriverGeometry::CDriverGeometry(unsigned int buttonCount, unsigned int hatCount, unsigned int axisCount) :
  m_buttonCount(buttonCount),
  m_hatCount(hatCount),
  m_axisCount(axisCount)
{
}

CDriverGeometry::CDriverGeometry(const CDriverGeometry& other) :
  m_buttonCount(other.ButtonCount()),
  m_hatCount(other.HatCount()),
  m_axisCount(other.AxisCount())
{
}

bool CDriverGeometry::IsValid() const
{
  return m_buttonCount != 0 ||
         m_hatCount    != 0 ||
         m_axisCount   != 0;
}

bool CDriverGeometry::operator<(const CDriverGeometry& other) const
{
  if (m_buttonCount < other.m_buttonCount) return true;
  if (m_buttonCount > other.m_buttonCount) return false;

  if (m_hatCount < other.m_hatCount) return true;
  if (m_hatCount > other.m_hatCount) return false;

  if (m_axisCount < other.m_axisCount) return true;
  if (m_axisCount > other.m_axisCount) return false;

  return false;
}
