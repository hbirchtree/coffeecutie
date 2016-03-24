#include <coffee/core/CUnitTesting>

using namespace Coffee;

bool compare_test()
{
    constexpr cstring string1 = "test";
    constexpr cstring string2 = "test";
    return StrCmp(string1,string2);
}

bool replace_test()
{
    constexpr cstring final_string = "test\\\\file\\\\name";
    CString test_string = CStrReplace("test/file/name","/","\\\\");
    return StrCmp(test_string.c_str(),final_string);
}

static const constexpr CoffeeTest::Test _tests[2] = {
    compare_test,
    replace_test
};

COFFEE_RUN_TESTS(_tests);