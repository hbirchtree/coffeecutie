find_package(nvidia-gdk CONFIG REQUIRED)

coffee_library(
    TARGET NVMLComponents
    SOURCES private/nvml_comp.cpp
    LIBRARIES ComponentApp nvidia-gdk::NVML
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:include>
)
