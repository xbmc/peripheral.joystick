/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <memory>

namespace JOYSTICK
{
  class IFile;
  typedef std::shared_ptr<IFile> FilePtr;

  class IFileUtils;
  typedef std::shared_ptr<IFileUtils> FileUtilsPtr;

  class IDirectoryUtils;
  typedef std::shared_ptr<IDirectoryUtils> DirectoryUtilsPtr;
}
