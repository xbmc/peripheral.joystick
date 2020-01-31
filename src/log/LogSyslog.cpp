/*
 *      Copyright (C) 2014-2020 Garrett Brown
 *      Copyright (C) 2014-2020 Team Kodi
 *      Portions Copyright (C) 2013-2014 Lars Op den Kamp
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
