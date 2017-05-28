set(ASSIMP_ROOT_DIR "${ASSIMP_ROOT_DIR}"
    CACHE PATH "Directory to search for Assimp")

FIND_PATH( ASSIMP_INCLUDE_DIR
  assimp/Importer.hpp
  assimp/importer.hpp
  PATHS
  ${CMAKE_BINARY_DIR}/libs/
  ${COFFEE_EXT_LIBRARY_DIR}/assimp/
  ${NATIVE_LIBRARY_DIR}
  ${COFFEE_ROOT_DIR}
  PATH_SUFFIXES include include/assimp
)

FIND_LIBRARY(
  ASSIMP_LIBRARIES
  NAMES assimp
  PATHS ${CMAKE_BINARY_DIR}/libs/lib/ ${COFFEE_ROOT_DIR}
  PATH_SUFFIXES
  64/link lib lib64
  lib/${CMAKE_LIBRARY_ARCHITECTURE} # CMake architecture path
)

IF (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARIES)
  SET(ASSIMP_FOUND TRUE)
ENDIF (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARIES)


INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(assimp REQUIRED_VARS ASSIMP_LIBRARIES ASSIMP_INCLUDE_DIR)
