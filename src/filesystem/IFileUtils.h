/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <stdint.h>
#include <string>
#include <time.h>

#include <kodi/Filesystem.h>

namespace JOYSTICK
{
  class IFileUtils
  {
  public:
    virtual ~IFileUtils(void) { }

    /*!
     * \brief Check if the url exists
     */
    virtual bool Exists(const std::string& url) = 0;

    virtual bool Stat(const std::string& url, kodi::vfs::FileStatus& buffer) = 0;

    virtual bool Rename(const std::string& url, const std::string& newUrl) = 0;

    virtual bool Delete(const std::string& url) = 0;

    virtual bool SetHidden(const std::string& url, bool bHidden) { return false; }
  };
}
