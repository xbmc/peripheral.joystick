/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "VFSFileUtils.h"

#include <kodi/Filesystem.h>

using namespace JOYSTICK;

bool CVFSFileUtils::Exists(const std::string& url)
{
  return kodi::vfs::FileExists(url, true);
}

bool CVFSFileUtils::Stat(const std::string& url, kodi::vfs::FileStatus& buffer)
{
  return kodi::vfs::StatFile(url, buffer);
}

bool CVFSFileUtils::Rename(const std::string& url, const std::string& newUrl)
{
  return kodi::vfs::RenameFile(url, newUrl);
}

bool CVFSFileUtils::Delete(const std::string& url)
{
  return kodi::vfs::DeleteFile(url);
}
