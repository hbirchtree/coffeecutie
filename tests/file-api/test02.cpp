#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>
#include <coffee/core/types/chunk.h>

using namespace Coffee;

const Url small_map_test = MkUrl("file_map_small.bin", RSCA::TemporaryFile);

static byte_t sample_storage[Unit_kB * 100] = {"I'M THE TRASHMAN!\n"};

bool filewrite_test()
{
    Resource rsc(small_map_test);

    rsc = Bytes::From(sample_storage, sizeof(sample_storage));

    return FileCommit(rsc, RSCA::WriteOnly | RSCA::Discard | RSCA::NewFile);
}

bool filemap_test()
{
    file_error ec;

    Resource rsc(small_map_test);
    FileMap(rsc);
    if(!rsc.data)
        return false;
    bool status = MemCmp(
        Bytes::From(sample_storage, sizeof(sample_storage)),
        C_OCAST<Bytes>(rsc));
    FileUnmap(rsc);

    FileFun::Rm(small_map_test, ec);

    return status;
}

COFFEE_TESTS_BEGIN(2)

    {filewrite_test, "File writing", "Really just prepares for the next test"},
    {filemap_test, "File mapping"}

COFFEE_TESTS_END()
