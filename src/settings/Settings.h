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
