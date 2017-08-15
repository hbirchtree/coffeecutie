#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

const cstring testfile = "fileapi_testfile.txt";

using File = FileFun;

const cstring writetest = "file_write_large.bin";
byte_t write_data[100] = {
    "I'M THE TRASHMAN. I THROW GARBAGE ALL OVER THE RING, AND THEN I START EATING GARBAGE.\n"
};

const szptr dynamic_size = 5_GB;
void* dynamic_store = nullptr;

bool filewrite_test()
{
    Resource rsc(writetest);
    rsc.size = dynamic_size;
    Profiler::Profile("Pre-allocation setup");

    rsc.data = Calloc(1,rsc.size);
    Profiler::Profile("5GB allocation");

    {
        byte_t* dest = (byte_t*)rsc.data;
        /* Write some data below 4GB mark */
        MemCpy(dest,write_data,sizeof(write_data));
        /* Write data above 4GB mark, requires 64-bit. Fuck 32-bit. */
        MemCpy(&dest[4_GB],write_data,sizeof(write_data));
    }
    Profiler::Profile("Copying data into segment");

    dynamic_store = rsc.data;

    bool stat = FileCommit(rsc,false, ResourceAccess::WriteOnly | ResourceAccess::Discard);
    Profiler::Profile("Writing 5GB of data to disk");

    return stat;
}

bool fileread_test()
{
    bool status = true;

    Resource rsc(writetest);
    Profiler::Profile("Pre-reading setup");

    status = CResources::FilePull(rsc);
    Profiler::Profile("Reading massive data");

    if(status)
    {
        status = (dynamic_size == rsc.size);

        if(status)
        {
            status = MemCmp(dynamic_store,rsc.data,dynamic_size);
            Profiler::Profile("Comparing 5GB of data");
        }

        CResources::FileFree(rsc);
        Profiler::Profile("Freeing 5GB of data");

        File::Rm(writetest);
        Profiler::Profile("Deleting file");
    }

    return status;
}

const constexpr CoffeeTest::Test _tests[2] = {
    {filewrite_test,"Massive file writing","Writing lots of text data to disk"},
    {fileread_test,"Massive file reading","Reading lots of text data from disk"},
};

COFFEE_RUN_TESTS(_tests);
