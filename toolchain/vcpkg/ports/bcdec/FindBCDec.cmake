include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
include(${CMAKE_ROOT}/Modules/SelectLibraryConfigurations.cmake)

if(NOT BCDec_INCLUDE_DIR)
    find_path(BCDec_INCLUDE_DIR NAMES bcdec.h PATHS ${BCDec_DIR} PATH_SUFFIXES include)
endif()

find_package_handle_standard_args(BCDec DEFAULT_MSG BCDec_INCLUDE_DIR)
mark_as_advanced(BCDec_INCLUDE_DIR)
