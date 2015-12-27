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
 *
 */

#include "StringUtils.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <regex>

using namespace JOYSTICK;

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

std::string& StringUtils::RemoveMACAddress(std::string& str)
{
  std::regex mac("[\\(\\[]?([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})[\\)\\]]?");
  str = std::regex_replace(str, mac, "");
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
