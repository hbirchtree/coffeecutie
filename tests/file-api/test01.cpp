#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>
#include <coffee/core/types/cdef/memsafe.h>

using namespace Coffee;

const Url writetest = MkUrl(
    "writetest.txt",
    ResourceAccess::SpecifyStorage | ResourceAccess::TemporaryFile);
const Url testfile = MkUrl(
    "fileapi_testfile.txt",
    ResourceAccess::SpecifyStorage | ResourceAccess::TemporaryFile);

using File = FileFun;

byte_t write_data[100] = {"I'M THE TRASHMAN. I THROW GARBAGE ALL OVER THE "
                          "RING, AND THEN I START EATING GARBAGE.\n"};

bool filewrite_test()
{
    Resource rsc(writetest);
    rsc.size = sizeof(write_data);
    rsc.data = write_data;
    return CResources::FileCommit(
        rsc, false, ResourceAccess::WriteOnly | ResourceAccess::Discard);
}

bool fileread_test()
{
    CResources::Resource rsc(writetest);
    CResources::FilePull(rsc);

    cDebug(
        "\nTheirs:\n{1}\nMine:\n{0}",
        StrUtil::hexdump(write_data, sizeof(write_data), true, 10),
        StrUtil::hexdump(rsc.data, rsc.size, true, 10));

    if(sizeof(write_data) != rsc.size)
    {
        CResources::FileFree(rsc);
        return false;
    }
    bool status = MemCmp(
        Bytes::From(write_data, sizeof(write_data)), C_OCAST<Bytes>(rsc));
    CResources::FileFree(rsc);
    return status;
}

bool filesize_test()
{
    file_error ec;

    szptr size   = CResources::FileFun::Size(writetest, ec);
    szptr target = sizeof(write_data);
    cDebug("{0}?={1}", size, target);

    File::Rm(writetest, ec);

    return size == target;
}

COFFEE_TEST_SUITE(3) = {
    {filewrite_test, "File writing", "Writing text data to disk"},
    {fileread_test, "File reading", "Reading text data from disk"},
    {filesize_test, "File size", "Verifying file size on disk"},
};

COFFEE_EXEC_TESTS();
