/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *  Portions Copyright (C) 2013-2014 Lars Op den Kamp
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

namespace JOYSTICK
{
  enum SYS_LOG_LEVEL
  {
    SYS_LOG_NONE = 0,
    SYS_LOG_ERROR,
    SYS_LOG_INFO,
    SYS_LOG_DEBUG
  };

  enum SYS_LOG_TYPE
  {
    SYS_LOG_TYPE_NULL = 0, // Discard log
    SYS_LOG_TYPE_CONSOLE,  // Log to stdout
    SYS_LOG_TYPE_SYSLOG,   // Log to syslog
    SYS_LOG_TYPE_ADDON     // Log to frontend
  };

  class ILog
  {
  public:
    virtual ~ILog(void) { }

    virtual void Log(SYS_LOG_LEVEL level, const char* logline) = 0;

    virtual SYS_LOG_TYPE Type(void) const = 0;
  };
}
