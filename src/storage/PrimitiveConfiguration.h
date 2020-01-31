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
#pragma once

#include <map>

namespace JOYSTICK
{
  struct TriggerProperties
  {
    int center;
    unsigned int range;

    TriggerProperties() { Reset(); }

    void Reset()
    {
      center = 0;
      range = 1;
    }

    bool operator==(const TriggerProperties& other) const
    {
      return center == other.center &&
             range == other.range;
    }
  };

  struct AxisConfiguration
  {
    TriggerProperties trigger;
    bool bIgnore = false;

    bool operator==(const AxisConfiguration& other) const
    {
      return trigger == other.trigger &&
             bIgnore == other.bIgnore;
    }
  };

  struct ButtonConfiguration
  {
    bool bIgnore = false;

    bool operator==(const ButtonConfiguration& other) const
    {
      return bIgnore == other.bIgnore;
    }
  };

  typedef std::map<unsigned int, AxisConfiguration> AxisConfigurationMap;
  typedef std::map<unsigned int, ButtonConfiguration> ButtonConfigurationMap;
}
