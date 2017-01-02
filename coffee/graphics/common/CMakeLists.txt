coffee_add_library ( CoffeeGraphicsAPI
    "private/nullrenderer.cpp;private/query/gpu_query.cpp"
    "CoffeeCore"
    "${CMAKE_SOURCE_DIR}/include/coffee/graphics_apis")

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

find_package( NvidiaML QUIET )

if(NVIDIAML_FOUND)
    set_property(TARGET nvidia-ml PROPERTY IMPORTED_LOCATION "${NVIDIAML_LIBRARIES}" )
    include_directories( "${NVIDIAML_INCLUDE_DIR}" )

    coffee_add_elibrary ( "coffee-nvidia" SHARED
        "private/query/nvidia_query.cpp" "${NVIDIAML_LIBRARIES}"
        "${CMAKE_SOURCE_DIR}/include/coffee/graphics/common" )
endif()

#coffee_add_elibrary ( "coffee-amd" SHARED "" )
#coffee_add_elibrary ( "coffee-intel" SHARED "" )
