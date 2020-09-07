/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "DeviceConfiguration.h"

#include <kodi/addon-instance/Peripheral.h>

namespace JOYSTICK
{
  /*!
   * \brief Record for devices in the button map database
   *
   * Device properties are inherited from kodi::addon::Joystick. These are:
   *
   *   - Device type
   *   - Name                     [ or "" if unknown ]
   *   - USB Vendor ID/Product ID [ or 00:00 if unknown ]
   *   - Driver index
   *   - Provider (driver name)   [ required ]
   *   - Requested Port           [ or -1 for no port requested ]
   *   - Button count
   *   - Hat count
   *   - Axis count
   *   - Motor count
   *
   * This class contains logic for comparing, sorting and combining device
   * records, as well as utility functions.
   *
   * The number of motors is hard-coded in the api section of the code and is
   * not used in the handling logic.
   */
  class CDevice : public kodi::addon::Joystick
  {
  public:
    CDevice(void) = default;
    CDevice(const kodi::addon::Joystick& joystick);
    virtual ~CDevice(void) = default;

    void Reset(void);

    /*!
     * \brief Define a comparison operator for driver records
     */
    bool operator==(const CDevice& rhs) const;
    bool operator!=(const CDevice& rhs) const { return !(*this == rhs); }

    /*!
     * \brief Define a total order for driver records
     */
    bool operator<(const CDevice& rhs) const;

    /*!
     * \brief Define a similarity metric for driver records
     */
    bool SimilarTo(const CDevice& rhs) const;

    /*!
     * \brief Define a validity test for driver records
     */
    bool IsValid(void) const;

    /*!
     * \brief Merge valid (known) properties of given record into this record
     */
    void MergeProperties(const CDevice& record);

    CDeviceConfiguration& Configuration(void) { return m_configuration; }
    const CDeviceConfiguration& Configuration(void) const { return m_configuration; }

  private:
    CDeviceConfiguration m_configuration;
  };
}
