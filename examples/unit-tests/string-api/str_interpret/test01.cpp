#include <coffee/core/CUnitTesting>
#include <coffee/core/string_casting.h>
#include <coffee/core/types/tdef/integertypes.h>

using namespace Coffee;

bool interpret_integers()
{
    do{
        {
            uint64 test = cast_string<uint64>("123.15");
            if(test != 123)
                break;
        }
        {
            bigscalar test = cast_string<bigscalar>("123.15");
            if(test != 123.15)
                break;
        }
        return true;
    }while(false);

    return false;
}

bool convert_string()
{
    CString test;
    do{
        test = cast_pod(123);

        if(test != "123")
            break;

        test = cast_pod(123.567f);

        if(StrUtil::zerortrim(test) != "123.567")
            break;

        test = cast_pod(123456.78910);

        if(test != "123456.7891")
            break;

        return true;
    }while(false);


    return false;
}

bool string_literal_test()
{
    bigscalar cast_test_f = "127"_bigscalar;
    uint64 cast_test_u = "127"_uint64;
    int64 cast_test_i = "-127"_int64;

    if(cast_test_f != 127 ||
            cast_test_u != 127 ||
            cast_test_i != -127)
        return false;

    return true;
}

static const CoffeeTest::Test string_tests[3] = {
    {interpret_integers},
    {convert_string, "String conversion of POD types", "", false},
    {string_literal_test}
};

COFFEE_RUN_TESTS(string_tests)
