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
   * \brief Generic implementation of GetLength() through other IFile methods
   *
   * NOTE: Derived class must implement IFile::Seek() and IFile::GetPosition()
   */
  class CSeekableFile : public IFile
  {
  public:
    virtual ~CSeekableFile(void) { }

    /*!
     * \brief Determine the length by seeking to the end and getting the position
     */
    virtual int64_t GetLength(void) override;
  };
}
