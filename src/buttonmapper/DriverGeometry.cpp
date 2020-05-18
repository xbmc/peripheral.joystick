/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
