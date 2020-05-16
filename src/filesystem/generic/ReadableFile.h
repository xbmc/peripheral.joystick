/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
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
