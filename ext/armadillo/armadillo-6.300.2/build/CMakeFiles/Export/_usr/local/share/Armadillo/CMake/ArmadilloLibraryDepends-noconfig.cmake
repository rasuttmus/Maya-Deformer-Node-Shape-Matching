#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "armadillo" for configuration ""
set_property(TARGET armadillo APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(armadillo PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_NOCONFIG "-framework Accelerate"
  IMPORTED_LOCATION_NOCONFIG "/usr/local/lib/libarmadillo.6.30.2.dylib"
  IMPORTED_SONAME_NOCONFIG "libarmadillo.6.dylib"
  )

list(APPEND _IMPORT_CHECK_TARGETS armadillo )
list(APPEND _IMPORT_CHECK_FILES_FOR_armadillo "/usr/local/lib/libarmadillo.6.30.2.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
