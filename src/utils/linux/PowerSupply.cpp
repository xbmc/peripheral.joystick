/*
 *  Copyright (C) 2026 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "PowerSupply.h"

#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <limits.h>
#include <stdlib.h>

using namespace JOYSTICK;

namespace
{
  // From the input node, the battery hangs off the HID device or the USB
  // interface: event5 -> input12 -> 0005:045E:02E0.0006/power_supply
  constexpr unsigned int MAX_PARENT_DEPTH = 3;
}

bool CPowerSupply::GetBattery(const std::string& sysfsPath, JOYSTICK_BATTERY& battery)
{
  char resolvedPath[PATH_MAX];
  if (realpath(sysfsPath.c_str(), resolvedPath) == nullptr)
    return false;

  std::string devicePath = resolvedPath;

  for (unsigned int depth = 0; depth <= MAX_PARENT_DEPTH; depth++)
  {
    const std::string powerSupplyDir = devicePath + "/power_supply";

    DIR* dir = opendir(powerSupplyDir.c_str());
    if (dir != nullptr)
    {
      bool bFound = false;

      while (dirent* entry = readdir(dir))
      {
        const std::string name = entry->d_name;
        if (name == "." || name == "..")
          continue;

        if (ReadBattery(powerSupplyDir + "/" + name, battery))
        {
          bFound = true;
          break;
        }
      }

      closedir(dir);

      if (bFound)
        return true;
    }

    const size_t lastSlash = devicePath.find_last_of('/');
    if (lastSlash == std::string::npos || lastSlash == 0)
      break;

    devicePath.erase(lastSlash);
  }

  return false;
}

bool CPowerSupply::ReadBattery(const std::string& powerSupplyPath, JOYSTICK_BATTERY& battery)
{
  if (ReadAttribute(powerSupplyPath + "/type") != "Battery")
    return false;

  // A device's own battery, not one powering the whole system
  if (ReadAttribute(powerSupplyPath + "/scope") == "System")
    return false;

  int level = -1;

  // Fails until the device has reported its charge
  const std::string capacity = ReadAttribute(powerSupplyPath + "/capacity");
  if (!capacity.empty())
  {
    level = std::atoi(capacity.c_str());
  }
  else
  {
    // Some drivers only report the charge in steps
    const std::string capacityLevel = ReadAttribute(powerSupplyPath + "/capacity_level");
    if (capacityLevel == "Critical")
      level = 5;
    else if (capacityLevel == "Low")
      level = 25;
    else if (capacityLevel == "Normal")
      level = 50;
    else if (capacityLevel == "High")
      level = 75;
    else if (capacityLevel == "Full")
      level = 100;
  }

  if (level < 0)
    return false;

  const std::string status = ReadAttribute(powerSupplyPath + "/status");
  if (status == "Charging")
    battery.state = JOYSTICK_BATTERY_STATE_CHARGING;
  else if (status == "Full" || (status == "Not charging" && level >= 100))
    battery.state = JOYSTICK_BATTERY_STATE_FULL;
  else if (status == "Discharging" || status == "Not charging")
    battery.state = JOYSTICK_BATTERY_STATE_DISCHARGING;
  else
    battery.state = JOYSTICK_BATTERY_STATE_UNKNOWN;

  battery.level = std::min(level, 100);

  return true;
}

std::string CPowerSupply::ReadAttribute(const std::string& path)
{
  std::ifstream file(path);

  std::string value;
  std::getline(file, value);

  return value;
}
