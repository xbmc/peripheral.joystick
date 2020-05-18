/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "VFSDirectoryUtils.h"
#include <kodi/Filesystem.h>

#include <assert.h>

using namespace JOYSTICK;

#if defined(TARGET_WINDOWS)
#if defined(CreateDirectory)
#define _CreateDirectory CreateDirectory
#undef CreateDirectory
#endif
#if defined(RemoveDirectory)
#define _RemoveDirectory RemoveDirectory
#undef RemoveDirectory
#endif
#endif

bool CVFSDirectoryUtils::Create(const std::string& path)
{
  return kodi::vfs::CreateDirectory(path.c_str());
}

bool CVFSDirectoryUtils::Exists(const std::string& path)
{
  return kodi::vfs::DirectoryExists(path.c_str());
}

bool CVFSDirectoryUtils::Remove(const std::string& path)
{
  return kodi::vfs::RemoveDirectory(path.c_str());
}

bool CVFSDirectoryUtils::GetDirectory(const std::string& path, const std::string& mask, std::vector<kodi::vfs::CDirEntry>& items)
{
  return kodi::vfs::GetDirectory(path, mask, items);
}

#if defined(TARGET_WINDOWS)
#if defined(_CreateDirectory)
#define CreateDirectory _CreateDirectory
#undef _CreateDirectory
#endif
#if defined(_RemoveDirectory)
#define RemoveDirectory _RemoveDirectory
#undef _RemoveDirectory
#endif
#endif
