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

#include "VFSFileUtils.h"

#include <kodi/Filesystem.h>

using namespace JOYSTICK;

bool CVFSFileUtils::Exists(const std::string& url)
{
  return kodi::vfs::FileExists(url, true);
}

bool CVFSFileUtils::Stat(const std::string& url, STAT_STRUCTURE& buffer)
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
