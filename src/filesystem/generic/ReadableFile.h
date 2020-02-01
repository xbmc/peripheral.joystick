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

#include "filesystem/IFile.h"

namespace JOYSTICK
{
  /*!
   * \brief Generic implementation of ReadFile() through other IFile methods
   *
   * NOTE: Derived class must implement IFile::Read()
   */
  class CReadableFile : public IFile
  {
  public:
    virtual ~CReadableFile(void) { }

    /*!
     * \brief Read an entire file in chunks through calls to IFile::Read()
     */
    virtual int64_t ReadFile(std::string& buffer, const uint64_t maxBytes = 0) override;
  };
}
