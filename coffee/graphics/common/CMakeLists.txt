coffee_add_library ( CoffeeGraphicsAPI
    "private/query/gpu_query.cpp"
    ""
    "${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis"
    )

target_link_libraries ( CoffeeGraphicsAPI
    PRIVATE
    CoffeeCore
    )

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

find_package( NvidiaML QUIET )

find_package( LibDRM QUIET )

if(NVIDIAML_FOUND)
    coffee_add_elibrary ( "coffee-nvidia" SHARED
        "private/query/nvidia_query.cpp" "${NVIDIAML_LIBRARIES};CoffeeCore"
        "${CMAKE_SOURCE_DIR}/include/coffee/graphics/common/query"
        )

    target_include_directories( "coffee-nvidia"
        PRIVATE
        $<BUILD_INTERFACE:${NVIDIAML_INCLUDE_DIR}>
        )
endif()

if(LIBDRM_FOUND AND FALSE)
    coffee_add_elibrary ( "coffee-libdrm" SHARED
        "private/query/libdrm_query.cpp;private/query/libdrm_query.c"
        "${LIBDRM_LIBRARIES};CoffeeCore"
        "${CMAKE_SOURCE_DIR}/include/coffee/graphics/common/query"
        )

    target_include_directories( "coffee-libdrm"
        PRIVATE
        $<BUILD_INTERFACE:${LIBDRM_INCLUDE_DIR}>
        )
endif()

#coffee_add_elibrary ( "coffee-amd" SHARED "" )
#coffee_add_elibrary ( "coffee-intel" SHARED "" )
