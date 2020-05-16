/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "api/Joystick.h"

#include <stdint.h>
#include <string>

namespace JOYSTICK
{
  class CJoystickLinux : public CJoystick
  {
  public:
    CJoystickLinux(int fd, const std::string& strFilename);
    virtual ~CJoystickLinux(void) { Deinitialize(); }

    // implementation of CJoystick
    virtual void Deinitialize(void) override;
    virtual bool Equals(const CJoystick* rhs) const override;

  protected:
    virtual bool ScanEvents(void) override;

  private:
    int         m_fd;
    std::string m_strFilename;
  };
}
