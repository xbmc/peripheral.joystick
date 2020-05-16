/*
 *  Copyright (C) 2014-2020 Garrett Brown
 *  Copyright (C) 2014-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#if !defined(ASSERT)
  #if defined(DEBUG) || defined(_DEBUG)
    #include <assert.h>
    #define ASSERT(x)  assert(x)
  #else
    #define ASSERT(x)
  #endif
#endif

#if !defined(SAFE_DELETE)
  #define SAFE_DELETE(x)  do { delete (x); (x) = NULL; } while (0)
#endif

#if !defined(SAFE_DELETE_ARRAY)
  #define SAFE_DELETE_ARRAY(x)  do { delete[] (x); (x) = NULL; } while (0)
#endif

#if !defined(SAFE_RELEASE)
  #define SAFE_RELEASE(p)  do { if(p) { (p)->Release(); (p)=NULL; } } while (0)
#endif

#ifndef CONSTRAIN
  // Credit: https://stackoverflow.com/questions/8941262/constrain-function-port-from-arduino
  #define CONSTRAIN(amt, low, high)  ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif

#ifndef ARRAY_SIZE
  #define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef MIN
  #define MIN(x, y)  ((y) < (x) ? (y) : (x))
#endif

#ifndef MAX
  #define MAX(x, y)  ((y) > (x) ? (y) : (x))
#endif

#if defined _WIN32 || defined __CYGWIN__
  #define DLL_PRIVATE
#else
  #if __GNUC__ >= 4
    #define DLL_PRIVATE  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PRIVATE
  #endif
#endif
