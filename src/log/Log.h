/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *  Portions Copyright (C) 2013-2014 Lars Op den Kamp
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "ILog.h"

#include <mutex>

#ifndef esyslog
#define esyslog(...) JOYSTICK::CLog::Get().Log(SYS_LOG_ERROR, __VA_ARGS__)
#endif

#ifndef isyslog
#define isyslog(...) JOYSTICK::CLog::Get().Log(SYS_LOG_INFO, __VA_ARGS__)
#endif

#ifndef dsyslog
#define dsyslog(...) JOYSTICK::CLog::Get().Log(SYS_LOG_DEBUG, __VA_ARGS__)
#endif

#define LOG_ERROR_STR(s)  esyslog("ERROR (%s,%d): %s: %m", __FILE__, __LINE__, s)

namespace JOYSTICK
{
  class CLog
  {
  private:
    CLog(ILog* pipe);

  public:
    static CLog& Get(void);
    ~CLog(void);

    bool SetType(SYS_LOG_TYPE type);
    void SetPipe(ILog* pipe);
    void SetLevel(SYS_LOG_LEVEL level);

    void Log(SYS_LOG_LEVEL level, const char* format, ...);

    static const char* TypeToString(SYS_LOG_TYPE type);
    static const char* LevelToString(SYS_LOG_LEVEL level);

  private:
    ILog*            m_pipe;
    SYS_LOG_LEVEL    m_level;
    std::recursive_mutex m_mutex;
  };
}
