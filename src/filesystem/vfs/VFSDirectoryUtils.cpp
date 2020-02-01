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
