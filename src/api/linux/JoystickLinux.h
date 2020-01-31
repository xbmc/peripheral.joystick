/*
 *      Copyright (C) 2014-2020 Garrett Brown
 *      Copyright (C) 2014-2020 Team Kodi
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
