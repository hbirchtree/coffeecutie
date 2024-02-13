find_package(pvrscope CONFIG REQUIRED)

coffee_library(
    TARGET PVRComponents
    SOURCES private/pvr_components.cpp
    LIBRARIES ComponentApp pvrscope::PVRScopeDeveloper
    HEADER_DIRS $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:include>
)
target_compile_definitions(PVRComponents PUBLIC -DGLEAM_ENABLE_PVRSCOPE)
