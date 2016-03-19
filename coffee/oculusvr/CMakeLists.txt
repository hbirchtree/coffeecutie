find_package(OVR)

if(NOT OVR_FOUND)
    message(FATAL_ERROR "Failed to locate OVR libraries")
endif()

include_directories ( SYSTEM
    ${OVR_INCLUDE_DIR}
    )

set ( VR_ADDITIONAL_LIBS )

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    list(APPEND VR_ADDITIONAL_LIBS dl)
endif()

coffee_add_library ( CoffeeOVR
    private/oculusrift.cpp)
target_link_libraries ( CoffeeOVR
    CoffeeCore
    ${OVR_LIBRARY}
    ${VR_ADDITIONAL_LIBS}
    )