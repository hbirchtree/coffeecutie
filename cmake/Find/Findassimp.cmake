set(ASSIMP_ROOT_DIR "${ASSIMP_ROOT_DIR}"
    CACHE PATH "Directory to search for Assimp")

FIND_PATH( ASSIMP_INCLUDE_DIR
  assimp/Importer.hpp
  assimp/importer.hpp
  PATHS
  ${CMAKE_BINARY_DIR}/libs/
  ${COFFEE_EXT_LIBRARY_DIR}/assimp/
  PATH_SUFFIXES
  include
)

FIND_LIBRARY(
  ASSIMP_LIBRARIES
  NAMES assimp
  PATHS ${CMAKE_BINARY_DIR}/libs/lib/
)

IF (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARIES)
  SET(ASSIMP_FOUND TRUE)
ENDIF (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARIES)

IF (ASSIMP_FOUND)
  IF (NOT ASSIMP_FIND_QUIETLY)
    MESSAGE(STATUS "Found asset importer library: ${ASSIMP_LIBRARIES}")
  ENDIF (NOT ASSIMP_FIND_QUIETLY)
ELSE (ASSIMP_FOUND)
  IF (ASSIMP_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find asset importer library")
  ENDIF (ASSIMP_FIND_REQUIRED)
ENDIF (ASSIMP_FOUND)


INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(assimp REQUIRED_VARS ASSIMP_LIBRARIES ASSIMP_INCLUDE_DIR)
