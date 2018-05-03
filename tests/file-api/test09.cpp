#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

using FileInterface = FileFunDef;
using NativeFileInterface = FileFun;

template<typename FileApi,
         typename implements<FileFunDef, FileApi>::type* = nullptr>
bool api_test()
{
    /* This test suite tests whether the function signature of the
     *  file API is correct. If it is not, compilation will fail.
     */

    const auto testingUrl = MkUrl("testfile.dat");

    /* File handle API */
    auto hnd = FileApi::Open(testingUrl,
                             ResourceAccess::NewFile
                             |ResourceAccess::Discard
                             |ResourceAccess::WriteOnly);
    FileApi::Seek(hnd, 0);
    FileApi::Read(hnd, 1, false);
    FileApi::Write(hnd, Bytes(), false);
    FileApi::Size(hnd);
    FileApi::Close(hnd);

    /* Mapping API */
    int _nothing = 0;
    auto map_hnd = FileApi::Map(testingUrl, ResourceAccess::ReadOnly,
                                10, 0, &_nothing);
    FileApi::MapCache(map_hnd.ptr, map_hnd.size,
                      0, 8);
    FileApi::MapUncache(map_hnd.ptr, map_hnd.size,
                        0, 8);
    FileApi::MapSync(map_hnd.ptr, map_hnd.size);
    FileApi::Unmap(&map_hnd);

    /* Plain URL API */
    FileApi::CanonicalName(testingUrl);
    FileApi::DereferenceLink(testingUrl);
    FileApi::Exists(testingUrl);
    FileApi::Size(testingUrl);
    FileApi::Touch(FileApi::NodeType::File, testingUrl);
    FileApi::Stat(testingUrl);
    FileApi::Ln(testingUrl, testingUrl);
    FileApi::Rm(testingUrl);

    return true;
}

bool test_url_version(RSCA access)
{
	try {
		Url test = MkUrl("test", access);

		CString test_string = *test;

		cDebug("URL mapping({1}): test -> {0}", test_string, StrUtil::pointerify(C_CAST<u32>(access)));
	}
    catch (std::runtime_error const&)
	{
		return false;
	}
	return true;
}

bool url_api()
{
	do {
		if (!test_url_version(RSCA::SystemFile))
			break;
		if (!test_url_version(RSCA::AssetFile))
			break;
		if (!test_url_version(RSCA::ConfigFile))
			break;
		if (!test_url_version(RSCA::TemporaryFile))
			break;
		if (!test_url_version(RSCA::CachedFile))
			break;

		return true;
	} while (false);

	return false;
}

COFFEE_TEST_SUITE(3) = {
    {api_test<FileInterface>, "Interface",
     "Testing that the unimplemented interface is properly tested",
     false, true},
    {api_test<NativeFileInterface>, "Native API",
     "Testing that the native API implements the interface correctly",
     false, true},
	 {url_api, "URL API",
	 "Testing whether the platform's URL API is working as intended",
	 false, true}
};

COFFEE_EXEC_TESTS();
