find_package(assimp)

set ( CAssimp_SRCS
        cassimpimporters.cpp
        private/assimpfun.cpp
        )

include_directories(${ASSIMP_INCLUDE_DIRS})

add_library( CAssimp ${COFFEE_LINK_OPT} ${CAssimp_SRCS})

target_link_libraries ( CAssimp
CoffeeCore
${assimp_LIBRARIES}
)

target_compile_features(CAssimp PRIVATE cxx_variadic_templates)
