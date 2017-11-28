find_package(assimp REQUIRED)

set ( CoffeeAssimp_SRCS
        private/cassimpimporters.cpp
        private/assimpfun.cpp
        )

coffee_add_library (
    CoffeeAssimp "${CoffeeAssimp_SRCS}"
    "Assimp"
    "${CMAKE_SOURCE_DIR}/include/coffee/assimp" )

coffee_bundle_includes ( ${ASSIMP_INCLUDE_DIR} )
coffee_bundle_library ( ${ASSIMP_LIBRARIES} )
if(${IRRXML_LIBRARIES})
    coffee_bundle_library ( ${IRRXML_LIBRARIES} )
endif()

target_include_directories ( CoffeeAssimp
    PUBLIC
    $<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIR}/..>
    $<INSTALL_INTERFACE:include/>
    )
