#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cblite" for configuration "MinSizeRel"
set_property(TARGET cblite APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(cblite PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/arm-linux-androideabi/libcblite.so"
  IMPORTED_SONAME_MINSIZEREL "libcblite.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS cblite )
list(APPEND _IMPORT_CHECK_FILES_FOR_cblite "${_IMPORT_PREFIX}/lib/arm-linux-androideabi/libcblite.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
