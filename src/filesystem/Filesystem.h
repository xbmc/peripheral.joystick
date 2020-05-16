/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

namespace JOYSTICK
{
  class CFilesystem
  {
  public:
    static bool Initialize(void);
    static void Deinitialize(void);
  };
}
