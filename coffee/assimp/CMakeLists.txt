find_package(assimp)

set ( CoffeeAssimp_SRCS
        private/cassimpimporters.cpp
        private/assimpfun.cpp
        )

include_directories(
        ${COFFEE_CORE_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

coffee_add_library ( CoffeeAssimp "${CoffeeAssimp_SRCS}" )

target_link_libraries ( CoffeeAssimp
    CoffeeCore
    ${assimp_LIBRARIES}
    )
