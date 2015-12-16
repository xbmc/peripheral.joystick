/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "DirectoryCache.h"

#include "platform/util/timeutils.h"

#include <algorithm>

using namespace JOYSTICK;

#define DIRECTORY_LIFETIME_MS  2000 // 2 seconds

// --- Helper function ---------------------------------------------------------

namespace JOYSTICK
{
  bool HasPath(const std::vector<ADDON::CVFSDirEntry>& items, const std::string& path)
  {
    return std::find_if(items.begin(), items.end(),
      [&path](const ADDON::CVFSDirEntry& item)
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

bool CDirectoryCache::GetDirectory(const std::string& path, std::vector<ADDON::CVFSDirEntry>& items)
{
  ItemMap::const_iterator itItemList = m_cache.find(path);

  if (itItemList != m_cache.end())
  {
    const ItemListRecord record = itItemList->second;

    // Check timestamp for stale data
    const int64_t timestamp = record.first;
    const int64_t expires = timestamp + DIRECTORY_LIFETIME_MS;

    if (expires <= PLATFORM::GetTimeMs())
    {
      items = record.second;
      return true;
    }
  }

  return false;
}

void CDirectoryCache::UpdateDirectory(const std::string& path, const std::vector<ADDON::CVFSDirEntry>& items)
{
  if (!m_callbacks)
    return;

  ItemListRecord record = m_cache[path];

  int64_t timestamp = record.first;
  ItemList cachedItems = record.second;

  // Remove missing items
  for (ItemList::const_iterator itOldItem = cachedItems.begin(); itOldItem != cachedItems.end(); ++itOldItem)
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

  m_cache[path].first = PLATFORM::GetTimeMs();
  m_cache[path].second = items;
}
