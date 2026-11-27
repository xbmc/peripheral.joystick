/*
 *  Copyright (C) 2014-2021 Garrett Brown
 *  Copyright (C) 2014-2021 Team Kodi
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
#if defined(HAVE_PERIPHERAL_JOYSTICK_BATTERY)
    virtual bool GetBattery(JOYSTICK_BATTERY& battery) override;
#endif

  protected:
    virtual bool ScanEvents(void) override;

  private:
    int         m_fd;
    std::string m_strFilename;
  };
}
