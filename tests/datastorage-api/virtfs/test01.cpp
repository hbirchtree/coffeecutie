#include <coffee/core/CFiles>
#include <coffee/core/VirtualFS>
#include <coffee/strings/libc_types.h>

#include <coffee/core/CUnitTesting>

using namespace Coffee;

using semantic::Bytes;

using RSC = Coffee::Resource;

static constexpr std::string_view test_txt_content = "TEST DATA LOL"
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
                                                     "TEST DATA LOL";

bool gen_dummy_virtfs(
    Vector<byte_t>&    outputData,
    Vector<vfs::desc>& inputData,
    vfs::fs const**    vfs)
{
    inputData.emplace_back(
        "test.txt",
        BytesConst::ofContainer(test_txt_content).view,
        vfs::File_Compressed);
    inputData.emplace_back(
        "test2.txt", BytesConst::ofContainer(test_txt_content).view, 0);

    if(auto fs = vfs::generate(inputData); fs.has_error())
        return false;
    else
        outputData = fs.value();

    if(auto fs = vfs::fs::open(Bytes::ofContainer(outputData).view);
       fs.has_error())
        return false;
    else
        *vfs = fs.value();

    return true;
}

bool virtfs_serialize()
{
    std::vector<byte_t>    outputData;
    std::vector<vfs::desc> inputData;
    vfs::fs const*         readVfs = nullptr;

    if(!gen_dummy_virtfs(outputData, inputData, &readVfs))
        return false;

    cDebug("Packed size: {0}", outputData.size());

    auto out_rsc = MkUrl("test.vfs", RSCA::TempFile);

    RSC output(out_rsc);
    output = Bytes::ofContainer(outputData);

    if(!FileCommit(output, RSCA::WriteOnly | RSCA::NewFile | RSCA::Discard))
        return false;

    if(auto fs = vfs::fs::open(output.data()); fs.has_error())
        return false;
    else
        readVfs = fs.value();

    vfs::Resource rsc(readVfs, MkUrl("test2.txt"));

    if(!rsc.valid())
        return false;

    auto rscData = rsc.data();

    if(rscData.size != inputData[1].data.size())
        return false;

    return true;
}

bool virtfs_iterator()
{
    Vector<byte_t>    outputData;
    Vector<vfs::desc> inputData;
    vfs::fs const*    readVfs = nullptr;

    if(!gen_dummy_virtfs(outputData, inputData, &readVfs))
        return false;

    vfs::view view = vfs::view::of(Bytes::ofContainer(outputData).view);

    for(auto const& file : view)
        cDebug("File: {0}", file.name());

    return true;
}

COFFEE_TESTS_BEGIN(2)

    {virtfs_serialize, "VirtFS serialization"},
    {virtfs_iterator, "VirtFS iterators"}

COFFEE_TESTS_END()
