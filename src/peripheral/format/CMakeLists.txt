find_package ( fmt CONFIG REQUIRED )

coffee_library(
    TARGET PeripheralFormat
    SOURCES
        private/string_formatters.cpp
    LIBRARIES
        PeripheralLib
        fmt::fmt
    HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
