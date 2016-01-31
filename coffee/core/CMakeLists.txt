include_directories(
    ${COFFEE_CORE_INCLUDE_DIR}
    )

find_package(SDL2 QUIET)

if(NOT WIN32)
    #Libunwind is used to print function names at runtime
    if(NOT ANDROID) #Bionic does not grant access to unwind, even though it is there
        find_package(Unwind)
        include_directories(${LIBUNWIND_INCLUDE_DIR})
    endif()
endif()

set ( CoffeeCore_SRCS

    private/coffee.cpp
    private/base/counter.cpp
    private/base/cdebug.cpp
    private/base/cdisplay.cpp
    private/base/cfiles.cpp
    private/base/cfilewatcher.cpp
    private/base/cobject.cpp
    private/base/cregex.cpp

    private/base/cdrendererbase.cpp

    private/functional/cqueuerunner.cpp

    private/jobs/ctasksystem.cpp

    private/unit_tests/data_types.cpp

    private/input/cinputfunctions.cpp

    private/plat/argument_parse.cpp
    private/plat/environment_details.cpp
    private/plat/cmemory.cpp
    private/plat/plat_file.cpp
    private/plat/linking/libraries.cpp
    private/plat/plat_timers.cpp
    private/plat/windows/win_core.cpp
    private/plat/unix/unix_core.cpp

    ${CMAKE_SOURCE_DIR}/libs/tinyxml2/tinyxml2.cpp
    )

set(DL_LIBRARY "")
if(NOT WIN32)
    list(APPEND DL_LIBRARY dl m)
    if(NOT ANDROID)
        list(APPEND DL_LIBRARY pthread)
    endif()
endif()

coffee_add_library ( CoffeeCore "${CoffeeCore_SRCS}" )

target_link_libraries ( CoffeeCore
    ${LIBUNWIND_LIBRARIES}
    ${DL_LIBRARY}
    )

if(ANDROID AND SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIR})
    target_link_libraries(CoffeeCore
        ${SDL2_LIBRARY})
endif()

if(WIN32)
    target_link_libraries ( CoffeeCore
        pathcch
        )
endif()
