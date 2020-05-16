/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "filesystem/IFileUtils.h"

namespace JOYSTICK
{
  class CVFSFileUtils : public IFileUtils
  {
  public:
    CVFSFileUtils(void) { }

    virtual ~CVFSFileUtils(void) { }

    // implementation of IFileUtils
    virtual bool Exists(const std::string& url) override;
    virtual bool Stat(const std::string& url, STAT_STRUCTURE& buffer) override;
    virtual bool Rename(const std::string& url, const std::string& newUrl) override;
    virtual bool Delete(const std::string& url) override;
  };
}
