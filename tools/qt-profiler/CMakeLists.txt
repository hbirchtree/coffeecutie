include ( QtIncludes )

set ( CoffeeProfiler_SRCS
    main.cpp
    profilingviewer.cpp
    )

coffee_add_example ( CoffeeProfiler "${CoffeeProfiler_SRCS}" )

target_link_libraries ( CoffeeProfiler
    CoffeeCore
    CoffeeQtShimCore
    )