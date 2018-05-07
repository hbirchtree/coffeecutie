#include <coffee/core/CUnitTesting>
#include <coffee/core/coffee_saving.h>

using namespace Coffee;

struct TestStructure
{
    uint64 big_integer_test;
    bigscalar big_float_test;
    int32 smaller_integer_test;
    int16 small_integer_test;

    bool operator ==(const TestStructure& b)
    {
        return big_integer_test == b.big_integer_test &&
                big_float_test == b.big_float_test &&
                smaller_integer_test == b.smaller_integer_test &&
                small_integer_test == b.small_integer_test;
    }
    bool operator !=(const TestStructure& b)
    {
        return !(*this == b);
    }
    operator CString()
    {
        return cStringFormat("TestStructure({0}, {1}, {2}, {3})",
                             big_integer_test, big_float_test,
                             smaller_integer_test, small_integer_test);
    }
};

bool save_to_disk()
{
    TestStructure data = {};
    szptr data_size = sizeof(data);

    cVerbose(5, "Separate operation");
    Store::RestoreMemory(Bytes::Create(data));

    cDebug("On-disk: {0}", C_CAST<CString>(data));

    data.big_float_test = 1610493.10;
    data.big_integer_test = UInt32_Max * 4;
    data.smaller_integer_test = UInt16_Max * 2;
    data.small_integer_test = 512;

    TestStructure data_cpy = data;

    cVerbose(5, "Separate operation");
    Store::SaveMemory(Bytes::Create(data));

    data_size = sizeof(data);

    cVerbose(5, "Separate operation");
    Store::RestoreMemory(Bytes::Create(data));

    cDebug("In-memory: {0}", C_CAST<CString>(data_cpy));
    cDebug("On-disk: {0}", C_CAST<CString>(data));

    return data_cpy == data;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {save_to_disk, "Saving and loading from disk"},
};

COFFEE_RUN_TESTS(_tests)
