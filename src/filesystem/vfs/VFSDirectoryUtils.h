/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "filesystem/IDirectoryUtils.h"

namespace JOYSTICK
{
  class CVFSDirectoryUtils : public IDirectoryUtils
  {
  public:
    CVFSDirectoryUtils(void) { }

    virtual ~CVFSDirectoryUtils(void) { }

    // implementation of IDirectoryUtils
    virtual bool Create(const std::string& path) override;
    virtual bool Exists(const std::string& path) override;
    virtual bool Remove(const std::string& path) override;
    virtual bool GetDirectory(const std::string& path, const std::string& mask, std::vector<kodi::vfs::CDirEntry>& items) override;
  };
}
