coffee_test (
    TARGET Unit_HandleSemantics
    TITLE "Cu_ResourceHandle"

    SOURCES handles.cpp

    LIBRARIES
    CoffeeCore
    )

coffee_test (
    TARGET Unit_ErrorCodeNesting
    TITLE "Cu_ErrorCodeNestingSearch"

    SOURCES test_error_codes.cpp

    LIBRARIES
    CoffeeCore
    )
