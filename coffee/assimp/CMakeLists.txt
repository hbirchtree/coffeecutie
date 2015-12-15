find_package(assimp)

set ( CAssimp_SRCS
        private/cassimpimporters.cpp
        private/assimpfun.cpp
        )

include_directories(
        ${COFFEE_CORE_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

add_library( CAssimp ${COFFEE_LINK_OPT} ${CAssimp_SRCS})

target_link_libraries ( CAssimp
    CoffeeCore
    ${assimp_LIBRARIES}
    )

set_property(TARGET CAssimp PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CAssimp PRIVATE cxx_variadic_templates)
