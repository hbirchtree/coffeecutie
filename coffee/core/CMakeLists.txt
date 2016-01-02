find_package(glbinding)

set ( CoffeeCore_SRCS

        coffee.cpp
        base/counter.cpp
        base/cdebug.cpp
        base/cdisplay.cpp
        base/cfiles.cpp
        base/cfilewatcher.cpp
        base/cobject.cpp
        base/cregex.cpp

        base/cdrendererbase.cpp

        functional/cqueuerunner.cpp

        jobs/ctasksystem.cpp

        unit_tests/data_types.cpp

        input/cinputfunctions.cpp

        plat/coffee_windows/win_core.cpp
        plat/coffee_unix/unix_core.cpp
        plat/argument_parse.cpp
        plat/environment_details.cpp
        plat/plat_file.cpp
        plat/plat_timers.cpp
        plat/cmemory.cpp
        plat/plat_libraries.cpp

        ${CMAKE_SOURCE_DIR}/libs/tinyxml2/tinyxml2.cpp
        )

set(DL_LIBRARY "")
if(NOT WIN32)
        list(APPEND DL_LIBRARY dl pthread)
endif()

add_library( CoffeeCore ${COFFEE_LINK_OPT} ${CoffeeCore_SRCS} )

#Libraries for X11 and Linux
if(NOT WIN32)
    find_package(X11)
    find_library(M_LIB m)

    find_package(Unwind)
    target_include_directories(CoffeeCore SYSTEM PRIVATE "${LIBUNWIND_INCLUDE_DIR}")
endif()
############################

include_directories(
    ${COFFEE_CORE_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    )

target_link_libraries ( CoffeeCore
		${LIBUNWIND_LIBRARIES}
		${DL_LIBRARY}
	)

if(WIN32)
    target_link_libraries ( CoffeeCore
        pathcch
        )
endif()

set_property(TARGET CoffeeCore PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(CoffeeCore PRIVATE cxx_variadic_templates)
target_compile_features(CoffeeCore PRIVATE cxx_constexpr)
