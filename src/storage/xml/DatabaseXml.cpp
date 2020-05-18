/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "DatabaseXml.h"
#include "ButtonMapXml.h"
#include "storage/StorageDefinitions.h"

using namespace JOYSTICK;

CDatabaseXml::CDatabaseXml(const std::string& strBasePath, bool bReadWrite, IDatabaseCallbacks* callbacks, IControllerHelper *controllerHelper) :
  CJustABunchOfFiles(strBasePath + "/" RESOURCE_XML_FOLDER, RESOURCE_XML_EXTENSION, bReadWrite, callbacks),
  m_controllerHelper(controllerHelper)
{
}

CButtonMap* CDatabaseXml::CreateResource(const std::string& resourcePath) const
{
  return new CButtonMapXml(resourcePath, m_controllerHelper);
}

CButtonMap* CDatabaseXml::CreateResource(const std::string& resourcePath, const DevicePtr& deviceInfo) const
{
  return new CButtonMapXml(resourcePath, deviceInfo, m_controllerHelper);
}
