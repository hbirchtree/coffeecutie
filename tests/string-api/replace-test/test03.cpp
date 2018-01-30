#include <coffee/core/CUnitTesting>

using namespace Coffee;

const constexpr cstring complete = "SGVsbG8gV29ybGQh";
const constexpr cstring original = "Hello World!";

bool encode_test()
{
    CString b64_data = Base64::encode(original,StrLen(original));

    if(b64_data != complete)
        cDebug("Wrong data: {0} != {1}",b64_data,complete);

    return b64_data == complete;
}

bool decode_test()
{
    Vector<byte_t> out;

    bool res = Base64::decode((byte_t const*)complete,StrLen(complete),&out);

    if(!out.size())
        cDebug("No data decoded");
    else
        if(!MemCmp(&out[0],original,out.size()))
            cDebug("Wrong data: {0} != {1}",&out[0],complete);

    return res && StrLen(original)==out.size() && MemCmp(out.data(),original,StrLen(original));
}

const constexpr CoffeeTest::Test _tests[2] = {
    {encode_test,"Encoding Base64", ""},
    {decode_test,"Decoding Base64", "", true, false},
};

COFFEE_RUN_TESTS(_tests);
