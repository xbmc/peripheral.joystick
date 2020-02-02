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

#include "DirectoryUtils.h"
#include "filesystem/vfs/VFSDirectoryUtils.h"

using namespace JOYSTICK;

bool CDirectoryUtils::Initialize()
{
  return true;
}

void CDirectoryUtils::Deinitialize(void)
{
}

// --- Directory operations ---------------------------------------------------------

bool CDirectoryUtils::Create(const std::string& path)
{
  // Create directory utils
  DirectoryUtilsPtr dirUtils = CreateDirectoryUtils(path);
  if (dirUtils)
    return dirUtils->Create(path);

  return false;
}

bool CDirectoryUtils::Exists(const std::string& path)
{
  // Create directory utils
  DirectoryUtilsPtr dirUtils = CreateDirectoryUtils(path);
  if (dirUtils)
    return dirUtils->Exists(path);

  return false;
}

bool CDirectoryUtils::Remove(const std::string& path)
{
  // Create directory utils
  DirectoryUtilsPtr dirUtils = CreateDirectoryUtils(path);
  if (dirUtils)
    return dirUtils->Remove(path);

  return false;
}

bool CDirectoryUtils::GetDirectory(const std::string& path, const std::string& mask, std::vector<kodi::vfs::CDirEntry>& items)
{
  // Create directory utils
  DirectoryUtilsPtr dirUtils = CreateDirectoryUtils(path);
  if (dirUtils)
    return dirUtils->GetDirectory(path, mask, items);

  return false;
}

DirectoryUtilsPtr CDirectoryUtils::CreateDirectoryUtils(const std::string& url)
{
  return DirectoryUtilsPtr(new CVFSDirectoryUtils());
}
