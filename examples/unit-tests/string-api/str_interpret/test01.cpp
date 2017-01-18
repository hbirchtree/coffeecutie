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
        test = cast_pod<uint32>(123);

        if(test != "123")
            break;

        test = cast_pod<scalar>(123.567f);

        if(StrUtil::zerortrim(test) != "123.567")
            break;

        test = cast_pod<bigscalar>(123456.78910);

        if(test != "123456.78910")
            break;

        return true;
    }while(false);


    return false;
}

static const CoffeeTest::Test string_tests[2] = {
    {interpret_integers},
    {convert_string}
};

COFFEE_RUN_TESTS(string_tests)
