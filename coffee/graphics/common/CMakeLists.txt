coffee_add_library ( CoffeeGraphicsAPI
    "private/nullrenderer.cpp;private/query/gpu_query.cpp;private/scene/cnode.cpp"
    "CoffeeCore"
    "${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis")

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

find_package( NvidiaML QUIET )

find_package( LibDRM QUIET )

if(NVIDIAML_FOUND)
    include_directories( "${NVIDIAML_INCLUDE_DIR}" )

    coffee_add_elibrary ( "coffee-nvidia" SHARED
        "private/query/nvidia_query.cpp" "${NVIDIAML_LIBRARIES};CoffeeCore"
        "${CMAKE_SOURCE_DIR}/include/coffee/graphics/common/query" )
endif()

if(LIBDRM_FOUND)
    include_directories( "${LIBDRM_INCLUDE_DIR}" )

    coffee_add_elibrary ( "coffee-libdrm" SHARED
        "private/query/libdrm_query.cpp;private/query/libdrm_query.c"
        "${LIBDRM_LIBRARIES};CoffeeCore"
        "${CMAKE_SOURCE_DIR}/include/coffee/graphics/common/query")
endif()

#coffee_add_elibrary ( "coffee-amd" SHARED "" )
#coffee_add_elibrary ( "coffee-intel" SHARED "" )
