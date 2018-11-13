#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>
#include <coffee/core/types/chunk.h>

using namespace Coffee;

using Scratch = CResources::FileFun::ScratchBuf;

bool filescratch_test()
{
    file_error ec;

#if defined(COFFEE_RASPBERRYPI)
    /* Raspberry Pi has limited memory, and often does not have swap */
    /* 256MB devices are out of the question */
    szptr size = 256 * 1024 * 1024;
#else
    szptr size = 512 * 1024 * 1024;
#endif

    Scratch f = CResources::FileFun::ScratchBuffer(size, RSCA::ReadWrite, ec);

    /* If true, no buffer was mapped */
    if(!f)
        return false;

    /* Seeing if SEGFAULT occurs when touching the memory */
    MemClear(f);

    /* Writing test, writing and confirming data */

    cstring test_data = "TESTDATA";

    szptr sz    = str::len(test_data);
    szptr times = size / sz - size % sz;

    Bytes testMem = Bytes::From(test_data, sz);

    for(u32 i = 0; i < times; i++)
        MemCpy(testMem, f.at(sz * i, sz));
    //        MemCpy(&((byte_t*)f.ptr)[sz*i],test_data,sz);

    bool flag = true;

    for(u32 i = 0; i < times; i++)
        if(!MemCmp(f.at(sz * i, sz), testMem))
        {
            flag = false;
            break;
        }

    CResources::FileFun::ScratchUnmap(std::move(f), ec);

    return flag;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {filescratch_test,
     "Scratch buffers",
     "Creating and using a scratch buffer",
     true},
};

COFFEE_RUN_TESTS(_tests);
