#include <coffee/core/CUnitTesting>
#include <peripherals/stl/types.h>
#include <peripherals/stl/string_ops.h>

using namespace Coffee;

bool compare_test()
{
    constexpr cstring string1 = "test";
    constexpr cstring string2 = "test";
    return std::string(string1) == string2;
}

bool replace_test()
{
    constexpr cstring final_string = "test\\\\file\\\\name";
    std::string           test_string =
        str::replace::str<char>("test/file/name", "/", "\\\\");
    return test_string == final_string;
}

COFFEE_TESTS_BEGIN(2)

    {compare_test, "C-string comparison"},
    {replace_test, "STL string replacement", "Used for resource operations"}

COFFEE_TESTS_END()
