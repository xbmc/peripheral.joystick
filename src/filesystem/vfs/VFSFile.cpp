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

#include "VFSFile.h"

#include "kodi/libXBMC_addon.h"

#include <algorithm>
#include <assert.h>
#include <limits>
#include <stddef.h>

using namespace JOYSTICK;

CVFSFile::CVFSFile(ADDON::CHelper_libXBMC_addon* frontend)
  : m_frontend(frontend),
    m_file(frontend),
    m_bOpen(false)
{
  assert(m_frontend);
}

bool CVFSFile::Open(const std::string& url, READ_FLAG flags /* = READ_FLAG_NONE */)
{
  if (m_file.Open(url, flags))
    m_bOpen = true;

  return m_bOpen;
}

bool CVFSFile::OpenForWrite(const std::string& url, bool bOverWrite /* = false */)
{
  if (m_file.OpenForWrite(url, bOverWrite))
    m_bOpen = true;

  return m_bOpen;
}

int64_t CVFSFile::Read(uint64_t byteCount, std::string& buffer)
{
  if (byteCount <= std::numeric_limits<size_t>::max())
  {
    buffer.resize((size_t)byteCount);

    return m_file.Read(const_cast<char*>(buffer.c_str()), byteCount);
  }

  return -1;
}

int64_t CVFSFile::ReadLine(std::string& buffer)
{
  return m_file.ReadLine(buffer) ? buffer.length() : -1;
}

int64_t CVFSFile::Write(uint64_t byteCount, const std::string& buffer)
{
  uint64_t writeSize = std::min(byteCount, (uint64_t)buffer.length());

  if (writeSize > 0)
    return m_file.Write(buffer.c_str(), writeSize);

  return -1;
}

void CVFSFile::Flush(void)
{
  return m_file.Flush();
}

int64_t CVFSFile::Seek(int64_t filePosition, SEEK_FLAG whence /* = SEEK_SET */)
{
  return m_file.Seek(filePosition, static_cast<unsigned int>(whence));
}

bool CVFSFile::Truncate(uint64_t size)
{
  return m_file.Truncate(size) == static_cast<int64_t>(size);
}

int64_t CVFSFile::GetPosition(void)
{
  return m_file.GetPosition();
}

int64_t CVFSFile::GetLength(void)
{
  return m_file.GetLength();
}

void CVFSFile::Close(void)
{
  if (m_bOpen)
  {
    m_bOpen = false;
    m_file.Close();
  }
}
