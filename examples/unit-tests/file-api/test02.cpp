#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

const cstring small_map_test = "file_map_small.bin";

byte_t sample_storage[Unit_kB*100] = {
    "I'M THE TRASHMAN!\n"
};

bool filewrite_test()
{
    CResources::Resource rsc(small_map_test);
    rsc.data = sample_storage;
    rsc.size = sizeof(sample_storage);
    return CResources::FileCommit(rsc);
}

bool filemap_test()
{
    CResources::Resource rsc(small_map_test);
    CResources::FileMap(rsc);
    bool status = MemCmp(sample_storage,rsc.data,sizeof(sample_storage));
    CResources::FileUnmap(rsc);
    return status;
}

const cstring big_map_test = "file_map_large.bin";

//byte_t sample_storage_large[Unit_GB*5] = {

//};

const constexpr CoffeeTest::Test _tests[2] = {
    {filewrite_test,"File writing","Really just prepares for the next test"},
    {filemap_test,"File mapping"}
};

COFFEE_RUN_TESTS(_tests);