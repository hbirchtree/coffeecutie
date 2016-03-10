set ( COFFEE_SEARCH_PATHS
    /usr/local
    /usr
    /opt

    ~/Library/Frameworks
    /Library/Frameworks
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    )

find_path ( COFFEE_INCLUDE_DIR
    coffee/core/coffee.h
    coffee/CCore

    PATHS
    ${COFFEE_ROOT_DIR}
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    include
    )

find_library ( COFFEE_CORE_LIBRARY
    CoffeeCore

    PATHS
    ${COFFEE_ROOT_DIR}
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    lib
    lib64
    )

find_library ( COFFEE_OCULUS_LIBRARY
    CoffeeOVR
    CoffeeOculusRift
    CoffeeOculusVR

    PATHS
    ${COFFEE_ROOT_DIR}
    ${COFFEE_SEARCH_PATHS}

    PATH_SUFFIXES
    lib
    lib64
    )