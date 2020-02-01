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

namespace JOYSTICK
{
  class CDriverGeometry
  {
  public:
    CDriverGeometry(unsigned int buttonCount, unsigned int hatCount, unsigned int axisCount);
    CDriverGeometry(const CDriverGeometry& other);

    bool IsValid() const;

    bool operator<(const CDriverGeometry& other) const;

    unsigned int ButtonCount() const { return m_buttonCount; }
    unsigned int HatCount() const { return m_hatCount; }
    unsigned int AxisCount() const { return m_axisCount; }

  private:
    const unsigned int m_buttonCount;
    const unsigned int m_hatCount;
    const unsigned int m_axisCount;
  };
}
