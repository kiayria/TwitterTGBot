#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "networklib" for configuration "Debug"
set_property(TARGET networklib APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(networklib PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libnetworklib.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS networklib )
list(APPEND _IMPORT_CHECK_FILES_FOR_networklib "${_IMPORT_PREFIX}/lib/libnetworklib.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
