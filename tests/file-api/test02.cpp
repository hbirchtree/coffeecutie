#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>
#include <coffee/core/types/cdef/memsafe.h>

using namespace Coffee;

const Url small_map_test = MkUrl(
    "file_map_small.bin",
    ResourceAccess::SpecifyStorage | ResourceAccess::TemporaryFile);

byte_t sample_storage[Unit_kB * 100] = {"I'M THE TRASHMAN!\n"};

bool filewrite_test()
{
    CResources::Resource rsc(small_map_test);

    rsc = Bytes::From(sample_storage, sizeof(sample_storage));

    return CResources::FileCommit(
        rsc,
        ResourceAccess::WriteOnly | ResourceAccess::Discard |
            ResourceAccess::NewFile);
}

bool filemap_test()
{
    file_error ec;

    CResources::Resource rsc(small_map_test);
    CResources::FileMap(rsc);
    if(!rsc.data)
        return false;
    bool status = MemCmp(
        Bytes::From(sample_storage, sizeof(sample_storage)),
        C_OCAST<Bytes>(rsc));
    CResources::FileUnmap(rsc);

    CResources::FileFun::Rm(small_map_test, ec);

    return status;
}

const constexpr CoffeeTest::Test _tests[2] = {
    {filewrite_test, "File writing", "Really just prepares for the next test"},
    {filemap_test, "File mapping"}};

COFFEE_RUN_TESTS(_tests);
