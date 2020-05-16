/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "PeripheralScanner.h"
#include "addon.h"

using namespace kodi::addon;
using namespace JOYSTICK;

CPeripheralScanner::CPeripheralScanner(CPeripheralJoystick* peripheralLib)
  : m_peripheralLib(peripheralLib)
{
}

void CPeripheralScanner::TriggerScan(void)
{
  if (m_peripheralLib)
    m_peripheralLib->TriggerScan();
}
