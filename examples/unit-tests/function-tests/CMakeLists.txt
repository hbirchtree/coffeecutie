set ( Calls01_SRCS
    test01.cpp
    test01_extra.cpp
    )
coffee_add_example ( Unit_Func_Calls01 "${Calls01_SRCS}" )
target_link_libraries ( Unit_Func_Calls01
    CoffeeCore
    )

coffee_add_example ( Unit_Func_Print01 test02.cpp )
target_link_libraries ( Unit_Func_Print01
    CoffeeCore
    )

coffee_add_example ( Unit_Func_Signals01 test03.cpp )
target_link_libraries ( Unit_Func_Signals01
    CoffeeCore
    )