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

add_library( CoffeeAssimp ${COFFEE_LINK_OPT} ${CoffeeAssimp_SRCS})

target_link_libraries ( CoffeeAssimp
    CoffeeCore
    ${assimp_LIBRARIES}
    )

set_property(TARGET CoffeeAssimp PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeAssimp PRIVATE cxx_variadic_templates)
