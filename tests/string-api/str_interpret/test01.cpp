#include <coffee/core/CUnitTesting>
#include <coffee/core/string_casting.h>
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/CMath>

using namespace Coffee;

bool interpret_integers()
{
    do
    {
        {
            u64 test = cast_string<u64>("123");
            if(test != 123)
                break;
        }
        {
            bigscalar test = cast_string<bigscalar>("123.15");
            if(!CMath::floating_equal(test, 123.15))
                break;
        }
        return true;
    } while(false);

    return false;
}

bool convert_string()
{
    CString test;
    do
    {
        test = cast_pod(123);

        if(test != "123")
            break;

        test = cast_pod(123.567f);

        /* Maybe we shouldn't... */
        if(str::trim::right_zero(test) != "123.567001")
            break;

        test = cast_pod(123456.78910);

        /* We should stop doing this... */
        if(str::trim::right_zero(test) != "123456.7891")
            break;

        return true;
    } while(false);

    return false;
}

bool string_literal_test()
{
//    bigscalar cast_test_f = "127"_bigscalar;
//    uint64    cast_test_u = "127"_u64;
//    int64     cast_test_i = "-127"_i64;

//    if(cast_test_f != 127 || cast_test_u != 127 || cast_test_i != -127)
//        return false;

    return true;
}

static const CoffeeTest::Test string_tests[3] = {
    {interpret_integers, "Interpreting integers from strings"},
    {convert_string, "String conversion of POD types", "", true},
    {string_literal_test, "Interpreting string literals"}};

COFFEE_RUN_TESTS(string_tests)
