#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>

using namespace Coffee;

using FileInterface       = FileFunDef<>;
using NativeFileInterface = FileFun;

template<
    typename FileApi,
    typename NestedError,
    typename implements<FileFunDef<NestedError>, FileApi>::type* = nullptr>
bool api_test()
{
    /* This test suite tests whether the function signature of the
     *  file API is correct. If it is not, compilation will fail.
     */

    const auto testingUrl = MkUrl("testfile.dat");

    typename FileApi::file_error ec;

    /* File handle API */
    auto hnd = FileApi::Open(
        testingUrl, RSCA::NewFile | RSCA::Discard | RSCA::WriteOnly, ec);
    FileApi::Read(hnd, 1, ec);
    FileApi::Write(hnd, Bytes(), ec);
    FileApi::Size(hnd, ec);
    FileApi::Valid(hnd, ec);
    FileApi::Close(std::move(hnd), ec);
    hnd = {};

    /* Mapping API */
    auto map_hnd = FileApi::Map(testingUrl, RSCA::ReadOnly, 10, 0, ec);
    FileApi::MapCache(map_hnd.data, map_hnd.size, 0, 8, ec);
    FileApi::MapUncache(map_hnd.data, map_hnd.size, 0, 8, ec);
    FileApi::MapSync(map_hnd.data, map_hnd.size, ec);
    FileApi::Unmap(std::move(map_hnd), ec);
    map_hnd = {};

    /* ScratchBuf API */
    auto scratch_hnd = FileApi::ScratchBuffer(100, RSCA::ReadWrite, ec);
    FileApi::ScratchUnmap(std::move(scratch_hnd), ec);

    /* Plain URL API */
    FileApi::CanonicalName(testingUrl, ec);
    FileApi::DereferenceLink(testingUrl, ec);
    FileApi::Exists(testingUrl, ec);
    FileApi::Size(testingUrl, ec);
    FileApi::Touch(FileApi::NodeType::File, testingUrl, ec);
    FileApi::Stat(testingUrl, ec);
    FileApi::Ln(testingUrl, testingUrl, ec);
    FileApi::Rm(testingUrl, ec);

    return true;
}

bool test_url_version(RSCA access)
{
    try
    {
        Url test = MkUrl("test", access);

        CString test_string = *test;

        cDebug(
            "URL mapping({1}): test -> {0}",
            test_string,
            StrUtil::pointerify(C_CAST<u32>(access)));
    } catch(std::runtime_error const&)
    {
        return false;
    }
    return true;
}

bool url_api()
{
    do
    {
        if(!test_url_version(RSCA::SystemFile))
            break;
        if(!test_url_version(RSCA::AssetFile))
            break;
        if(!test_url_version(RSCA::ConfigFile))
            break;
        if(!test_url_version(RSCA::TemporaryFile))
            break;
        if(!test_url_version(RSCA::CachedFile))
            break;

        return true;
    } while(false);

    return false;
}

COFFEE_TEST_SUITE(4) = {
    {api_test<FileInterface, FileInterface::file_error::nested_error_type>,
     "Interface",
     "Testing that the unimplemented interface is properly tested",
     false,
     true},
    {api_test<CFILEFun, CFILEFun::file_error::nested_error_type>,
     "FILE API",
     "Testing the FILE*-based C API",
     false,
     true},
    {api_test<
         NativeFileInterface,
         NativeFileInterface::file_error::nested_error_type>,
     "Native API",
     "Testing that the native API implements the interface correctly",
     false,
     true},
    {url_api,
     "URL API",
     "Testing whether the platform's URL API is working as intended",
     false,
     true}};

COFFEE_EXEC_TESTS();
