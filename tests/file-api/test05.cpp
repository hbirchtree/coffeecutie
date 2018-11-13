#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>
#include <coffee/core/types/chunk.h>

using namespace Coffee;

byte_t sample_storage[Unit_kB * 100] = {"I'M THE TRASHMAN!\n"};

const Url big_map_test = MkUrl("file_map_large.bin", RSCA::TemporaryFile);

Bytes large_data = {};

bool filewrite_large_test()
{
    CResources::Resource rsc(big_map_test);

    /* TODO: Update with Bytes */

    Profiler::Profile("Pre-allocation setup");

    large_data = Bytes::Alloc(5 * Unit_GB);
    Profiler::Profile("5GB allocation");

    rsc = large_data;

    {
        cstring test_string = "I'M THE TRASHMAN!\n";
        memcpy(large_data.data, test_string, str::len(test_string));
        memcpy(&large_data[4_GB], test_string, str::len(test_string));
    }

    Profiler::Profile("Copying data into segment");

    bool stat = CResources::FileCommit(
        rsc, RSCA::WriteOnly | RSCA::NewFile | RSCA::Discard);
    Profiler::Profile("Writing 5GB of data to disk");

    return stat;
}

bool filemap_large_test()
{
    bool       stat = true;
    file_error ec;

    CResources::Resource rsc(big_map_test);
    Profiler::Profile("Pre-mapping setup");

    CResources::FileMap(rsc);
    Profiler::Profile("Massive file mapping");

    if(rsc.size != Unit_GB * 5)
        stat = false;
    else
    {
        stat = MemCmp(large_data, C_OCAST<Bytes>(rsc));
        Profiler::Profile("Comparing 5GB of data");
    }

    Profiler::Profile("Freeing 5GB of data");

    CResources::FileUnmap(rsc);
    Profiler::Profile("Unmapping resource");

    CResources::FileFun::Rm(big_map_test, ec);
    Profiler::Profile("Deleting file");

    return stat;
}

const constexpr CoffeeTest::Test _tests[2] = {
    {filewrite_large_test,
     "Large file writing",
     "Allocates 5GB of data and writes it to disk"},
    {filemap_large_test,
     "Large file mapping",
     "Mapping a large file into memory"}};

COFFEE_RUN_TESTS(_tests);
