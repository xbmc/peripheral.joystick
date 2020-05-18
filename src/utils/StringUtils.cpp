/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "StringUtils.h"

#include <algorithm>
#include <iterator>
#include <cctype>
#include <functional>
#include <regex>
#include <stdio.h>
#include <stdlib.h>

using namespace JOYSTICK;

#define FORMAT_BLOCK_SIZE  512 // # of bytes for initial allocation for printf

// --- isspace_c() -------------------------------------------------------------

// Hack to check only first byte of UTF-8 character
// without this hack "TrimX" functions failed on Win32 and OS X with UTF-8 strings
static int isspace_c(char c)
{
  return (c & 0x80) == 0 && std::isspace(c);
}

// --- StringUtils -------------------------------------------------------------

std::string StringUtils::MakeSafeUrl(const std::string& str)
{
  std::string safeUrl;

  safeUrl.reserve(str.size());

  std::transform(str.begin(), str.end(), std::back_inserter(safeUrl),
    [](char c)
    {
      if (('a' <= c && c <= 'z') ||
          ('A' <= c && c <= 'Z') ||
          ('0' <= c && c <= '9') ||
           c == '-' ||
           c == '.' ||
           c == '_' ||
           c == '~')
      {
        return c;
      }
      return '_';
    });

  return safeUrl;
}

std::string StringUtils::MakeSafeString(std::string str)
{
  std::transform(str.begin(), str.end(), str.begin(),
    [](char c)
    {
      if (c < 0x20)
        return ' ';

      return c;
    });

  return str;
}

std::string StringUtils::RemoveMACAddress(std::string& str)
{
  std::regex re(R"mac([\(\[]?([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})[\)\]]?)mac");
  return std::regex_replace(str, re, "", std::regex_constants::format_default);
}

std::string& StringUtils::Trim(std::string& str)
{
  return TrimRight(TrimLeft(str));
}

std::string& StringUtils::TrimLeft(std::string& str)
{
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun(isspace_c))));
  return str;
}

std::string& StringUtils::TrimRight(std::string& str)
{
  str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun(isspace_c))).base(), str.end());
  return str;
}

std::string& StringUtils::Trim(std::string& str, const char* chars)
{
  return TrimRight(TrimLeft(str, chars), chars);
}

std::string& StringUtils::TrimLeft(std::string& str, const char* chars)
{
  size_t nidx = str.find_first_not_of(chars);
  str.erase(0, nidx);
  return str;
}

std::string& StringUtils::TrimRight(std::string& str, const char* chars)
{
  size_t nidx = str.find_last_not_of(chars);
  str.erase(nidx == str.npos ? 0 : ++nidx);
  return str;
}

bool StringUtils::EndsWith(const std::string& str, const std::string& suffix)
{
  if (str.length() >= suffix.length())
    return str.substr(str.length() - suffix.length()) == suffix;
  return false;
}

std::string StringUtils::Format(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  std::string str = FormatV(fmt, args);
  va_end(args);

  return str;
}

std::string StringUtils::FormatV(const char* fmt, va_list args)
{
  if (!fmt || !fmt[0])
    return "";

  int size = FORMAT_BLOCK_SIZE;
  va_list argCopy;

  while (1)
  {
    char* cstr = static_cast<char*>(malloc(sizeof(char) * size));
    if (!cstr)
      return "";

    va_copy(argCopy, args);
    int nActual = vsnprintf(cstr, size, fmt, argCopy);
    va_end(argCopy);

    if (nActual > -1 && nActual < size) // We got a valid result
    {
      std::string str(cstr, nActual);
      free(cstr);
      return str;
    }
    free(cstr);
#ifndef __WIN32__
    if (nActual > -1)                   // Exactly what we will need (glibc 2.1)
      size = nActual + 1;
    else                                // Let's try to double the size (glibc 2.0)
      size *= 2;
#else  // __WIN32__
    va_copy(argCopy, args);
    size = _vscprintf(fmt, argCopy);
    va_end(argCopy);
    if (size < 0)
      return "";
    else
      size++; // increment for null-termination
#endif // __WIN32__
  }

  return ""; // unreachable
}
