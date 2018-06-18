set ( CBlam_SRCS
        private/volta/cblam_bitm.cpp
        private/volta/cblam_map.cpp
        private/volta/cblam_mod2.cpp
        private/volta/cblam_scenario_loader.cpp
        )

coffee_library (
    TARGET CoffeeBlam
    SOURCES ${CBlam_SRCS}
    LIBRARIES CoffeeImage
    HEADER_DIRS ${CMAKE_SOURCE_DIR}/include/coffee/image
    )
