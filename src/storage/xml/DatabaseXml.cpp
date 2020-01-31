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
