#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

const cstring testfile = "fileapi_testfile.txt";

using Resource = CResources::Resource;
using File = CResources::FileFun;

const cstring writetest = "file_write_large.bin";
byte_t write_data[100] = {
    "I'M THE TRASHMAN. I THROW GARBAGE ALL OVER THE RING, AND THEN I START EATING GARBAGE.\n"
};

const constexpr szptr dynamic_size = Unit_GB*5;
void* dynamic_store = nullptr;

bool filewrite_test()
{
    Resource rsc(writetest);
    rsc.size = dynamic_size;
    rsc.data = Calloc(1,rsc.size);

    {
        byte_t* dest = (byte_t*)rsc.data;
        /* Write some data below 4GB mark */
        MemCpy(dest,write_data,sizeof(write_data));
        /* Write data above 4GB mark, requires 64-bit. Fuck 32-bit. */
        MemCpy(&dest[Unit_GB*4],write_data,sizeof(write_data));
    }

    dynamic_store = rsc.data;

    return CResources::FileCommit(rsc,false, ResourceAccess::WriteOnly | ResourceAccess::Discard);
}

bool fileread_test()
{
    bool status = true;

    CResources::Resource rsc(writetest);
    status = CResources::FilePull(rsc);

    if(status)
    {
        status = (dynamic_size == rsc.size);

        if(status)
            status = MemCmp(dynamic_store,rsc.data,dynamic_size);

        CResources::FileFree(rsc);

        File::Rm(writetest);
    }

    return status;
}

const constexpr CoffeeTest::Test _tests[2] = {
    {filewrite_test,"Massive file writing","Writing lots of text data to disk"},
    {fileread_test,"Massive file reading","Reading lots of text data from disk"},
};

COFFEE_RUN_TESTS(_tests);
