if(GAMECUBE)
    coffee_library (
        TARGET GeckoThreads
        LIBRARIES
        SOURCES
        stl_replacements/gecko/threads.cpp
        )
endif()

coffee_library (
    TARGET PeripheralThreadNaming
    LIBRARIES CoffeeCore_Base
    SOURCES
    stl_extensions/thread_naming.cpp
    )
