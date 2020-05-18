/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "SeekableFile.h"

using namespace JOYSTICK;

int64_t CSeekableFile::GetLength(void)
{
  uint64_t length = -1;

  int64_t position = GetPosition();
  if (position >= 0)
  {
    Seek(0, SEEK_FLAG_END);

    length = GetPosition();

    Seek(position);
  }

  return length;
}
