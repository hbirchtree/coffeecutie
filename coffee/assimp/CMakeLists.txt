if(NOT DEFINED ASSIMP_INCLUDE_DIR)
    set (ASSIMP_INCLUDE_DIR libs/assimp/include)
endif()

if(NOT DEFINED ASSIMP_LIBRARY_DIR)
    set (ASSIMP_LIBRARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib )
endif()

include_directories (
                    ${ASSIMP_INCLUDE_DIR}
                    )

find_library(ASSIMP_LIBRARY NAMES "assimp" PATHS ${ASSIMP_LIBRARY_DIR} )

message("Assimp location: ${ASSIMP_LIBRARY}")

set ( CAssimp_SRCS
        coffee/assimp/cassimpimporters.cpp
        coffee/assimp/assimpfun.cpp
        )

add_library( CAssimp ${COFFEE_LINK_OPT} ${CAssimp_SRCS})

target_link_libraries ( CAssimp
${ASSIMP_LIBRARY}
)

target_compile_features(CAssimp PRIVATE cxx_variadic_templates)
