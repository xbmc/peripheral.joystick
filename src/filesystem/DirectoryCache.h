/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
