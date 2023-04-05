find_package ( spirv_cross_core CONFIG REQUIRED )
find_package ( spirv_cross_util CONFIG REQUIRED )
find_package ( spirv_cross_reflect CONFIG REQUIRED )
find_package ( spirv_cross_glsl CONFIG REQUIRED )
find_package ( spirv_cross_cpp CONFIG REQUIRED )

find_package ( unofficial-shaderc_util CONFIG REQUIRED )
find_package ( unofficial-shaderc CONFIG REQUIRED )

find_package ( glslang CONFIG REQUIRED )

find_package ( SPIRV-Headers CONFIG REQUIRED )
find_package ( SPIRV-Tools CONFIG REQUIRED )
find_package ( SPIRV-Tools-opt CONFIG REQUIRED )
find_package ( SPIRV-Tools-link CONFIG REQUIRED )

find_package (Threads REQUIRED )

coffee_library(
    TARGET ShaderProcessor
    NO_COMPONENT_APP
    USE_CMD
    SOURCES
        private/compatibility_pass.cpp
        private/glslifier.cpp
        private/optimizer.cpp
        private/spvifier.cpp
    HEADER_DIRS
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${SPIRV-Headers_DIR}/../../../include/spirv/unified1>
    LIBRARIES
        PeripheralPlatform
        spirv-cross-cpp
        spirv-cross-glsl
        spirv-cross-reflect
        spirv-cross-util
        unofficial::shaderc::shaderc
        unofficial::shaderc_util::shaderc_util
        glslang::OGLCompiler
        glslang::glslang
        glslang::SPIRV
        glslang::SPVRemapper
        SPIRV-Headers::SPIRV-Headers
        SPIRV-Tools-link
        Threads::Threads
    )

coffee_application(
    TARGET ShaderCooker
    NO_COMPONENT_APP
    USE_CMD
    SOURCES
        cooker.cpp
    LIBRARIES
        CoreApplication
        ShaderProcessor
    )

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_options ( ShaderCooker PRIVATE
        -Wl,-rpath=../lib,-rpath=${CMAKE_BINARY_DIR}/lib
        )
endif()
