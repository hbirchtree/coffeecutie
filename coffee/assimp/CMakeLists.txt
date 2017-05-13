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

coffee_bundle_includes ( ${ASSIMP_INCLUDE_DIR}/assimp )
coffee_bundle_library ( ${ASSIMP_LIBRARIES} )
