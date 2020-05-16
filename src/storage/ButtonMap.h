/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "StorageTypes.h"
#include "buttonmapper/ButtonMapTypes.h"

#include <chrono>
#include <set>
#include <stdint.h>
#include <string>

namespace JOYSTICK
{
  class IControllerHelper;

  class CButtonMap
  {
  public:
    CButtonMap(const std::string& strResourcePath, IControllerHelper *controllerHelper);
    CButtonMap(const std::string& strResourcePath, const DevicePtr& device, IControllerHelper *controllerHelper);

    virtual ~CButtonMap(void) { }

    const std::string& Path(void) const { return m_strResourcePath; }

    const DevicePtr& Device(void) const { return m_device; }

    bool IsValid(void) const;

    const ButtonMap& GetButtonMap();

    void MapFeatures(const std::string& controllerId, const FeatureVector& features);

    bool SaveButtonMap();

    bool RevertButtonMap();

    bool ResetButtonMap(const std::string& controllerId);

    bool Refresh(void);

  protected:
    virtual bool Load(void) = 0;
    virtual bool Save(void) const = 0;

    static void MergeFeature(const kodi::addon::JoystickFeature& feature, FeatureVector& features, const std::string& controllerId);

    static void Sanitize(FeatureVector& features, const std::string& controllerId);

    // Construction parameter
    IControllerHelper *const m_controllerHelper;

    const std::string m_strResourcePath;
    DevicePtr         m_device;
    DevicePtr         m_originalDevice;
    ButtonMap         m_buttonMap;
    ButtonMap         m_originalButtonMap;

  private:
    std::chrono::steady_clock::time_point m_timestamp;
    bool    m_bModified;
  };
}
