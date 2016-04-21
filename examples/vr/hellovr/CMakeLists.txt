find_package ( SDL2 REQUIRED )
find_package ( OpenVR REQUIRED )

include_directories(
    "${SDL2_INCLUDE_DIR}"
    "${OPENVR_INCLUDE_DIR}"
    )

set ( SOURCES
    main.cpp
    shared/lodepng.cpp
    shared/Matrices.cpp
    shared/pathtools.cpp
    )

set ( LIBRARIES
    CoffeeCore
    CoffeeVR

    "${SDL2_LIBRARY}"
    "${OPENVR_LIBRARY}"
    )

coffee_add_example ( OpenVR_HelloVR "Hello VR" "${SOURCES}" "${LIBRARIES}" )
