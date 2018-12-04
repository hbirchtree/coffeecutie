#include <coffee/core/CUnitTesting>
#include <coffee/core/stl_types.h>
#include <peripherals/stl/string_ops.h>

using namespace Coffee;

bool compare_test()
{
    constexpr cstring string1 = "test";
    constexpr cstring string2 = "test";
    return CString(string1) == string2;
}

bool replace_test()
{
    constexpr cstring final_string = "test\\\\file\\\\name";
    CString           test_string =
        str::replace::str<char>("test/file/name", "/", "\\\\");
    return test_string == final_string;
}

const constexpr CoffeeTest::Test _tests[2] = {
    {compare_test, "C-string comparison"},
    {replace_test, "STL string replacement", "Used for resource operations"}};

COFFEE_RUN_TESTS(_tests);
