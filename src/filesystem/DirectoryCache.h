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
#pragma once

#include <kodi/Filesystem.h>

#include <chrono>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

namespace JOYSTICK
{
  class IDirectoryCacheCallback
  {
  public:
    virtual ~IDirectoryCacheCallback(void) { }

    virtual void OnAdd(const kodi::vfs::CDirEntry& item) = 0;
    virtual void OnRemove(const kodi::vfs::CDirEntry& item) = 0;
  };

  class CDirectoryCache
  {
  public:
    void Initialize(IDirectoryCacheCallback* callbacks);
    void Deinitialize(void);

    bool GetDirectory(const std::string& path, std::vector<kodi::vfs::CDirEntry>& items);
    void UpdateDirectory(const std::string& path, const std::vector<kodi::vfs::CDirEntry>& items);

  private:
    IDirectoryCacheCallback* m_callbacks;

    typedef std::vector<kodi::vfs::CDirEntry>     ItemList;
    typedef std::pair<std::chrono::steady_clock::time_point, ItemList> ItemListRecord;
    typedef std::map<std::string, ItemListRecord> ItemMap;

    ItemMap m_cache;
  };
}
