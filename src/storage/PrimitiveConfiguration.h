/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
