/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ButtonMapXml.h"
#include "ButtonMapDefinitions.h"
#include "DeviceXml.h"
#include "buttonmapper/ButtonMapTranslator.h"
#include "storage/Device.h"
#include "storage/StorageManager.h"
#include "log/Log.h"

#include <tinyxml.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <string>

using namespace JOYSTICK;

CButtonMapXml::CButtonMapXml(const std::string& strResourcePath, IControllerHelper *controllerHelper) :
  CButtonMap(strResourcePath, controllerHelper)
{
}

CButtonMapXml::CButtonMapXml(const std::string& strResourcePath, const DevicePtr& device, IControllerHelper *controllerHelper) :
  CButtonMap(strResourcePath, device, controllerHelper)
{
}

bool CButtonMapXml::Load(void)
{
  TiXmlDocument xmlFile;
  if (!xmlFile.LoadFile(m_strResourcePath))
  {
    esyslog("Error opening %s: %s", m_strResourcePath.c_str(), xmlFile.ErrorDesc());
    return false;
  }

  TiXmlElement* pRootElement = xmlFile.RootElement();
  if (!pRootElement || pRootElement->NoChildren() || pRootElement->ValueStr() != BUTTONMAP_XML_ROOT)
  {
    esyslog("Can't find root <%s> tag", BUTTONMAP_XML_ROOT);
    return false;
  }

  const TiXmlElement* pDevice = pRootElement->FirstChildElement(BUTTONMAP_XML_ELEM_DEVICE);

  if (!pDevice)
  {
    esyslog("Can't find <%s> tag", BUTTONMAP_XML_ELEM_DEVICE);
    return false;
  }

  // Don't overwrite valid device
  if (!m_device->IsValid())
  {
    if (!CDeviceXml::Deserialize(pDevice, *m_device))
      return false;
  }

  const TiXmlElement* pController = pDevice->FirstChildElement(BUTTONMAP_XML_ELEM_CONTROLLER);

  if (!pController)
  {
    esyslog("Device \"%s\": can't find <%s> tag", m_device->Name().c_str(), BUTTONMAP_XML_ELEM_CONTROLLER);
    return false;
  }

  // For logging purposes
  unsigned int totalFeatureCount = 0;

  while (pController)
  {
    const char* id = pController->Attribute(BUTTONMAP_XML_ATTR_CONTROLLER_ID);
    if (!id)
    {
      esyslog("Device \"%s\": <%s> tag has no attribute \"%s\"", m_device->Name().c_str(),
              BUTTONMAP_XML_ELEM_CONTROLLER, BUTTONMAP_XML_ATTR_CONTROLLER_ID);
      return false;
    }

    FeatureVector features;
    if (!Deserialize(pController, features, id))
      return false;

    if (features.empty())
    {
      esyslog("Device \"%s\" has no features for controller %s", m_device->Name().c_str(), id);
    }
    else
    {
      totalFeatureCount += static_cast<unsigned int>(features.size());
      m_buttonMap[id] = std::move(features);
    }

    pController = pController->NextSiblingElement(BUTTONMAP_XML_ELEM_CONTROLLER);
  }

  dsyslog("Loaded device \"%s\" with %u controller profiles and %u total features", m_device->Name().c_str(), m_buttonMap.size(), totalFeatureCount);

  return true;
}

bool CButtonMapXml::Save(void) const
{
  TiXmlDocument xmlFile;

  TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
  xmlFile.LinkEndChild(decl);

  TiXmlElement rootElement(BUTTONMAP_XML_ROOT);
  TiXmlNode* root = xmlFile.InsertEndChild(rootElement);
  if (root == NULL)
    return false;

  TiXmlElement* pElem = root->ToElement();
  if (!pElem)
    return false;

  TiXmlElement deviceElement(BUTTONMAP_XML_ELEM_DEVICE);
  TiXmlNode* deviceNode = pElem->InsertEndChild(deviceElement);
  if (deviceNode == NULL)
    return false;

  TiXmlElement* deviceElem = deviceNode->ToElement();
  if (deviceElem == NULL)
    return false;

  CDeviceXml::Serialize(*m_device, deviceElem);

  if (!SerializeButtonMaps(deviceElem))
    return false;

  return xmlFile.SaveFile(m_strResourcePath);
}

bool CButtonMapXml::SerializeButtonMaps(TiXmlElement* pElement) const
{
  for (ButtonMap::const_iterator it = m_buttonMap.begin(); it != m_buttonMap.end(); ++it)
  {
    const ControllerID& controllerId = it->first;
    const FeatureVector& features = it->second;

    if (features.empty())
      continue;

    TiXmlElement profileElement(BUTTONMAP_XML_ELEM_CONTROLLER);
    TiXmlNode* profileNode = pElement->InsertEndChild(profileElement);
    if (profileNode == NULL)
      continue;

    TiXmlElement* profileElem = profileNode->ToElement();
    if (profileElem == NULL)
      continue;

    profileElem->SetAttribute(BUTTONMAP_XML_ATTR_CONTROLLER_ID, controllerId);

    Serialize(features, profileElem);
  }
  return true;
}

bool CButtonMapXml::Serialize(const FeatureVector& features, TiXmlElement* pElement) const
{
  if (pElement == NULL)
    return false;

  for (FeatureVector::const_iterator it = features.begin(); it != features.end(); ++it)
  {
    const kodi::addon::JoystickFeature& feature = *it;

    if (!IsValid(feature))
      continue;

    TiXmlElement featureElement(BUTTONMAP_XML_ELEM_FEATURE);
    TiXmlNode* featureNode = pElement->InsertEndChild(featureElement);
    if (featureNode == NULL)
      return false;

    TiXmlElement* featureElem = featureNode->ToElement();
    if (featureElem == NULL)
      return false;

    featureElem->SetAttribute(BUTTONMAP_XML_ATTR_FEATURE_NAME, feature.Name());

    switch (feature.Type())
    {
      case JOYSTICK_FEATURE_TYPE_SCALAR:
      {
        SerializePrimitive(featureElem, feature.Primitive(JOYSTICK_SCALAR_PRIMITIVE));

        break;
      }
      case JOYSTICK_FEATURE_TYPE_ANALOG_STICK:
      {
        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_ANALOG_STICK_UP), BUTTONMAP_XML_ELEM_UP))
          return false;

        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_ANALOG_STICK_DOWN), BUTTONMAP_XML_ELEM_DOWN))
          return false;

        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_ANALOG_STICK_RIGHT), BUTTONMAP_XML_ELEM_RIGHT))
          return false;

        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_ANALOG_STICK_LEFT), BUTTONMAP_XML_ELEM_LEFT))
          return false;

        break;
      }
      case JOYSTICK_FEATURE_TYPE_RELPOINTER:
      {
        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_RELPOINTER_UP), BUTTONMAP_XML_ELEM_UP))
          return false;

        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_RELPOINTER_DOWN), BUTTONMAP_XML_ELEM_DOWN))
          return false;

        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_RELPOINTER_RIGHT), BUTTONMAP_XML_ELEM_RIGHT))
          return false;

        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_RELPOINTER_LEFT), BUTTONMAP_XML_ELEM_LEFT))
          return false;

        break;
      }
      case JOYSTICK_FEATURE_TYPE_ACCELEROMETER:
      {
        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_ACCELEROMETER_POSITIVE_X), BUTTONMAP_XML_ELEM_POSITIVE_X))
          return false;

        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_ACCELEROMETER_POSITIVE_Y), BUTTONMAP_XML_ELEM_POSITIVE_Y))
          return false;

        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_ACCELEROMETER_POSITIVE_Z), BUTTONMAP_XML_ELEM_POSITIVE_Z))
          return false;

        break;
      }
      case JOYSTICK_FEATURE_TYPE_MOTOR:
      {
        SerializePrimitive(featureElem, feature.Primitive(JOYSTICK_MOTOR_PRIMITIVE));

        break;
      }
      case JOYSTICK_FEATURE_TYPE_WHEEL:
      {
        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_WHEEL_LEFT), BUTTONMAP_XML_ELEM_LEFT))
          return false;

        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_WHEEL_RIGHT), BUTTONMAP_XML_ELEM_RIGHT))
          return false;

        break;
      }
      case JOYSTICK_FEATURE_TYPE_THROTTLE:
      {
        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_THROTTLE_UP), BUTTONMAP_XML_ELEM_UP))
          return false;

        if (!SerializePrimitiveTag(featureElem, feature.Primitive(JOYSTICK_THROTTLE_DOWN), BUTTONMAP_XML_ELEM_DOWN))
          return false;

        break;
      }
      case JOYSTICK_FEATURE_TYPE_KEY:
      {
        SerializePrimitive(featureElem, feature.Primitive(JOYSTICK_KEY_PRIMITIVE));

        break;
      }
      default:
        break;
    }
  }

  return true;
}

bool CButtonMapXml::IsValid(const kodi::addon::JoystickFeature& feature)
{
  for (auto primitive : feature.Primitives())
  {
    if (primitive.Type() != JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN)
      return true;
  }
  return false;
}

bool CButtonMapXml::SerializePrimitiveTag(TiXmlElement* pElement, const kodi::addon::DriverPrimitive& primitive, const char* tagName)
{
  if (primitive.Type() != JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN)
  {
    if (pElement == NULL)
      return false;

    TiXmlElement primitiveElement(tagName);
    TiXmlNode* primitiveNode = pElement->InsertEndChild(primitiveElement);
    if (primitiveNode == NULL)
      return false;

    TiXmlElement* primitiveElem = primitiveNode->ToElement();
    if (primitiveElem == NULL)
      return false;

    SerializePrimitive(primitiveElem, primitive);
  }

  return true;
}

void CButtonMapXml::SerializePrimitive(TiXmlElement* pElement, const kodi::addon::DriverPrimitive& primitive)
{
  std::string strPrimitive = ButtonMapTranslator::ToString(primitive);
  if (!strPrimitive.empty())
  {
    switch (primitive.Type())
    {
      case JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON:
      {
        pElement->SetAttribute(BUTTONMAP_XML_ATTR_FEATURE_BUTTON, strPrimitive);
        break;
      }
      case JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION:
      {
        pElement->SetAttribute(BUTTONMAP_XML_ATTR_FEATURE_HAT, strPrimitive);
        break;
      }
      case JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS:
      {
        pElement->SetAttribute(BUTTONMAP_XML_ATTR_FEATURE_AXIS, strPrimitive);
        break;
      }
      case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR:
      {
        pElement->SetAttribute(BUTTONMAP_XML_ATTR_FEATURE_MOTOR, strPrimitive);
        break;
      }
      case JOYSTICK_DRIVER_PRIMITIVE_TYPE_KEY:
      {
        pElement->SetAttribute(BUTTONMAP_XML_ATTR_FEATURE_KEY, strPrimitive);
        break;
      }
      case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOUSE_BUTTON:
      {
        pElement->SetAttribute(BUTTONMAP_XML_ATTR_FEATURE_MOUSE, strPrimitive);
        break;
      }
      case JOYSTICK_DRIVER_PRIMITIVE_TYPE_RELPOINTER_DIRECTION:
      {
        pElement->SetAttribute(BUTTONMAP_XML_ATTR_FEATURE_AXIS, strPrimitive);
        break;
      }
      default:
        break;
    }
  }
}

bool CButtonMapXml::Deserialize(const TiXmlElement* pElement, FeatureVector& features, const std::string &controllerId) const
{
  const TiXmlElement* pFeature = pElement->FirstChildElement(BUTTONMAP_XML_ELEM_FEATURE);

  if (!pFeature)
  {
    esyslog("Can't find <%s> tag", BUTTONMAP_XML_ELEM_FEATURE);
    return false;
  }

  for ( ; pFeature != nullptr; pFeature = pFeature->NextSiblingElement(BUTTONMAP_XML_ELEM_FEATURE))
  {
    const char* name = pFeature->Attribute(BUTTONMAP_XML_ATTR_FEATURE_NAME);
    if (!name)
    {
      esyslog("<%s> tag has no \"%s\" attribute", BUTTONMAP_XML_ELEM_FEATURE, BUTTONMAP_XML_ATTR_FEATURE_NAME);
      return false;
    }
    std::string strName(name);

    // Check if the feature was already deserialized
    auto it = std::find_if(features.begin(), features.end(),
      [strName](const kodi::addon::JoystickFeature& feature)
      {
        return feature.Name() == strName;
      });

    if (it != features.end())
    {
      esyslog("Duplicate feature \"%s\" found, skipping", strName.c_str());
      continue;
    }

    const TiXmlElement* pUp = nullptr;
    const TiXmlElement* pDown = nullptr;
    const TiXmlElement* pRight = nullptr;
    const TiXmlElement* pLeft = nullptr;

    const TiXmlElement* pPositiveX = nullptr;
    const TiXmlElement* pPositiveY = nullptr;
    const TiXmlElement* pPositiveZ = nullptr;

    // Determine the feature type
    JOYSTICK_FEATURE_TYPE type;

    kodi::addon::DriverPrimitive primitive;
    if (DeserializePrimitive(pFeature, primitive))
    {
      type = JOYSTICK_FEATURE_TYPE_SCALAR;
    }
    else
    {
      pUp = pFeature->FirstChildElement(BUTTONMAP_XML_ELEM_UP);
      pDown = pFeature->FirstChildElement(BUTTONMAP_XML_ELEM_DOWN);
      pRight = pFeature->FirstChildElement(BUTTONMAP_XML_ELEM_RIGHT);
      pLeft = pFeature->FirstChildElement(BUTTONMAP_XML_ELEM_LEFT);

      if (pUp || pDown || pRight || pLeft)
      {
        type = m_controllerHelper->FeatureType(controllerId, strName);
      }
      else
      {
        pPositiveX = pFeature->FirstChildElement(BUTTONMAP_XML_ELEM_POSITIVE_X);
        pPositiveY = pFeature->FirstChildElement(BUTTONMAP_XML_ELEM_POSITIVE_Y);
        pPositiveZ = pFeature->FirstChildElement(BUTTONMAP_XML_ELEM_POSITIVE_Z);

        if (pPositiveX || pPositiveY || pPositiveZ)
        {
          type = JOYSTICK_FEATURE_TYPE_ACCELEROMETER;
        }
        else
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_FEATURE);
          return false;
        }
      }
    }

    kodi::addon::JoystickFeature feature(strName, type);

    // Deserialize according to type
    switch (type)
    {
      case JOYSTICK_FEATURE_TYPE_SCALAR:
      {
        feature.SetPrimitive(JOYSTICK_SCALAR_PRIMITIVE, primitive);
        break;
      }
      case JOYSTICK_FEATURE_TYPE_ANALOG_STICK:
      {
        kodi::addon::DriverPrimitive up;
        kodi::addon::DriverPrimitive down;
        kodi::addon::DriverPrimitive right;
        kodi::addon::DriverPrimitive left;

        bool bSuccess = true;

        if (pUp && !DeserializePrimitive(pUp, up))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_UP);
          bSuccess = false;
        }

        if (pDown && !DeserializePrimitive(pDown, down))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_DOWN);
          bSuccess = false;
        }

        if (pRight && !DeserializePrimitive(pRight, right))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_RIGHT);
          bSuccess = false;
        }

        if (pLeft && !DeserializePrimitive(pLeft, left))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_LEFT);
          bSuccess = false;
        }

        if (!bSuccess)
          return false;

        feature.SetPrimitive(JOYSTICK_ANALOG_STICK_UP, up);
        feature.SetPrimitive(JOYSTICK_ANALOG_STICK_DOWN, down);
        feature.SetPrimitive(JOYSTICK_ANALOG_STICK_RIGHT, right);
        feature.SetPrimitive(JOYSTICK_ANALOG_STICK_LEFT, left);

        break;
      }
      case JOYSTICK_FEATURE_TYPE_RELPOINTER:
      {
        kodi::addon::DriverPrimitive up;
        kodi::addon::DriverPrimitive down;
        kodi::addon::DriverPrimitive right;
        kodi::addon::DriverPrimitive left;

        bool bSuccess = true;

        if (pUp && !DeserializePrimitive(pUp, up))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_UP);
          bSuccess = false;
        }

        if (pDown && !DeserializePrimitive(pDown, down))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_DOWN);
          bSuccess = false;
        }

        if (pRight && !DeserializePrimitive(pRight, right))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_RIGHT);
          bSuccess = false;
        }

        if (pLeft && !DeserializePrimitive(pLeft, left))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_LEFT);
          bSuccess = false;
        }

        if (!bSuccess)
          return false;

        feature.SetPrimitive(JOYSTICK_RELPOINTER_UP, up);
        feature.SetPrimitive(JOYSTICK_RELPOINTER_DOWN, down);
        feature.SetPrimitive(JOYSTICK_RELPOINTER_RIGHT, right);
        feature.SetPrimitive(JOYSTICK_RELPOINTER_LEFT, left);

        break;
      }
      case JOYSTICK_FEATURE_TYPE_ACCELEROMETER:
      {
        kodi::addon::DriverPrimitive positiveX;
        kodi::addon::DriverPrimitive positiveY;
        kodi::addon::DriverPrimitive positiveZ;

        bool bSuccess = true;

        if (pPositiveX && !DeserializePrimitive(pPositiveX, positiveY))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_POSITIVE_X);
          bSuccess = false;
        }

        if (pPositiveY && !DeserializePrimitive(pPositiveY, positiveY))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_POSITIVE_Y);
          bSuccess = false;
        }

        if (pPositiveZ && !DeserializePrimitive(pPositiveZ, positiveZ))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_POSITIVE_Z);
          bSuccess = false;
        }

        if (!bSuccess)
          return false;

        feature.SetPrimitive(JOYSTICK_ACCELEROMETER_POSITIVE_X, positiveX);
        feature.SetPrimitive(JOYSTICK_ACCELEROMETER_POSITIVE_Y, positiveY);
        feature.SetPrimitive(JOYSTICK_ACCELEROMETER_POSITIVE_Z, positiveZ);

        break;
      }
      case JOYSTICK_FEATURE_TYPE_MOTOR:
      {
        feature.SetPrimitive(JOYSTICK_MOTOR_PRIMITIVE, primitive);
        break;
      }
      case JOYSTICK_FEATURE_TYPE_WHEEL:
      {
        kodi::addon::DriverPrimitive right;
        kodi::addon::DriverPrimitive left;

        bool bSuccess = true;

        if (pRight && !DeserializePrimitive(pRight, right))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_RIGHT);
          bSuccess = false;
        }

        if (pLeft && !DeserializePrimitive(pLeft, left))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_LEFT);
          bSuccess = false;
        }

        if (!bSuccess)
          return false;

        feature.SetPrimitive(JOYSTICK_WHEEL_RIGHT, right);
        feature.SetPrimitive(JOYSTICK_WHEEL_LEFT, left);

        break;
      }
      case JOYSTICK_FEATURE_TYPE_THROTTLE:
      {
        kodi::addon::DriverPrimitive up;
        kodi::addon::DriverPrimitive down;

        bool bSuccess = true;

        if (pUp && !DeserializePrimitive(pUp, up))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_UP);
          bSuccess = false;
        }

        if (pDown && !DeserializePrimitive(pDown, down))
        {
          esyslog("Feature \"%s\": <%s> tag is not a valid primitive", strName.c_str(), BUTTONMAP_XML_ELEM_DOWN);
          bSuccess = false;
        }

        if (!bSuccess)
          return false;

        feature.SetPrimitive(JOYSTICK_THROTTLE_UP, up);
        feature.SetPrimitive(JOYSTICK_THROTTLE_DOWN, down);

        break;
      }
      case JOYSTICK_FEATURE_TYPE_KEY:
      {
        feature.SetPrimitive(JOYSTICK_KEY_PRIMITIVE, primitive);
        break;
      }
      default:
        break;
    }

    features.push_back(feature);
  }

  return true;
}

bool CButtonMapXml::DeserializePrimitive(const TiXmlElement* pElement, kodi::addon::DriverPrimitive& primitive)
{
  std::vector<std::pair<const char*, JOYSTICK_DRIVER_PRIMITIVE_TYPE>> types = {
    { BUTTONMAP_XML_ATTR_FEATURE_BUTTON, JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON },
    { BUTTONMAP_XML_ATTR_FEATURE_HAT, JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION },
    { BUTTONMAP_XML_ATTR_FEATURE_AXIS, JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS }, // Overloaded for relative pointer
    { BUTTONMAP_XML_ATTR_FEATURE_MOTOR, JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR },
    { BUTTONMAP_XML_ATTR_FEATURE_KEY, JOYSTICK_DRIVER_PRIMITIVE_TYPE_KEY },
    { BUTTONMAP_XML_ATTR_FEATURE_MOUSE, JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOUSE_BUTTON },
  };

  for (const auto &it : types)
  {
    const char *attr = pElement->Attribute(it.first);
    if (attr != nullptr)
      primitive = ButtonMapTranslator::ToDriverPrimitive(attr, it.second);
  }

  return primitive.Type() != JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN;
}
