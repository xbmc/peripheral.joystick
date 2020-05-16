/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "StorageUtils.h"
#include "Device.h"
#include "filesystem/DirectoryUtils.h"
#include "log/Log.h"
#include "utils/StringUtils.h"

#include <algorithm>
#include <set>
#include <sstream>
#include <stdio.h>

using namespace JOYSTICK;

std::set<std::string> CStorageUtils::m_existingDirs;

bool CStorageUtils::EnsureDirectoryExists(const std::string& path)
{
  if (m_existingDirs.find(path) != m_existingDirs.end())
    return true; // Already exists

  if (!CDirectoryUtils::Exists(path))
  {
    dsyslog("Creating directory: %s", path.c_str());
    if (!CDirectoryUtils::Create(path))
    {
      esyslog("Failed to create directory!");
      return false;
    }
  }

  m_existingDirs.insert(path);

  return true;
}

std::string CStorageUtils::RootFileName(const kodi::addon::Joystick& device)
{
  std::string baseFilename = StringUtils::MakeSafeUrl(device.Name());

  // Limit filename to a sane number of characters.
  if (baseFilename.length() > 50)
    baseFilename.erase(baseFilename.begin() + 50, baseFilename.end());

  // Append remaining properties
  std::stringstream filename;

  filename << baseFilename;
  if (device.IsVidPidKnown())
  {
    filename << "_v" << CStorageUtils::FormatHexString(device.VendorID());
    filename << "_p" << CStorageUtils::FormatHexString(device.ProductID());
  }
  if (device.ButtonCount() != 0)
    filename << "_" << device.ButtonCount() << "b";
  if (device.HatCount() != 0)
    filename << "_" << device.HatCount() << "h";
  if (device.AxisCount() != 0)
    filename << "_" << device.AxisCount() << "a";
  if (device.Index() != 0)
    filename << "_" << device.Index();

  return filename.str();
}

int CStorageUtils::HexStringToInt(const char* strHex)
{
  int iVal;
  sscanf(strHex, "%x", &iVal);
  return iVal;
};

std::string CStorageUtils::FormatHexString(int iVal)
{
  if (iVal < 0)
    iVal = 0;
  if (iVal > 65536)
    iVal = 65536;

  return StringUtils::Format("%04X", iVal);
};

std::string CStorageUtils::PrimitiveToString(const kodi::addon::DriverPrimitive& primitive)
{
  switch (primitive.Type())
  {
  case JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON:
    return StringUtils::Format("button %u", primitive.DriverIndex());
  case JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION:
    switch (primitive.HatDirection())
    {
    case JOYSTICK_DRIVER_HAT_UP:
      return "hat up";
    case JOYSTICK_DRIVER_HAT_RIGHT:
      return "hat right";
    case JOYSTICK_DRIVER_HAT_DOWN:
      return "hat down";
    case JOYSTICK_DRIVER_HAT_LEFT:
      return "hat left";
    default:
      break;
    }
    break;
  case JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS:
    return StringUtils::Format("axis %s%u",
        primitive.SemiAxisDirection() == JOYSTICK_DRIVER_SEMIAXIS_POSITIVE ? "+" : "-",
        primitive.DriverIndex());
  case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR:
    return StringUtils::Format("motor %u", primitive.DriverIndex());
  case JOYSTICK_DRIVER_PRIMITIVE_TYPE_KEY:
    return StringUtils::Format("key \"%s\"", primitive.Keycode().c_str());
  case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOUSE_BUTTON:
    return StringUtils::Format("mouse button %u", primitive.MouseIndex());
  case JOYSTICK_DRIVER_PRIMITIVE_TYPE_RELPOINTER_DIRECTION:
    switch (primitive.RelPointerDirection())
    {
    case JOYSTICK_DRIVER_RELPOINTER_UP:
      return "pointer up";
    case JOYSTICK_DRIVER_RELPOINTER_RIGHT:
      return "pointer right";
    case JOYSTICK_DRIVER_RELPOINTER_DOWN:
      return "pointer down";
    case JOYSTICK_DRIVER_RELPOINTER_LEFT:
      return "pointer left";
    default:
      break;
    }
    break;
  default:
    break;
  }

  return "";
}
