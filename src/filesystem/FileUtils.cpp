/*
 *      Copyright (C) 2015-2017 Garrett Brown
 *      Copyright (C) 2015-2017 Team Kodi
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

bool CFileUtils::Stat(const std::string& url, STAT_STRUCTURE& buffer)
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
