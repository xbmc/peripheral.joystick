project(peripheral.joystick)

cmake_minimum_required(VERSION 2.6)
enable_language(CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(CheckIncludeFiles)

# --- Add-on Dependencies ------------------------------------------------------

find_package(kodi REQUIRED)
find_package(kodiplatform REQUIRED)
find_package(p8-platform REQUIRED)

include_directories(${INCLUDES}
                    ${PROJECT_SOURCE_DIR}/src
                    ${KODI_INCLUDE_DIR}
                    ${kodiplatform_INCLUDE_DIRS}
                    ${p8-platform_INCLUDE_DIRS})


set(JOYSTICK_SOURCES src/addon.cpp
                     src/api/AnomalousTriggerFilter.cpp
                     src/api/Joystick.cpp
                     src/api/JoystickAsync.cpp
                     src/api/JoystickInterfaceCallback.cpp
                     src/api/JoystickManager.cpp
                     src/api/JoystickTranslator.cpp
                     src/api/PeripheralScanner.cpp
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
                     src/storage/ButtonMapTranslator.cpp
                     src/storage/Device.cpp
                     src/storage/JustABunchOfFiles.cpp
                     src/storage/StorageManager.cpp
                     src/storage/StorageUtils.cpp
                     src/storage/xml/ButtonMapXml.cpp
                     src/storage/xml/DatabaseXml.cpp
                     src/storage/xml/DeviceXml.cpp
                     src/utils/StringUtils.cpp)

check_include_files("syslog.h" HAVE_SYSLOG)

if(HAVE_SYSLOG)
  list(APPEND JOYSTICK_SOURCES src/log/LogSyslog.cpp)
endif()

list(APPEND DEPLIBS ${kodiplatform_LIBRARIES}
                    ${p8-platform_LIBRARIES})

# --- Cocoa --------------------------------------------------------------------

if("${CORE_SYSTEM_NAME}" STREQUAL "darwin" OR "${CORE_SYSTEM_NAME}" STREQUAL "osx")
  find_library(COCOA_LIBRARY Cocoa)

  add_definitions(-DHAVE_COCOA)

  list(APPEND JOYSTICK_SOURCES src/api/cocoa/JoystickCocoa.cpp
                               src/api/cocoa/JoystickInterfaceCocoa.cpp)

  list(APPEND DEPLIBS ${COCOA_LIBRARY})
endif()

# --- SDL ----------------------------------------------------------------------

if(NOT "${CORE_SYSTEM_NAME}" STREQUAL "windows")
  find_package(Sdl)

  if(SDL_FOUND)
    include_directories(${SDL_INCLUDE_DIRS})

    add_definitions(-DHAVE_SDL)

    list(APPEND JOYSTICK_SOURCES src/api/sdl/JoystickInterfaceSDL.cpp
                                 src/api/sdl/JoystickSDL.cpp)

    list(APPEND DEPLIBS ${SDL_LIBRARIES})
  endif()
endif()

# --- Linux Joystick API -------------------------------------------------------

check_include_files(linux/joystick.h HAVE_LINUX_JOYSTICK_H)

if(HAVE_LINUX_JOYSTICK_H)
  add_definitions(-DHAVE_LINUX_JOYSTICK)

  list(APPEND JOYSTICK_SOURCES src/api/linux/JoystickInterfaceLinux.cpp
                               src/api/linux/JoystickLinux.cpp)
endif()

# --- DirectInput --------------------------------------------------------------

if("${CORE_SYSTEM_NAME}" STREQUAL "windows")
  add_definitions(-DHAVE_DIRECT_INPUT)

  list(APPEND JOYSTICK_SOURCES src/api/directinput/JoystickDirectInput.cpp
                               src/api/directinput/JoystickInterfaceDirectInput.cpp)
endif()

# --- XInput -------------------------------------------------------------------

if("${CORE_SYSTEM_NAME}" STREQUAL "windows")
  add_definitions(-DHAVE_XINPUT)

  list(APPEND JOYSTICK_SOURCES src/api/xinput/JoystickInterfaceXInput.cpp
                               src/api/xinput/JoystickXInput.cpp
                               src/api/xinput/XInputDLL.cpp)
endif()

# ------------------------------------------------------------------------------

build_addon(peripheral.joystick JOYSTICK DEPLIBS)

include(CPack)
