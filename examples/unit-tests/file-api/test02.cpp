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

void* large_data = nullptr;

bool filewrite_large_test()
{
    CResources::Resource rsc(big_map_test);
    rsc.size = Unit_GB*5;
    rsc.data = Alloc(rsc.size);
    large_data = rsc.data;
    byte_t* data = (byte_t*)large_data;
    cstring test_string = "I'M THE TRASHMAN!\n";
    MemCpy(data,test_string,StrLen(test_string));
    bool stat = CResources::FileCommit(rsc);
    return stat;
}

bool filemap_large_test()
{
    CResources::Resource rsc(big_map_test);
    CResources::FileMap(rsc);
    bool stat = MemCmp(large_data,rsc.data,rsc.size);
    CFree(large_data);
    CResources::FileUnmap(rsc);
    return stat;
}

const constexpr CoffeeTest::Test _tests[4] = {
    {filewrite_test,"File writing","Really just prepares for the next test"},
    {filemap_test,"File mapping"},
    {filewrite_large_test,"Large file writing","Allocates 5GB of data and writes it to disk",true},
    {filemap_large_test,"Large file mapping","Mapping a large file into memory",true}
};

COFFEE_RUN_TESTS(_tests);