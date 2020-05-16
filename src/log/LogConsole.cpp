/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *  Portions Copyright (C) 2013-2014 Lars Op den Kamp
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "LogConsole.h"

#include <stdio.h>

using namespace JOYSTICK;

void CLogConsole::Log(SYS_LOG_LEVEL level, const char* logline)
{
  // TODO: Prepend current date

  std::lock_guard<std::recursive_mutex> lock(m_mutex);

  printf("%s\n", logline);
}
