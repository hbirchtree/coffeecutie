find_package(assimp)

if(NOT DEFINED assimp_LIBRARIES OR NOT DEFINED assimp_INCLUDE_DIRS)
    message(FATAL_ERROR "Failed to locate Assimp")
endif()

set ( CoffeeAssimp_SRCS
        private/cassimpimporters.cpp
        private/assimpfun.cpp
        )

include_directories ( SYSTEM
    ${ASSIMP_INCLUDE_DIRS}
    )

coffee_add_library ( CoffeeAssimp "${CoffeeAssimp_SRCS}" )

target_link_libraries ( CoffeeAssimp
    CoffeeCore
    ${assimp_LIBRARIES}
    )
