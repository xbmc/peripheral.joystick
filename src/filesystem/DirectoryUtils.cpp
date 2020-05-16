/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
