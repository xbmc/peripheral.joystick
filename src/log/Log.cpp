/*
 *      Copyright (C) 2014-2017 Garrett Brown
 *      Copyright (C) 2014-2017 Team Kodi
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

#include "Log.h"
#include "LogAddon.h"
#include "LogConsole.h"

#if defined(HAVE_SYSLOG)
#include "LogSyslog.h"
#endif

#include <stdarg.h>
#include <stdio.h>

using namespace JOYSTICK;

#define MAXSYSLOGBUF (256)

CLog::CLog(ILog* pipe)
 : m_pipe(pipe),
   m_level(SYS_LOG_DEBUG)
{
}

CLog& CLog::Get(void)
{
  static CLog _instance(new CLogConsole);
  return _instance;
}

CLog::~CLog(void)
{
  SetPipe(NULL);
}

bool CLog::SetType(SYS_LOG_TYPE type)
{
  P8PLATFORM::CLockObject lock(m_mutex);
  if (m_pipe && m_pipe->Type() == type)
    return true; // Already set

  switch (type)
  {
  case SYS_LOG_TYPE_CONSOLE:
    SetPipe(new CLogConsole);
    break;
#if defined(HAVE_SYSLOG)
  case SYS_LOG_TYPE_SYSLOG:
    SetPipe(new CLogSyslog);
    break;
#endif
  case SYS_LOG_TYPE_NULL:
    SetPipe(NULL);
    break;
  case SYS_LOG_TYPE_ADDON: // Must be set through SetPipe() because CLogAddon has no default constructor
  default:
    Log(SYS_LOG_ERROR, "Failed to set log type to %s", TypeToString(type));
    return false;
  }

  return true;
}

void CLog::SetPipe(ILog* pipe)
{
  P8PLATFORM::CLockObject lock(m_mutex);

  const SYS_LOG_TYPE newType = pipe   ? pipe->Type()   : SYS_LOG_TYPE_NULL;
  const SYS_LOG_TYPE oldType = m_pipe ? m_pipe->Type() : SYS_LOG_TYPE_NULL;

  delete m_pipe;
  m_pipe = pipe;
}

void CLog::SetLevel(SYS_LOG_LEVEL level)
{
  P8PLATFORM::CLockObject lock(m_mutex);

  const SYS_LOG_LEVEL newLevel = level;
  const SYS_LOG_LEVEL oldLevel = m_level;

  m_level = level;
}

void CLog::Log(SYS_LOG_LEVEL level, const char* format, ...)
{
  char fmt[MAXSYSLOGBUF];
  char buf[MAXSYSLOGBUF];
  va_list ap;

  va_start(ap, format);
  snprintf(fmt, sizeof(fmt), "%s", format); // TODO: Prepend CThread::ThreadId()
  vsnprintf(buf, MAXSYSLOGBUF - 1, fmt, ap);
  va_end(ap);

  P8PLATFORM::CLockObject lock(m_mutex);

  if (level > m_level)
    return;

  if (m_pipe)
    m_pipe->Log(level, buf);
}

const char* CLog::TypeToString(SYS_LOG_TYPE type)
{
  switch (type)
  {
  case SYS_LOG_TYPE_NULL:
    return "null";
  case SYS_LOG_TYPE_CONSOLE:
    return "console";
  case SYS_LOG_TYPE_SYSLOG:
    return "syslog";
  case SYS_LOG_TYPE_ADDON:
    return "addon";
  default:
    return "unknown";
  }
}

const char* CLog::LevelToString(SYS_LOG_LEVEL level)
{
  switch (level)
  {
  case SYS_LOG_NONE:
    return "none";
  case SYS_LOG_ERROR:
    return "error";
  case SYS_LOG_INFO:
    return "info";
  case SYS_LOG_DEBUG:
    return "debug";
  default:
    return "unknown";
  }
}
