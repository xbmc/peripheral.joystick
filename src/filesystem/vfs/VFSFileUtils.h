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
