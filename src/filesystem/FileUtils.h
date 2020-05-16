/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "FilesystemTypes.h"
#include "IFile.h"
#include "IFileUtils.h"

#include <string>

namespace JOYSTICK
{
  class CFileUtils
  {
  public:
    static bool Initialize(void);
    static void Deinitialize(void);

    static bool Exists(const std::string& url);
    static bool Stat(const std::string& url, kodi::vfs::FileStatus& buffer);
    static bool Rename(const std::string& url, const std::string& newUrl);
    static bool Delete(const std::string& url);
    static bool SetHidden(const std::string& url, bool bHidden);

  private:
    /*!
     * \brief Create a file utilities instance to handle the specified URL
     *
     * \return The file utilities instance, or empty if no file utility
     *         implementations can handle the URL
     */
    static FileUtilsPtr CreateFileUtils(const std::string& url);
  };
}
