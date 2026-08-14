/*
 *  Copyright (C) 2015-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <chrono>
#include <cstdint>

namespace JOYSTICK
{
/*!
 * \brief Milliseconds from a monotonic clock
 *
 * Only ever compared against other values from this function. steady_clock
 * rather than the wall clock, so the intervals stay correct if the system time
 * is adjusted.
 */
inline int64_t GetTimeMs()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
} // namespace JOYSTICK
