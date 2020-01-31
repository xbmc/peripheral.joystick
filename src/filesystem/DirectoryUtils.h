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
#pragma once

#include "FilesystemTypes.h"
#include "IDirectoryUtils.h"

#include <string>

namespace JOYSTICK
{
  class CDirectoryUtils
  {
  public:
    static bool Initialize(void);
    static void Deinitialize(void);

    // Directory operations
    static bool Create(const std::string& path);
    static bool Exists(const std::string& path);
    static bool Remove(const std::string& path);
    static bool GetDirectory(const std::string& path, const std::string& mask, std::vector<kodi::vfs::CDirEntry>& items);

  private:
    /*!
     * \brief Create a directory utility instance to handle the specified URL
     *
     * \return The directory utility instance, or empty if no directory utility
     *         implementations can handle the URL
     */
    static DirectoryUtilsPtr CreateDirectoryUtils(const std::string& url);
  };
}
