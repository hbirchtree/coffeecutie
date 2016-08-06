include ( QtIncludes )

set ( CoffeeProfiler_SRCS
    main.cpp
    profilingviewer.cpp
    )

list ( APPEND LIBRARIES
    CoffeeCore
    CoffeeQtShimCore
)

coffee_add_application ( Profiler "Application Profiler" "${CoffeeProfiler_SRCS}" "${LIBRARIES}" )
