#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>
#include <peripherals/semantic/chunk.h>

using namespace Coffee;

const Url testfile  = MkUrl("fileapi_testfile.txt", RSCA::TemporaryFile);
const Url writetest = MkUrl("file_write_large.bin", RSCA::TemporaryFile);

using File = FileFun;

static byte_t write_data[100] = {"I'M THE TRASHMAN. I THROW GARBAGE ALL OVER "
                                 "THE RING, AND THEN I START EATING "
                                 "GARBAGE.\n"};

static const szptr dynamic_size  = 5_GB;
static Bytes       dynamic_store = {};

bool filewrite_test()
{
    Resource rsc(writetest);
    Profiler::Profile("Pre-allocation setup");

    rsc = Bytes::Alloc(dynamic_size);
    Profiler::Profile("5GB allocation");

    Bytes rscView = rsc;

    {
        /* Write some data below 4GB mark */
        MemCpy(Bytes::From(write_data, 100), *rscView.at(0, 100));
        /* Write data above 4GB mark, requires 64-bit. Fuck 32-bit. */
        MemCpy(Bytes::From(write_data, 100), *rscView.at(4_GB, 100));
    }

    Profiler::Profile("Copying data into segment");

    dynamic_store = C_OCAST<Bytes>(rsc);

    bool stat =
        FileCommit(rsc, RSCA::WriteOnly | RSCA::NewFile | RSCA::Discard);
    Profiler::Profile("Writing 5GB of data to disk");

    return stat;
}

bool fileread_test()
{
    bool       status = true;
    file_error ec;

    Resource rsc(writetest);
    Profiler::Profile("Pre-reading setup");

    status = FilePull(rsc);
    Profiler::Profile("Reading massive data");

    if(status)
    {
        status = (dynamic_size == rsc.size);

        if(status)
        {
            status = MemCmp(dynamic_store, C_OCAST<Bytes>(rsc));
            Profiler::Profile("Comparing 5GB of data");
        }

        File::Rm(writetest, ec);
        Profiler::Profile("Deleting file");
    }

    return status;
}

COFFEE_TESTS_BEGIN(2)

    {filewrite_test,
     "Massive file writing",
     "Writing lots of text data to disk"},
    {fileread_test,
     "Massive file reading",
     "Reading lots of text data from disk"}

COFFEE_TESTS_END()
