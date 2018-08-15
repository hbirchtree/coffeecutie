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

using VFS = VirtFS::VFS;

bool gen_dummy_virtfs(Vector<byte_t>& outputData,
                      Vector<VirtFS::VirtDesc>& inputData,
                      VFS const** vfs)
{
    inputData.emplace_back(
            "test.txt",
            Bytes::CreateString(test_txt_content),
            VirtFS::File_Compressed
        );
    inputData.emplace_back(
            "test2.txt",
            Bytes::CreateString(test_txt_content),
            0
        );

    if(!VirtFS::GenVirtFS(inputData, &outputData))
        return false;

    if(!VFS::OpenVFS(Bytes::CreateFrom(outputData), vfs))
        return false;

    return true;
}

bool virtfs_serialize()
{
    Vector<byte_t> outputData;
    Vector<VirtFS::VirtDesc> inputData;
    VFS const* readVfs = nullptr;

    if(!gen_dummy_virtfs(outputData, inputData, &readVfs))
        return false;

    cDebug("Packed size: {0}", outputData.size());

    auto out_rsc = MkUrl("test.vfs", RSCA::TempFile);

    RSC output(out_rsc);
    output.data = outputData.data();
    output.size = outputData.size();

    if(!FileCommit(output, RSCA::WriteOnly | RSCA::NewFile | RSCA::Discard))
        return false;

    if(!VFS::OpenVFS(FileGetDescriptor(output),
                             &readVfs))
        return false;

    VirtFS::Resource rsc(readVfs, MkUrl("test2.txt"));

    if(!rsc.valid())
        return false;

    Bytes rscData = rsc.data();

    if(rscData.size != inputData[1].data.size)
        return false;

    return true;
}

bool virtfs_iterator()
{
    Vector<byte_t> outputData;
    Vector<VirtFS::VirtDesc> inputData;
    VFS const* readVfs = nullptr;

    if(!gen_dummy_virtfs(outputData, inputData, &readVfs))
        return false;

    using vfs_view = VirtFS::vfs_view;

    vfs_view view(Bytes::CreateFrom(outputData));

    for(auto const& file : view)
        cDebug("File: {0}", file.name);

    return true;
}

COFFEE_TEST_SUITE(2) = {
    {virtfs_serialize, "VirtFS serialization"},
    {virtfs_iterator, "VirtFS iterators"}
};

COFFEE_EXEC_TESTS();
