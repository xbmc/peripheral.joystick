/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *  Portions Copyright (C) 2013-2014 Lars Op den Kamp
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "LogSyslog.h"

#include <syslog.h>

using namespace JOYSTICK;

CLogSyslog::CLogSyslog(void)
{
  // TODO: Get log level from CLog
  openlog("joystick", LOG_CONS, LOG_DEBUG); // LOG_PID doesn't work as expected under NPTL
}

CLogSyslog::~CLogSyslog(void)
{
  closelog();
}

void CLogSyslog::Log(SYS_LOG_LEVEL level, const char* logline)
{
  int priority = LOG_ERR;
  switch (level)
  {
  case SYS_LOG_ERROR:
    priority = LOG_ERR;
    break;
  case SYS_LOG_INFO:
    priority = LOG_INFO;
    break;
  case SYS_LOG_DEBUG:
    priority = LOG_DEBUG;
    break;
  default:
    return;
  }
  syslog(priority, "%s", logline);
}
