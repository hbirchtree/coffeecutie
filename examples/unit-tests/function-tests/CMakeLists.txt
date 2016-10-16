set ( Calls01_SRCS
    test01.cpp
    test01_extra.cpp
    )

#coffee_add_test ( Unit_Func_Calls01 "Cu_FuncBench" "${Calls01_SRCS}" CoffeeCore )

coffee_add_test ( Unit_Func_Print01 "Cu_PrintBench" test02.cpp CoffeeCore )

coffee_add_test ( Unit_Func_Signals01 "Cu_Signals" test03.cpp CoffeeCore )
