#include <coffee/core/CUnitTesting>
#include <coffee/core/types/cdef/memsafe.h>

using namespace Coffee;

const constexpr cstring complete = "SGVsbG8gV29ybGQh";
const constexpr cstring original = "Hello World!";

bool encode_test()
{
    CString b64_data = b64::encode(original, str::len(original));

    if(b64_data != complete)
        cDebug("Wrong data: {0} != {1}", b64_data, complete);

    return b64_data == complete;
}

bool decode_test()
{
    Vector<byte_t> out;

    bool res = b64::decode((byte_t const*)complete, str::len(complete), &out);

    Bytes originalData = Bytes::From(original, sizeof(original));

    if(!out.size())
        cDebug("No data decoded");
    else if(!MemCmp(Bytes::CreateFrom(out), originalData))
        cDebug("Wrong data: {0} != {1}", &out[0], complete);

    return res && str::len(original) == out.size() &&
           MemCmp(Bytes::CreateFrom(out), originalData);
}

const constexpr CoffeeTest::Test _tests[2] = {
    {encode_test, "Encoding Base64", ""},
    {decode_test, "Decoding Base64", "", true, false},
};

COFFEE_RUN_TESTS(_tests);
