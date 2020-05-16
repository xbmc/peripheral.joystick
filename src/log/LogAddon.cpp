/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *  Portions Copyright (C) 2013-2014 Lars Op den Kamp
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "LogAddon.h"
#include "utils/CommonMacros.h"

#include <kodi/AddonBase.h>

using namespace JOYSTICK;

CLogAddon::CLogAddon()
{
}

void CLogAddon::Log(SYS_LOG_LEVEL level, const char* logline)
{
  AddonLog loglevel;

  switch (level)
  {
  case SYS_LOG_ERROR:
    loglevel = ADDON_LOG_ERROR;
    break;
  case SYS_LOG_INFO:
    loglevel = ADDON_LOG_INFO;
    break;
  case SYS_LOG_DEBUG:
    loglevel = ADDON_LOG_DEBUG;
    break;
  default:
    return;
  }

  kodi::Log(loglevel, logline);
}
