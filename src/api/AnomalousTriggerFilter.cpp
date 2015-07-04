/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 */

#include "AnomalousTriggerFilter.h"
#include "log/Log.h"

using namespace JOYSTICK;

#define ANOMOLOUS_MAGNITUDE  0.5f

CAnomalousTriggerFilter::CAnomalousTriggerFilter(unsigned int axisIndex)
  : axisIndex(axisIndex),
    m_state(STATE_IDLE),
    m_center(CENTER_ZERO),
    m_range(TRIGGER_RANGE_HALF)
{
}

float CAnomalousTriggerFilter::Filter(float value)
{
  if (m_state == STATE_IDLE)
  {
    if (value != 0.0f)
    {
      if (value < -ANOMOLOUS_MAGNITUDE)
        m_center = CENTER_NEGATIVE_ONE;
      else if (value > ANOMOLOUS_MAGNITUDE)
        m_center = CENTER_POSITIVE_ONE;
      else
        m_center = CENTER_ZERO;

      if (IsAnomalousTrigger())
        dsyslog("Anomalous trigger detected on axis %u (initial value = %f)", axisIndex, value);
    }
    m_state = STATE_CENTER_KNOWN;
  }

  if (IsAnomalousTrigger())
  {
    // Adjust range if value enters opposite semiaxis
    if (m_state == STATE_CENTER_KNOWN)
    {
      if (m_center == CENTER_NEGATIVE_ONE && value > 0.0f)
      {
        m_range = TRIGGER_RANGE_FULL;
        m_state = STATE_RANGE_KNOWN;
      }
      else if (m_center == CENTER_POSITIVE_ONE && value < 0.0f)
      {
        m_range = TRIGGER_RANGE_FULL;
        m_state = STATE_RANGE_KNOWN;
      }
    }

    // Translate center to zero
    value = value - GetCenter(m_center);

    // Adjust range to lie in an interval of length 1
    if (m_range == TRIGGER_RANGE_FULL)
      value /= 2;
  }

  return value;
}

bool CAnomalousTriggerFilter::IsAnomalousTrigger(void)
{
  return m_center != CENTER_ZERO;
}

float CAnomalousTriggerFilter::GetCenter(AXIS_CENTER center)
{
  switch (center)
  {
    case CENTER_NEGATIVE_ONE: return -1.0f;
    case CENTER_POSITIVE_ONE: return  1.0f;
    default:
      break;
  }
  return 0.0f;
}
