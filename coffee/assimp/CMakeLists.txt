find_package(assimp REQUIRED)

set ( CoffeeAssimp_SRCS
        private/cassimpimporters.cpp
        private/assimpfun.cpp
        )

include_directories ( SYSTEM
    ${ASSIMP_INCLUDE_DIR}
    )

coffee_add_library (
    CoffeeAssimp "${CoffeeAssimp_SRCS}"
    "CoffeeCore;${ASSIMP_LIBRARIES}"
    "${CMAKE_SOURCE_DIR}/include/coffee/assimp" )

install (
    DIRECTORY
    ${ASSIMP_INCLUDE_DIR}/assimp

    DESTINATION
    include
    )
install (
    FILES
    ${ASSIMP_LIBRARIES}

    DESTINATION
    lib
    )
