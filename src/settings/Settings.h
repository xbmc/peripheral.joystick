/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <string>
#include <kodi/General.h>

namespace JOYSTICK
{
  class CSettings
  {
  private:
    CSettings(void);

  public:
    static CSettings& Get(void);

    /*!
     * \brief Set the value of a setting. Called by the frontend.
     */
    void SetSetting(const std::string& strName, const kodi::CSettingValue& value);

    /*!
     * \brief Check if the settings have been initialized
     *
     * This will return true if SetSetting() has been called at least once.
     */
    bool IsInitialized(void) const { return m_bInitialized; }

    /*!
     * \brief Generate .cfg files compatible with RetroArch's joypad autoconfig
     */
    bool GenerateRetroArchConfigs(void) const { return m_bGenerateRetroArchConfigs; }

  private:
    bool        m_bInitialized;
    bool        m_bGenerateRetroArchConfigs;
  };
}
