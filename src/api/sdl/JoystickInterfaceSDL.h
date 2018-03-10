/*
 *      Copyright (C) 2016-2017 Sam Lantinga
 *      Copyright (C) 2014-2017 Garrett Brown
 *      Copyright (C) 2014-2017 Team Kodi
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

#include "api/IJoystickInterface.h"

namespace JOYSTICK
{
  class CJoystickInterfaceSDL : public IJoystickInterface
  {
  public:
    CJoystickInterfaceSDL(void) { }
    virtual ~CJoystickInterfaceSDL(void) { Deinitialize(); }

    // Implementation of IJoystickInterface
    virtual EJoystickInterface Type(void) const override;
    virtual bool Initialize(void) override;
    virtual void Deinitialize(void) override;
    virtual bool ScanForJoysticks(JoystickVector& joysticks) override;

  private:
    bool m_bInitialScan = true;
  };
}
