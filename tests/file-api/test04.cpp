#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>
#include <coffee/core/types/cdef/memsafe.h>

using namespace Coffee;

const Url testfile = MkUrl("fileapi_testfile.txt",
                           ResourceAccess::SpecifyStorage
                           |ResourceAccess::TemporaryFile);
const Url writetest = MkUrl("file_write_large.bin",
                            ResourceAccess::SpecifyStorage
                            |ResourceAccess::TemporaryFile);

using File = FileFun;

byte_t write_data[100] = {
    "I'M THE TRASHMAN. I THROW GARBAGE ALL OVER THE RING, AND THEN I START EATING GARBAGE.\n"
};

const szptr dynamic_size = 5_GB;
Bytes dynamic_store = {};

bool filewrite_test()
{
    Resource rsc(writetest);
    Profiler::Profile("Pre-allocation setup");

    rsc = Bytes::Alloc(dynamic_size);
    Profiler::Profile("5GB allocation");

    Bytes rscView = rsc;

    {
//        byte_t* dest = (byte_t*)rsc.data;
        /* Write some data below 4GB mark */
//        MemCpy(dest,write_data,sizeof(write_data));
        MemCpy(Bytes::From(write_data, 100), rscView.at(0));
        /* Write data above 4GB mark, requires 64-bit. Fuck 32-bit. */
        MemCpy(Bytes::From(write_data, 100), rscView.at(4_GB));
//        MemCpy(&dest[4_GB],write_data,sizeof(write_data));
    }
    Profiler::Profile("Copying data into segment");

    dynamic_store = C_OCAST<Bytes>(rsc);

    bool stat = FileCommit(rsc,false,
                           ResourceAccess::WriteOnly
                           |ResourceAccess::Discard);
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
            status = MemCmp(dynamic_store, C_OCAST<Bytes>(rsc));
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
