/*
 *      Copyright (C) 2014-2020 Garrett Brown
 *      Copyright (C) 2014-2020 Team Kodi
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

#include "JoystickInterfaceLinux.h"
#include "JoystickLinux.h"
#include "api/JoystickTypes.h"
#include "log/Log.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace JOYSTICK;

EJoystickInterface CJoystickInterfaceLinux::Type(void) const
{
  return EJoystickInterface::LINUX;
}

bool CJoystickInterfaceLinux::ScanForJoysticks(JoystickVector& joysticks)
{
  // TODO: Use udev to grab device names instead of reading /dev/input/js*
  std::string inputDir("/dev/input");
  DIR *pd = opendir(inputDir.c_str());
  if (pd == NULL)
  {
    // Disabled until udev is used to grab device names
    //esyslog("%s: can't open %s (errno=%d)", __FUNCTION__, inputDir.c_str(), errno);
    return false;
  }

  dirent *pDirent;
  while ((pDirent = readdir(pd)) != NULL)
  {
    if (std::string(pDirent->d_name).substr(0, 2) == "js")
    {
      // Found a joystick device
      std::string filename(inputDir + "/" + pDirent->d_name);

      int fd = open(filename.c_str(), O_RDONLY);
      if (fd < 0)
      {
        esyslog("%s: can't open %s (errno=%d)", __FUNCTION__, filename.c_str(), errno);
        continue;
      }

      unsigned char axes      = 0;
      unsigned char buttons   = 0;
      int           version   = 0x000000;
      char          name[128] = { };

      if (ioctl(fd, JSIOCGVERSION, &version) < 0 ||
          ioctl(fd, JSIOCGAXES, &axes)       < 0 ||
          ioctl(fd, JSIOCGBUTTONS, &buttons) < 0 ||
          ioctl(fd, JSIOCGNAME(128), name)   < 0)
      {
        esyslog("%s: failed ioctl() (errno=%d)", __FUNCTION__, errno);
        close(fd);
        continue;
      }

      if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
      {
        esyslog("%s: failed fcntl() (errno=%d)", __FUNCTION__, errno);
        close(fd);
        continue;
      }

      // We don't support the old (0.x) interface
      if (version < 0x010000)
      {
        esyslog("%s: old (0.x) interface is not supported (version=%08x)", __FUNCTION__, version);
        close(fd);
        continue;
      }

      unsigned int index = (unsigned int)std::max(strtol(pDirent->d_name + strlen("js"), NULL, 10), 0L);

      JoystickPtr joystick = JoystickPtr(new CJoystickLinux(fd, filename));
      joystick->SetName(name);
      joystick->SetButtonCount(buttons);
      joystick->SetAxisCount(axes);
      joystick->SetRequestedPort(index);
      joysticks.push_back(joystick);
    }
  }

  closedir(pd);

  return true;
}
