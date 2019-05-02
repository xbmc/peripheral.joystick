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

#include "DirectoryCache.h"

#include <algorithm>
#include <chrono>

using namespace JOYSTICK;

static constexpr std::chrono::seconds DIRECTORY_LIFETIME = std::chrono::seconds(2);

// --- Helper function ---------------------------------------------------------

namespace JOYSTICK
{
  bool HasPath(const std::vector<kodi::vfs::CDirEntry>& items, const std::string& path)
  {
    return std::find_if(items.begin(), items.end(),
      [&path](const kodi::vfs::CDirEntry& item)
      {
        return item.Path() == path;
      }) != items.end();
  }
}

// --- CDirectoryCache ---------------------------------------------------------

void CDirectoryCache::Initialize(IDirectoryCacheCallback* callbacks)
{
  m_callbacks = callbacks;
}

void CDirectoryCache::Deinitialize(void)
{
  m_callbacks = nullptr;
}

bool CDirectoryCache::GetDirectory(const std::string& path, std::vector<kodi::vfs::CDirEntry>& items)
{
  ItemMap::const_iterator itItemList = m_cache.find(path);

  if (itItemList != m_cache.end())
  {
    const ItemListRecord& record = itItemList->second;

    // Check timestamp for stale data
    const std::chrono::steady_clock::time_point timestamp = record.first;
    const std::chrono::steady_clock::time_point expires = timestamp + DIRECTORY_LIFETIME;

    if (expires <= std::chrono::steady_clock::now())
    {
      items = record.second;
      return true;
    }
  }

  return false;
}

void CDirectoryCache::UpdateDirectory(const std::string& path, const std::vector<kodi::vfs::CDirEntry>& items)
{
  if (!m_callbacks)
    return;

  ItemListRecord& record = m_cache[path];

  std::chrono::steady_clock::time_point& timestamp = record.first;
  ItemList& cachedItems = record.second;

  // Remove missing items
  for (ItemList::iterator itOldItem = cachedItems.begin(); itOldItem != cachedItems.end(); ++itOldItem)
  {
    if (!HasPath(items, itOldItem->Path()))
    {
      // Item was removed
      m_callbacks->OnRemove(*itOldItem);
    }
  }

  // Add new items
  for (ItemList::const_iterator itNewItem = items.begin(); itNewItem != items.end(); ++itNewItem)
  {
    if (!HasPath(cachedItems, itNewItem->Path()))
    {
      // Item is being added
      m_callbacks->OnAdd(*itNewItem);
    }
  }

  timestamp = std::chrono::steady_clock::now();
  cachedItems = items;
}
