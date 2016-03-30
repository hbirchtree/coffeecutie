include ( QtIncludes )

set ( CoffeeProfiler_SRCS
    main.cpp
    profilingviewer.cpp
    )

list ( APPEND LIBRARIES
    CoffeeCore
    CoffeeQtShimCore
)

coffee_add_example ( Profiler "Application Profiler" "${CoffeeProfiler_SRCS}" "${LIBRARIES}" )
