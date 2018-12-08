#include <coffee/strings/libc_types.h>

#include <coffee/core/CStringFormat>
#include <coffee/core/CUnitTesting>
#include <coffee/core/coffee_saving.h>

using namespace Coffee;

struct TestStructure
{
    u64 big_integer_test;
    i32 smaller_integer_test;
    i16 small_integer_test;
    i16 _pad;

    bool operator==(const TestStructure& b)
    {
        return big_integer_test == b.big_integer_test &&
               smaller_integer_test == b.smaller_integer_test &&
               small_integer_test == b.small_integer_test;
    }
    bool operator!=(const TestStructure& b)
    {
        return !(*this == b);
    }
    operator CString()
    {
        return cStringFormat(
            "TestStructure({0}, {1}, {2}, {3})",
            big_integer_test,
            0,
            smaller_integer_test,
            small_integer_test);
    }
};

bool save_to_disk()
{
    auto storeApi = Store::CreateDefaultSave();

    TestStructure data = {};

    cVerbose(5, "Separate operation");
    storeApi->save(Bytes::Create(data));

    cDebug("On-disk: {0}", C_CAST<CString>(data));

    data.big_integer_test     = UInt32_Max * 4;
    data.smaller_integer_test = UInt16_Max * 2;
    data.small_integer_test   = 512;

    TestStructure data_cpy = data;

    cVerbose(5, "Separate operation");
    if(storeApi->save(Bytes::Create(data)) != sizeof(data))
        return false;

    data = {};

    cVerbose(5, "Separate operation");
    if(storeApi->restore(Bytes::Create(data)) != sizeof(data))
        return false;

    cDebug("In-memory: {0}", C_CAST<CString>(data_cpy));
    cDebug("On-disk: {0}", C_CAST<CString>(data));

    return data_cpy == data;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {save_to_disk, "Saving and loading from disk"},
};

COFFEE_RUN_TESTS(_tests)
