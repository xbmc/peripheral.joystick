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

namespace JOYSTICK
{
  class CLogConsole : public ILog
  {
  public:
    virtual ~CLogConsole(void) { }

    virtual void Log(SYS_LOG_LEVEL level, const char* logline) override;
    virtual SYS_LOG_TYPE Type(void) const override { return SYS_LOG_TYPE_CONSOLE; }

  private:
    std::recursive_mutex m_mutex;
  };
}
