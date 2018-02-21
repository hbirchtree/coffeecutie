#include <coffee/core/CUnitTesting>
#include <coffee/core/datastorage/binary/virtualfs.h>
#include <coffee/core/CFiles>

using namespace Coffee;

using RSC = Coffee::CResources::Resource;

static const cstring test_txt_content =
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        "TEST DATA LOL"
        ;

bool virtfs_serialize()
{
    Vector<byte_t> outputData;
    Vector<VirtFS::VirtDesc> inputData;
    inputData.emplace_back(
            "test.txt",
            Bytes::CreateString(test_txt_content),
            VirtFS::File_Compressed
        );

    bool status = VirtFS::GenVirtFS(inputData, &outputData);

    cDebug("Packed size: {0}", outputData.size());

    RSC output(MkUrl("test.vfs", RSCA::TempFile));
    output.data = outputData.data();
    output.size = outputData.size();

    FileCommit(output);

    return status;
}

COFFEE_TEST_SUITE(2) = {
    {virtfs_serialize, "VirtFS serialization"},
    {virtfs_serialize, "VirtFS serialization"}
};

COFFEE_EXEC_TESTS();
