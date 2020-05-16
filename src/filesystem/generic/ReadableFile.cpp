/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ReadableFile.h"

#include <algorithm>

using namespace JOYSTICK;

// Size of buffer used for each Read() call
#define READ_CHUNK_SIZE  (100 * 1024) // 100 KB

int64_t CReadableFile::ReadFile(std::string& buffer, const uint64_t maxBytes /* = 0 */)
{
  std::string chunkBuffer;
  chunkBuffer.reserve(READ_CHUNK_SIZE);

  int64_t bytesRead = 0;
  int64_t bytesToRead = maxBytes;

  const bool bReadForever = (maxBytes == 0);
  while (bReadForever || bytesToRead > 0)
  {
    // Read a chunk of data via Read() API call
    unsigned int chunkReadSize = bReadForever ? READ_CHUNK_SIZE :
        (unsigned int)std::min((int64_t)READ_CHUNK_SIZE, (int64_t)bytesToRead);

    int64_t chunkBytesRead = Read(chunkReadSize, chunkBuffer);

    // If the read failed, bail out
    if (chunkBytesRead < 0)
      return -1;

    // If no bytes were read, assume we hit the end of the file
    if (chunkBytesRead == 0)
      break;

    bytesRead += chunkBytesRead;

    if (!bReadForever)
      bytesToRead -= chunkBytesRead;

    buffer.append(chunkBuffer);

    // If less bytes were read that requested, assume we hit the end of the file
    if (chunkBytesRead < chunkReadSize)
      break;
  }

  return bytesRead;
}
