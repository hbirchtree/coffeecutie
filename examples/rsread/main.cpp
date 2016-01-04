#include <coffee/CCore>

using namespace Coffee;
using namespace Coffee::CResources;

struct C_FORCE_PACKING RSCacheSectorHeader
{
    int16 fileNumber;
    int16 filePartNumber;
    uint24 sectorNumber;
    uint8 cacheNumber;
};

struct C_FORCE_PACKING RSIndexEntry
{
    uint24 fileSize;
    uint24 sectorAt;
};

int32 coffee_main(int32 argc, byte_t** argv)
{
    coffee_file_set_resource_prefix("sample_data/runescape-data/");

    CResource idxfile("main_file_cache.idx0");
    coffee_file_pull(idxfile);

    RSCacheSectorHeader* t1 = (RSCacheSectorHeader*)idxfile.data;
    RSIndexEntry* t2 = (RSIndexEntry*)idxfile.data;

    coffee_file_free(idxfile);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
