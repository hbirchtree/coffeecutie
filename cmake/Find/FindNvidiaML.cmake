set ( NVIDIAML_SEARCH_PATHS
    /usr
    /usr/local
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0"
    )

find_path ( NVIDIAML_INCLUDE_DIR
    nvml.h
    PATH_SUFFIXES include/nvidia/gdk/ include/
    PATHS ${NVIDIAML_SEARCH_PATHS}
    )

find_library ( NVIDIAML_LIB_TMP
    NAMES nvidia-ml nvml
    PATH_SUFFIXES src/gdk/nvml/lib/ "lib/${CMAKE_LIBRARY_ARCHITECTURE_SDL}/"
    PATHS ${NVIDIAML_SEARCH_PATHS}
    )

if(NVIDIAML_LIB_TMP)
    set ( NVIDIAML_LIBRARIES "${NVIDIAML_LIB_TMP}" CACHE FILEPATH "" )
    set ( NVIDIAML_FOUND TRUE )
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(NvidiaML REQUIRED_VARS NVIDIAML_LIBRARIES NVIDIAML_INCLUDE_DIR)
