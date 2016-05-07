#include <coffee/core/CUnitTesting>

using namespace Coffee;

const constexpr cstring complete = "SGVsbG8gV29ybGQh";
const constexpr cstring original = "Hello World!";

bool encode_test()
{
    CString b64_data = Base64::encode(original,StrLen(original));

    if(b64_data != complete)
        cDebug("Wrong data: {0}",b64_data);

    return b64_data == complete;
}

bool decode_test()
{
    Vector<byte_t> out;

    bool res = Base64::decode((byte_t const*)complete,StrLen(original),&out);

    return res && StrLen(original)==out.size() && MemCmp(out.data(),original,StrLen(complete));
}

const constexpr CoffeeTest::Test _tests[2] = {
    {encode_test,"Converting to Base64"},
    {decode_test,"Converting to Base64"},
};

COFFEE_RUN_TESTS(_tests);
