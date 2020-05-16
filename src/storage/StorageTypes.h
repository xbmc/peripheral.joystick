/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <set>
#include <vector>

namespace JOYSTICK
{
  class CDevice;
  typedef std::shared_ptr<CDevice> DevicePtr;
  typedef std::vector<DevicePtr>   DeviceVector;
  typedef std::set<DevicePtr>      DeviceSet;

  class IDatabase;
  typedef std::shared_ptr<IDatabase> DatabasePtr;
  typedef std::vector<DatabasePtr>   DatabaseVector;
}
