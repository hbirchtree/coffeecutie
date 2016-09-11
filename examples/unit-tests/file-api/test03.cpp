#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

using Scratch = CResources::FileFun::ScratchBuf;

bool filescratch_test()
{
#if defined(COFFEE_RASPBERRYPI)
    /* Raspberry Pi has limited memory, and often does not have swap */
    /* 256MB devices are out of the question */
    szptr size = 256*1024*1024;
#else
    szptr size = 512*1024*1024;
#endif

    Scratch f = CResources::FileFun::ScratchBuffer(size,ResourceAccess::ReadWrite);

    /* If true, no buffer was mapped */
    if(f.size == 0 || !f.ptr)
        return false;

    /* Seeing if SEGFAULT occurs when touching the memory */
    Mem::MemClear(f.ptr,f.size);

    /* Writing test, writing and confirming data */

    cstring test_data = "TESTDATA";

    szptr sz = StrLen(test_data);
    szptr times = size/sz - size%sz;

    for(uint32 i=0;i<times;i++)
        Mem::MemCpy(&((byte_t*)f.ptr)[sz*i],test_data,sz);

    bool flag = true;

    for(uint32 i=0;i<times;i++)
        if(!Mem::MemCmp(&((byte_t*)f.ptr)[sz*i],test_data,sz))
        {
            flag = false;
            break;
        }

    CResources::FileFun::ScratchUnmap(&f);

    return flag;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {filescratch_test,"Scratch buffers","Creating and using a scratch buffer",true},
};

COFFEE_RUN_TESTS(_tests);
