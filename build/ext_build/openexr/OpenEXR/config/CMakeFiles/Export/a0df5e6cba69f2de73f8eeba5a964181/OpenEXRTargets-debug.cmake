#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "OpenEXR::IlmImf" for configuration "Debug"
set_property(TARGET OpenEXR::IlmImf APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(OpenEXR::IlmImf PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libIlmImf-2_5_d.a"
  )

list(APPEND _cmake_import_check_targets OpenEXR::IlmImf )
list(APPEND _cmake_import_check_files_for_OpenEXR::IlmImf "${_IMPORT_PREFIX}/lib/libIlmImf-2_5_d.a" )

# Import target "OpenEXR::IlmImfUtil" for configuration "Debug"
set_property(TARGET OpenEXR::IlmImfUtil APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(OpenEXR::IlmImfUtil PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libIlmImfUtil-2_5_d.a"
  )

list(APPEND _cmake_import_check_targets OpenEXR::IlmImfUtil )
list(APPEND _cmake_import_check_files_for_OpenEXR::IlmImfUtil "${_IMPORT_PREFIX}/lib/libIlmImfUtil-2_5_d.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
