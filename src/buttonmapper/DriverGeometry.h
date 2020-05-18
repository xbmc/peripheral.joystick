/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
