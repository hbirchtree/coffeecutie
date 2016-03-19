include ( QtIncludes )

set ( CoffeeProfiler_SRCS
    main.cpp
    profilingviewer.cpp
    )

coffee_add_example ( Profiler "${CoffeeProfiler_SRCS}" )

target_link_libraries ( Profiler
    CoffeeCore
    CoffeeQtShimCore
    )