/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "FileUtils.h"
#include "filesystem/vfs/VFSFileUtils.h"

using namespace JOYSTICK;

bool CFileUtils::Initialize(void)
{
  return true;
}

void CFileUtils::Deinitialize(void)
{
}

bool CFileUtils::Exists(const std::string& url)
{
  // Create file utils
  FileUtilsPtr fileUtils = CreateFileUtils(url);
  if (fileUtils)
    return fileUtils->Exists(url);

  return false;
}

bool CFileUtils::Stat(const std::string& url, kodi::vfs::FileStatus& buffer)
{
  // Create file utils
  FileUtilsPtr fileUtils = CreateFileUtils(url);
  if (fileUtils)
    return fileUtils->Stat(url, buffer);

  return false;
}

bool CFileUtils::Rename(const std::string& url, const std::string& newUrl)
{
  // Create file utils
  FileUtilsPtr fileUtils = CreateFileUtils(url);
  if (fileUtils)
    return fileUtils->Rename(url, newUrl);

  return false;
}

bool CFileUtils::Delete(const std::string& url)
{
  // Create file utils
  FileUtilsPtr fileUtils = CreateFileUtils(url);
  if (fileUtils)
    return fileUtils->Delete(url);

  return false;
}

bool CFileUtils::SetHidden(const std::string& url, bool bHidden)
{
  // Create file utils
  FileUtilsPtr fileUtils = CreateFileUtils(url);
  if (fileUtils)
    return fileUtils->SetHidden(url, bHidden);

  return false;
}

FileUtilsPtr CFileUtils::CreateFileUtils(const std::string& url)
{
  return FileUtilsPtr(new CVFSFileUtils());
}
