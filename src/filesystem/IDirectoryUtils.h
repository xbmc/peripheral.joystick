/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/Filesystem.h>

#include <string>

namespace JOYSTICK
{
  class IDirectoryUtils
  {
  public:
    virtual ~IDirectoryUtils(void) { }

    /*!
     * \brief Create a directory
     * \param path Path to the directory
     * \return True if path is created, false otherwise
     */
    virtual bool Create(const std::string& path) = 0;

    /*!
     * \brief Check if a directory exists
     * \param path Directory to check
     * \return True if the directory exists, false otherwise
     */
    virtual bool Exists(const std::string& path) = 0;

    /*!
     * \brief Remove a directory
     * \param path The directory to remove
     * \return True if the directory was removed, false otherwise
     */
    virtual bool Remove(const std::string& path) = 0;

    /*!
     * \brief Get the contents of a directory
     * \param path The directory
     * \param mask The mask of file extensions that are allowed
     * \param[out] items The resulting contents, or empty if false is returned
     * \return True if the directory listing succeeded, even if no items are returned
     *
     * The mask has to look like the following:
     *
     *     .m4a|.flac|.aac|
     *
     * In this case, only *.m4a, *.flac, *.aac files will be retrieved with
     * GetDirectory().
     *
     * The trailing slash after .aac ensures a match at the end of the filename.
     */
    virtual bool GetDirectory(const std::string& path, const std::string& mask, std::vector<kodi::vfs::CDirEntry>& items) = 0;
  };
}
