/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "JoystickInterfaceCallback.h"
#include "Joystick.h"

using namespace JOYSTICK;

void CJoystickInterfaceCallback::AddScanResult(const JoystickPtr& joystick)
{
  m_scanResults.push_back(joystick);
}

void CJoystickInterfaceCallback::GetScanResults(JoystickVector& joysticks)
{
  joysticks.insert(joysticks.end(), m_scanResults.begin(), m_scanResults.end());
  m_scanResults.clear();
}
