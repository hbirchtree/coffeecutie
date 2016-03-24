#include <coffee/core/CUnitTesting>

using namespace Coffee;

bool basic_tests()
{
    /* Verify storage sizes */
    if(sizeof(uint8)!=1)
        return false;
    if(sizeof(uint16)!=2)
        return false;
    if(sizeof(uint32)!=4)
        return false;
    if(sizeof(uint64)!=8)
        return false;

    if(sizeof(int8)!=1)
        return false;
    if(sizeof(int16)!=2)
        return false;
    if(sizeof(int32)!=4)
        return false;
    if(sizeof(int64)!=8)
        return false;

    if(sizeof(szptr)!=sizeof(intptr_t))
        return false;

    return true;
}

bool wrapping_tests()
{
    /* Unsigned */
    {
        uint8 v = UInt8_Max;
        v++;
        if(v != 0)
            return false;
    }
    {
        uint16 v = UInt16_Max;
        v++;
        if(v != 0)
            return false;
    }
    {
        uint32 v = UInt32_Max;
        v++;
        if(v != 0)
            return false;
    }
    {
        uint64 v = UInt64_Max;
        v++;
        if(v != 0)
            return false;
    }

    /* Signed, positive */
    {
        int8 v = Int8_Max;
        v++;
        if(v != Int8_Min)
            return false;
    }
    {
        int16 v = Int16_Max;
        v++;
        if(v != Int16_Min)
            return false;
    }
    {
        int32 v = Int32_Max;
        v++;
        if(v != Int32_Min)
            return false;
    }
    {
        int64 v = Int64_Max;
        v++;
        if(v != Int64_Min)
            return false;
    }

    /* Signed, negative */
    {
        int8 v = Int8_Min;
        v--;
        if(v != Int8_Max)
            return false;
    }
    {
        int16 v = Int16_Min;
        v--;
        if(v != Int16_Max)
            return false;
    }
    {
        int32 v = Int32_Min;
        v--;
        if(v != Int32_Max)
            return false;
    }
    {
        int64 v = Int64_Min;
        v--;
        if(v != Int64_Max)
            return false;
    }

    return true;
}

bool uint24_test()
{
    cWarning("This test is likely to fail on Windows!");
    return sizeof(uint24)==3;
}

static const constexpr CoffeeTest::Test _tests[3] = {
    basic_tests,
    wrapping_tests,
    uint24_test
};

COFFEE_RUN_TESTS(_tests);