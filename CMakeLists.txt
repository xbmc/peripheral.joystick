cmake_minimum_required(VERSION 3.1)
project(peripheral.joystick)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(CheckIncludeFiles)

# --- Add-on Dependencies ------------------------------------------------------

find_package(Kodi REQUIRED)
find_package(kodiplatform REQUIRED)
find_package(p8-platform REQUIRED)
find_package(TinyXML REQUIRED)

include_directories(${INCLUDES}
                    ${PROJECT_SOURCE_DIR}/src
                    ${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)
                    ${kodiplatform_INCLUDE_DIRS}
                    ${p8-platform_INCLUDE_DIRS}
                    ${TINYXML_INCLUDE_DIRS})

set(JOYSTICK_SOURCES src/addon.cpp
                     src/api/IJoystickInterface.cpp
                     src/api/Joystick.cpp
                     src/api/JoystickInterfaceCallback.cpp
                     src/api/JoystickManager.cpp
                     src/api/JoystickTranslator.cpp
                     src/api/JoystickUtils.cpp
                     src/api/PeripheralScanner.cpp
                     src/buttonmapper/ButtonMapper.cpp
                     src/buttonmapper/ButtonMapTranslator.cpp
                     src/buttonmapper/ButtonMapUtils.cpp
                     src/buttonmapper/ControllerTransformer.cpp
                     src/buttonmapper/DriverGeometry.cpp
                     src/buttonmapper/JoystickFamily.cpp
                     src/buttonmapper/StringRegistry.cpp
                     src/filesystem/DirectoryCache.cpp
                     src/filesystem/DirectoryUtils.cpp
                     src/filesystem/Filesystem.cpp
                     src/filesystem/FileUtils.cpp
                     src/filesystem/generic/ReadableFile.cpp
                     src/filesystem/generic/SeekableFile.cpp
                     src/filesystem/vfs/VFSDirectoryUtils.cpp
                     src/filesystem/vfs/VFSFileUtils.cpp
                     src/log/Log.cpp
                     src/log/LogAddon.cpp
                     src/log/LogConsole.cpp
                     src/settings/Settings.cpp
                     src/storage/ButtonMap.cpp
                     src/storage/Device.cpp
                     src/storage/DeviceConfiguration.cpp
                     src/storage/JustABunchOfFiles.cpp
                     src/storage/MouseTranslator.cpp
                     src/storage/StorageManager.cpp
                     src/storage/StorageUtils.cpp
                     src/storage/api/DatabaseJoystickAPI.cpp
                     src/storage/xml/ButtonMapXml.cpp
                     src/storage/xml/DatabaseXml.cpp
                     src/storage/xml/DeviceXml.cpp
                     src/storage/xml/JoystickFamiliesXml.cpp
                     src/utils/StringUtils.cpp)

set(JOYSTICK_HEADERS src/addon.h
                     src/api/IJoystickInterface.h
                     src/api/Joystick.h
                     src/api/JoystickInterfaceCallback.h
                     src/api/JoystickManager.h
                     src/api/JoystickTranslator.h
                     src/api/JoystickTypes.h
                     src/api/PeripheralScanner.h
                     src/buttonmapper/ButtonMapper.h
                     src/buttonmapper/ButtonMapTranslator.h
                     src/buttonmapper/ButtonMapTypes.h
                     src/buttonmapper/ButtonMapUtils.h
                     src/buttonmapper/ControllerTransformer.h
                     src/buttonmapper/DriverGeometry.h
                     src/buttonmapper/JoystickFamily.h
                     src/buttonmapper/StringRegistry.h
                     src/filesystem/DirectoryCache.h
                     src/filesystem/DirectoryUtils.h
                     src/filesystem/Filesystem.h
                     src/filesystem/FilesystemTypes.h
                     src/filesystem/FileUtils.h
                     src/filesystem/IDirectoryUtils.h
                     src/filesystem/IFile.h
                     src/filesystem/IFileUtils.h
                     src/filesystem/generic/ReadableFile.h
                     src/filesystem/generic/SeekableFile.h
                     src/filesystem/vfs/VFSDirectoryUtils.h
                     src/filesystem/vfs/VFSFileUtils.h
                     src/log/ILog.h
                     src/log/LogAddon.h
                     src/log/LogConsole.h
                     src/log/Log.h
                     src/settings/Settings.h
                     src/storage/ButtonMap.h
                     src/storage/DeviceConfiguration.h
                     src/storage/Device.h
                     src/storage/IDatabase.h
                     src/storage/JustABunchOfFiles.h
                     src/storage/MouseTranslator.h
                     src/storage/PrimitiveConfiguration.h
                     src/storage/StorageDefinitions.h
                     src/storage/StorageManager.h
                     src/storage/StorageTypes.h
                     src/storage/StorageUtils.h
                     src/storage/api/DatabaseJoystickAPI.h
                     src/storage/xml/ButtonMapDefinitions.h
                     src/storage/xml/ButtonMapXml.h
                     src/storage/xml/DatabaseXml.h
                     src/storage/xml/DeviceXml.h
                     src/storage/xml/JoystickFamiliesXml.h
                     src/storage/xml/JoystickFamilyDefinitions.h
                     src/utils/CommonIncludes.h
                     src/utils/CommonMacros.h
                     src/utils/StringUtils.h)

if(CORE_SYSTEM_NAME MATCHES windows)
  list(APPEND JOYSTICK_SOURCES src/utils/windows/CharsetConverter.cpp)
  list(APPEND JOYSTICK_HEADERS src/utils/windows/CharsetConverter.h)
endif()

check_include_files("syslog.h" HAVE_SYSLOG)

if(HAVE_SYSLOG)
  list(APPEND JOYSTICK_SOURCES src/log/LogSyslog.cpp)
  list(APPEND JOYSTICK_HEADERS src/log/LogSyslog.h)
endif()

list(APPEND DEPLIBS ${kodiplatform_LIBRARIES}
                    ${p8-platform_LIBRARIES} ${TINYXML_LIBRARIES})

# --- SDL2 ---------------------------------------------------------------------

# SDL game controller support only used by Steam Link
if(CORE_PLATFORM_NAME STREQUAL steamlink)
  find_package(Sdl2)

  if(SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})

    add_definitions(-DHAVE_SDL_GAMEPAD)

    list(APPEND JOYSTICK_SOURCES src/api/sdl/JoystickInterfaceSDL.cpp
                                 src/api/sdl/JoystickSDL.cpp)
    list(APPEND JOYSTICK_HEADERS src/api/sdl/JoystickInterfaceSDL.h
                                 src/api/sdl/JoystickSDL.h)

    list(APPEND DEPLIBS ${SDL2_LIBRARIES})
  endif()
endif()

# --- Cocoa --------------------------------------------------------------------

if("${CORE_SYSTEM_NAME}" STREQUAL "darwin" OR "${CORE_SYSTEM_NAME}" STREQUAL "osx")
  find_library(COCOA_LIBRARY Cocoa)

  add_definitions(-DHAVE_COCOA)

  list(APPEND JOYSTICK_SOURCES src/api/cocoa/JoystickCocoa.cpp
                               src/api/cocoa/JoystickInterfaceCocoa.cpp)
  list(APPEND JOYSTICK_HEADERS src/api/cocoa/JoystickCocoa.h
                               src/api/cocoa/JoystickInterfaceCocoa.h)

  list(APPEND DEPLIBS ${COCOA_LIBRARY})
  list(APPEND DEPLIBS "-framework IOKit")
endif()

# --- Linux Joystick API -------------------------------------------------------

if(CORE_SYSTEM_NAME STREQUAL linux)
  check_include_files(linux/joystick.h HAVE_LINUX_JOYSTICK_H)
endif()

if(HAVE_LINUX_JOYSTICK_H)
  add_definitions(-DHAVE_LINUX_JOYSTICK)

  list(APPEND JOYSTICK_SOURCES src/api/linux/JoystickInterfaceLinux.cpp
                               src/api/linux/JoystickLinux.cpp)
  list(APPEND JOYSTICK_HEADERS src/api/linux/JoystickInterfaceLinux.h
                               src/api/linux/JoystickLinux.h)
endif()

# --- DirectInput --------------------------------------------------------------

if("${CORE_SYSTEM_NAME}" STREQUAL "windows")
  add_definitions(-DHAVE_DIRECT_INPUT)

  list(APPEND JOYSTICK_SOURCES src/api/directinput/JoystickDirectInput.cpp
                               src/api/directinput/JoystickInterfaceDirectInput.cpp)
  list(APPEND JOYSTICK_HEADERS src/api/directinput/JoystickDirectInput.h
                               src/api/directinput/JoystickInterfaceDirectInput.h)
endif()

# --- XInput -------------------------------------------------------------------

if("${CORE_SYSTEM_NAME}" MATCHES "windows")
  add_definitions(-DHAVE_XINPUT)

  list(APPEND JOYSTICK_SOURCES src/api/xinput/JoystickInterfaceXInput.cpp
                               src/api/xinput/JoystickXInput.cpp
                               src/api/xinput/XInputDLL.cpp)
  list(APPEND JOYSTICK_HEADERS src/api/xinput/JoystickInterfaceXInput.h
                               src/api/xinput/JoystickXInput.h
                               src/api/xinput/XInputDLL.h)
endif()

# --- udev ---------------------------------------------------------------------

if(CORE_SYSTEM_NAME STREQUAL linux)
  find_package(udev REQUIRED)
endif()

if(UDEV_FOUND)
  include_directories(${UDEV_INCLUDE_DIRS})

  add_definitions(-DHAVE_UDEV)

  list(APPEND JOYSTICK_SOURCES src/api/udev/JoystickInterfaceUdev.cpp
                               src/api/udev/JoystickUdev.cpp)
  list(APPEND JOYSTICK_HEADERS src/api/udev/JoystickInterfaceUdev.h
                               src/api/udev/JoystickUdev.h)

  list(APPEND DEPLIBS ${UDEV_LIBRARIES})
endif()

# ------------------------------------------------------------------------------

set(LINUX_SELECT_LINE "\
        <setting id=\"driver_linux\" type=\"integer\" label=\"30001\">
          <default>0</default>
          <constraints>
            <options>
              <option label=\"30005\">0</option>
              <option label=\"30007\">1</option>
              <option label=\"30002\">2</option>
            </options>
          </constraints>
          <control type=\"list\" format=\"string\"/>
        </setting>")

set(SDL_SELECT_LINE "\
        <setting id=\"driver_sdl\" type=\"integer\" label=\"30001\">
          <default>0</default>
          <constraints>
            <options>
              <option label=\"30008\">0</option>
              <option label=\"30005\">1</option>
              <option label=\"30007\">2</option>
              <option label=\"30002\">3</option>
            </options>
          </constraints>
          <control type=\"list\" format=\"string\"/>
        </setting>")

set(OSX_SELECT_LINE  "\
        <setting id=\"driver_osx\" type=\"integer\" label=\"30001\">
          <default>0</default>
          <constraints>
            <options>
              <option label=\"30006\">0</option>
              <option label=\"30002\">1</option>
            </options>
          </constraints>
          <control type=\"list\" format=\"string\"/>
        </setting>")

set(XINPUT_CHECK_LINE "\
        <setting id=\"driver_xinput\" type=\"boolean\" label=\"30003\">
          <default>true</default>
          <control type=\"toggle\"/>
        </setting>")

set(DIRECTINPUT_CHECK_LINE "\
        <setting id=\"driver_directinput\" type=\"boolean\" label=\"30004\">
          <default>true</default>
          <control type=\"toggle\"/>
        </setting>")

# Write settings.xml.include
if(CORE_SYSTEM_NAME MATCHES windows)
  set(XINPUT_CHECK "${XINPUT_CHECK_LINE}")
  if(CORE_SYSTEM_NAME STREQUAL windows)
    set(DIRECTINPUT_CHECK "${DIRECTINPUT_CHECK_LINE}")
  endif()
elseif(CORE_SYSTEM_NAME STREQUAL osx)
  set(OSX_SELECT "${OSX_SELECT_LINE}")
else()
  if(SDL2_FOUND)
    set(SDL_SELECT "${SDL_SELECT_LINE}")
  else()
    set(LINUX_SELECT "${LINUX_SELECT_LINE}")
  endif()
endif()

# ------------------------------------------------------------------------------

build_addon(peripheral.joystick JOYSTICK DEPLIBS)

include(CPack)
