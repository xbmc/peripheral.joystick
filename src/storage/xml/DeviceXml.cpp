/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "DeviceXml.h"
#include "ButtonMapDefinitions.h"
#include "storage/Device.h"
#include "storage/DeviceConfiguration.h"
#include "storage/PrimitiveConfiguration.h"
#include "storage/StorageUtils.h"
#include "log/Log.h"

#include <tinyxml.h>

#include <cstdlib>
#include <utility>

using namespace JOYSTICK;

bool CDeviceXml::Serialize(const CDevice& record, TiXmlElement* pElement)
{
  if (!pElement)
    return false;

  pElement->SetAttribute(BUTTONMAP_XML_ATTR_DEVICE_NAME, record.Name());
  pElement->SetAttribute(BUTTONMAP_XML_ATTR_DEVICE_PROVIDER, record.Provider());
  if (record.IsVidPidKnown())
  {
    pElement->SetAttribute(BUTTONMAP_XML_ATTR_DEVICE_VID, CStorageUtils::FormatHexString(record.VendorID()));
    pElement->SetAttribute(BUTTONMAP_XML_ATTR_DEVICE_PID, CStorageUtils::FormatHexString(record.ProductID()));
  }
  if (record.ButtonCount() != 0)
    pElement->SetAttribute(BUTTONMAP_XML_ATTR_DEVICE_BUTTONCOUNT, record.ButtonCount());
  if (record.HatCount() != 0)
    pElement->SetAttribute(BUTTONMAP_XML_ATTR_DEVICE_HATCOUNT, record.HatCount());
  if (record.AxisCount() != 0)
    pElement->SetAttribute(BUTTONMAP_XML_ATTR_DEVICE_AXISCOUNT, record.AxisCount());
  if (record.Index() != 0)
    pElement->SetAttribute(BUTTONMAP_XML_ATTR_DEVICE_INDEX, record.Index());

  if (!SerializeConfig(record.Configuration(), pElement))
    return false;

  return true;
}

bool CDeviceXml::Deserialize(const TiXmlElement* pElement, CDevice& record)
{
  if (!pElement)
    return false;

  record.Reset();

  const char* name = pElement->Attribute(BUTTONMAP_XML_ATTR_DEVICE_NAME);
  if (!name)
  {
    esyslog("<%s> tag has no \"%s\" attribute", BUTTONMAP_XML_ELEM_DEVICE, BUTTONMAP_XML_ATTR_DEVICE_NAME);
    return false;
  }
  record.SetName(name);

  const char* provider = pElement->Attribute(BUTTONMAP_XML_ATTR_DEVICE_PROVIDER);
  if (!provider)
  {
    esyslog("<%s> tag has no \"%s\" attribute", BUTTONMAP_XML_ELEM_DEVICE, BUTTONMAP_XML_ATTR_DEVICE_PROVIDER);
    return false;
  }
  record.SetProvider(provider);

  const char* vid = pElement->Attribute(BUTTONMAP_XML_ATTR_DEVICE_VID);
  if (vid)
    record.SetVendorID(CStorageUtils::HexStringToInt(vid));

  const char* pid = pElement->Attribute(BUTTONMAP_XML_ATTR_DEVICE_PID);
  if (pid)
    record.SetProductID(CStorageUtils::HexStringToInt(pid));

  const char* buttonCount = pElement->Attribute(BUTTONMAP_XML_ATTR_DEVICE_BUTTONCOUNT);
  if (buttonCount)
    record.SetButtonCount(std::atoi(buttonCount));

  const char* hatCount = pElement->Attribute(BUTTONMAP_XML_ATTR_DEVICE_HATCOUNT);
  if (hatCount)
    record.SetHatCount(std::atoi(hatCount));

  const char* axisCount = pElement->Attribute(BUTTONMAP_XML_ATTR_DEVICE_AXISCOUNT);
  if (axisCount)
    record.SetAxisCount(std::atoi(axisCount));

  const char* index = pElement->Attribute(BUTTONMAP_XML_ATTR_DEVICE_INDEX);
  if (index)
    record.SetIndex(std::atoi(index));

  if (!DeserializeConfig(pElement, record.Configuration()))
    return false;

  return true;
}

bool CDeviceXml::SerializeConfig(const CDeviceConfiguration& config, TiXmlElement* pElement)
{
  if (!config.IsEmpty())
  {
    TiXmlElement configurationElement(BUTTONMAP_XML_ELEM_CONFIGURATION);
    TiXmlNode* configurationNode = pElement->InsertEndChild(configurationElement);
    if (configurationNode == nullptr)
      return false;

    TiXmlElement* configurationElem = configurationNode->ToElement();
    if (configurationElem == nullptr)
      return false;

    for (const auto& axis : config.Axes())
    {
      if (!SerializeAxis(axis.first, axis.second, configurationElem))
        return false;
    }

    for (const auto& button : config.Buttons())
    {
      if (!SerializeButton(button.first, button.second, configurationElem))
        return false;
    }
  }

  return true;
}

bool CDeviceXml::DeserializeConfig(const TiXmlElement* pElement, CDeviceConfiguration& config)
{
  const TiXmlElement* pDevice = pElement->FirstChildElement(BUTTONMAP_XML_ELEM_CONFIGURATION);

  if (pDevice)
  {
    const TiXmlElement* pAxis = pDevice->FirstChildElement(BUTTONMAP_XML_ELEM_AXIS);

    for ( ; pAxis != nullptr; pAxis = pAxis->NextSiblingElement(BUTTONMAP_XML_ELEM_AXIS))
    {
      unsigned int axisIndex;
      AxisConfiguration axisConfig;
      if (!DeserializeAxis(pAxis, axisIndex, axisConfig))
        return false;

      config.SetAxis(axisIndex, axisConfig);
    }

    const TiXmlElement* pButton = pDevice->FirstChildElement(BUTTONMAP_XML_ELEM_BUTTON);

    for ( ; pButton != nullptr; pButton = pButton->NextSiblingElement(BUTTONMAP_XML_ELEM_BUTTON))
    {
      unsigned int buttonIndex;
      ButtonConfiguration buttonConfig;
      if (!DeserializeButton(pButton, buttonIndex, buttonConfig))
        return false;

      config.SetButton(buttonIndex, buttonConfig);
    }
  }

  return true;
}

bool CDeviceXml::SerializeAxis(unsigned int index, const AxisConfiguration& axisConfig, TiXmlElement* pElement)
{
  AxisConfiguration defaultConfig{ };
  if (!(axisConfig == defaultConfig))
  {
    TiXmlElement axisElement(BUTTONMAP_XML_ELEM_AXIS);
    TiXmlNode* axisNode = pElement->InsertEndChild(axisElement);
    if (axisNode == nullptr)
      return false;

    TiXmlElement* axisElem = axisNode->ToElement();
    if (axisElem == nullptr)
      return false;

    axisElem->SetAttribute(BUTTONMAP_XML_ATTR_DRIVER_INDEX, index);

    TriggerProperties defaultTrigger{ };
    if (!(axisConfig.trigger == defaultTrigger))
    {
      axisElem->SetAttribute(BUTTONMAP_XML_ATTR_AXIS_CENTER, axisConfig.trigger.center);
      axisElem->SetAttribute(BUTTONMAP_XML_ATTR_AXIS_RANGE, axisConfig.trigger.range);
    }

    if (axisConfig.bIgnore)
      axisElem->SetAttribute(BUTTONMAP_XML_ATTR_IGNORE, "true");
  }

  return true;
}

bool CDeviceXml::SerializeButton(unsigned int index, const ButtonConfiguration& buttonConfig, TiXmlElement* pElement)
{
  ButtonConfiguration defaultConfig{ };
  if (!(buttonConfig == defaultConfig))
  {
    TiXmlElement buttonElement(BUTTONMAP_XML_ELEM_BUTTON);
    TiXmlNode* buttonNode = pElement->InsertEndChild(buttonElement);
    if (buttonNode == nullptr)
      return false;

    TiXmlElement* buttonElem = buttonNode->ToElement();
    if (buttonElem == nullptr)
      return false;

    buttonElem->SetAttribute(BUTTONMAP_XML_ATTR_DRIVER_INDEX, index);

    if (buttonConfig.bIgnore)
      buttonElem->SetAttribute(BUTTONMAP_XML_ATTR_IGNORE, "true");
  }

  return true;
}

bool CDeviceXml::DeserializeAxis(const TiXmlElement* pElement, unsigned int& index, AxisConfiguration& axisConfig)
{
  AxisConfiguration config{ };

  const char* strIndex = pElement->Attribute(BUTTONMAP_XML_ATTR_DRIVER_INDEX);
  if (!strIndex)
  {
    esyslog("<%s> tag has no \"%s\" attribute", BUTTONMAP_XML_ELEM_AXIS, BUTTONMAP_XML_ATTR_DRIVER_INDEX);
    return false;
  }
  index = std::atoi(strIndex);

  const char* center = pElement->Attribute(BUTTONMAP_XML_ATTR_AXIS_CENTER);
  if (center)
    config.trigger.center = std::atoi(center);

  const char* range = pElement->Attribute(BUTTONMAP_XML_ATTR_AXIS_RANGE);
  if (range)
    config.trigger.range = std::atoi(range);

  const char* ignore = pElement->Attribute(BUTTONMAP_XML_ATTR_IGNORE);
  if (ignore)
    config.bIgnore = (std::string(ignore) == "true");

  axisConfig = config;

  return true;
}

bool CDeviceXml::DeserializeButton(const TiXmlElement* pElement, unsigned int& index, ButtonConfiguration& buttonConfig)
{
  ButtonConfiguration config{ };

  const char* strIndex = pElement->Attribute(BUTTONMAP_XML_ATTR_DRIVER_INDEX);
  if (!strIndex)
  {
    esyslog("<%s> tag has no \"%s\" attribute", BUTTONMAP_XML_ELEM_AXIS, BUTTONMAP_XML_ATTR_DRIVER_INDEX);
    return false;
  }
  index = std::atoi(strIndex);

  const char* ignore = pElement->Attribute(BUTTONMAP_XML_ATTR_IGNORE);
  if (ignore)
    config.bIgnore = (std::string(ignore) == "true");

  buttonConfig = config;

  return true;
}
