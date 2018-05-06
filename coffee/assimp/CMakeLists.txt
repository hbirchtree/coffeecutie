if(NOT ASSIMP_FOUND)
    find_package(assimp REQUIRED)
endif()

set ( CoffeeAssimp_SRCS
        private/cassimpimporters.cpp
        )

coffee_library (
    TARGET CoffeeAssimp
    SOURCES ${CoffeeAssimp_SRCS}
    LIBRARIES Assimp
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/assimp
    )

coffee_bundle_includes ( ${ASSIMP_INCLUDE_DIR} )
coffee_bundle_library ( ${ASSIMP_LIBRARIES} )

if(DEFINED IRRXML_LIBRARIES)
    coffee_bundle_library ( ${IRRXML_LIBRARIES} )
endif()

target_include_directories ( CoffeeAssimp
    PUBLIC
    $<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIR}/..>
    $<INSTALL_INTERFACE:include/>
    )
