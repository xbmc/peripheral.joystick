/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "storage/JustABunchOfFiles.h"

#include <string>
#include <vector>

class TiXmlElement;

namespace JOYSTICK
{
  class IControllerHelper;

  class CDatabaseXml : public CJustABunchOfFiles
  {
  public:
    CDatabaseXml(const std::string& strBasePath, bool bReadWrite, IDatabaseCallbacks* callbacks, IControllerHelper *controllerHelper);

    virtual ~CDatabaseXml(void) { }

    // implementation of CJustABunchOfFiles
    virtual CButtonMap* CreateResource(const std::string& resourcePath) const override;
    virtual CButtonMap* CreateResource(const std::string& resourcePath, const DevicePtr& deviceInfo) const override;

  private:
    // Construction parameter
    IControllerHelper *const m_controllerHelper;
  };
}
