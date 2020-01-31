/*
 *      Copyright (C) 2015-2020 Garrett Brown
 *      Copyright (C) 2015-2020 Team Kodi
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

#include "Device.h"

using namespace JOYSTICK;

namespace JOYSTICK
{
  bool AreElementCountsKnown(const kodi::addon::Joystick &joystick)
  {
    return joystick.ButtonCount() != 0 ||
           joystick.HatCount() != 0 ||
           joystick.AxisCount() != 0;
  }
}

CDevice::CDevice(const kodi::addon::Joystick& joystick) :
  kodi::addon::Joystick(joystick)
{
}

void CDevice::Reset(void)
{
  Joystick::operator=(kodi::addon::Joystick());
  m_configuration.Reset();
}

bool CDevice::operator==(const CDevice& rhs) const
{
  return Name() == rhs.Name() &&
         Provider() == rhs.Provider() &&
         VendorID() == rhs.VendorID() &&
         ProductID() == rhs.ProductID() &&
         ButtonCount() == rhs.ButtonCount() &&
         HatCount() == rhs.HatCount() &&
         AxisCount() == rhs.AxisCount() &&
         Index() == rhs.Index();
}

bool CDevice::operator<(const CDevice& rhs) const
{
  if (Name() < rhs.Name()) return true;
  if (Name() > rhs.Name()) return false;

  if (Provider() < rhs.Provider()) return true;
  if (Provider() > rhs.Provider()) return false;

  if (VendorID() < rhs.VendorID()) return true;
  if (VendorID() > rhs.VendorID()) return false;

  if (ProductID() < rhs.ProductID()) return true;
  if (ProductID() > rhs.ProductID()) return false;

  if (ButtonCount() < rhs.ButtonCount()) return true;
  if (ButtonCount() > rhs.ButtonCount()) return false;

  if (HatCount() < rhs.HatCount()) return true;
  if (HatCount() > rhs.HatCount()) return false;

  if (AxisCount() < rhs.AxisCount()) return true;
  if (AxisCount() > rhs.AxisCount()) return false;

  if (Index() < rhs.Index()) return true;
  if (Index() > rhs.Index()) return false;

  return false;
}

bool CDevice::SimilarTo(const CDevice& other) const
{
  if (Provider() != other.Provider())
    return false;

  if (!Name().empty() && !other.Name().empty())
  {
    if (Name() != other.Name())
      return false;
  }

  if (IsVidPidKnown() && other.IsVidPidKnown())
  {
    if (VendorID() != other.VendorID() ||
        ProductID() != other.ProductID())
    {
      return false;
    }
  }

  if (AreElementCountsKnown(*this) && AreElementCountsKnown(other))
  {
    if (ButtonCount() != other.ButtonCount() ||
        HatCount() != other.HatCount() ||
        AxisCount() != other.AxisCount())
    {
      return false;
    }
  }

  return true;
}

bool CDevice::IsValid(void) const
{
  return !Name().empty() &&
         !Provider().empty();
}

void CDevice::MergeProperties(const CDevice& record)
{
  if (!record.Name().empty())
    SetName(record.Name());

  if (!record.Provider().empty())
    SetProvider(record.Provider());

  if (record.IsVidPidKnown())
  {
    SetVendorID(record.VendorID());
    SetProductID(record.ProductID());
  }

  if (AreElementCountsKnown(record))
  {
    SetButtonCount(record.ButtonCount());
    SetHatCount(record.HatCount());
    SetAxisCount(record.AxisCount());
  }

  SetIndex(record.Index());
}
