/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
