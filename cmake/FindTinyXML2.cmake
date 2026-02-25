# FindTinyXML2
# -----------
# Finds the TinyXML2 library
#
# This will define the following variables:
#
# TINYXML2_FOUND - system has TinyXML2
#
# and the following imported targets:
#
#   tinyxml2::tinyxml2 - The UDEV library

find_package(TinyXML2 CONFIG)

if(TinyXML2_FOUND)
  set(TINYXML2_FOUND TinyXML2_FOUND)
elseif(NOT TinyXML2_FOUND)
  find_package(PkgConfig)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_TINYXML2 tinyxml2 QUIET)
  endif()

  find_path(TINYXML2_INCLUDE_DIR tinyxml2.h
                                 PATHS ${PC_TINYXML2_INCLUDEDIR})
  find_library(TINYXML2_LIBRARY NAMES tinyxml2
                                PATHS ${PC_TINYXML2_LIBDIR})
  set(TINYXML2_VERSION ${PC_TINYXML2_VERSION})

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(TinyXML2
                                    REQUIRED_VARS TINYXML2_LIBRARY TINYXML2_INCLUDE_DIR
                                    VERSION_VAR TINYXML2_VERSION)

  if(TINYXML2_FOUND)
    add_library(tinyxml2::tinyxml2 UNKNOWN IMPORTED)
    set_target_properties(tinyxml2::tinyxml2 PROPERTIES
                          IMPORTED_LOCATION "${TINYXML2_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${TINYXML2_INCLUDE_DIR}")
  endif()

  mark_as_advanced(TINYXML2_INCLUDE_DIR TINYXML2_LIBRARY)
endif()
