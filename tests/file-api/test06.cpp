#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>

using namespace Coffee;

const Url testfile =
    MkUrl("fileapi_testfile.txt", RSCA::TemporaryFile);

using File = FileFun;

bool filedel_test()
{
    file_error ec;

    if(!File::Touch(FileType::File, testfile, ec))
        return false;

    if(!File::Exists(testfile, ec))
        return false;

    if(!File::Rm(testfile, ec))
        return false;

    return !File::Exists(testfile, ec);
}

bool check_literal_constructor()
{
    Resource r = "testfile.txt"_rsc;

    // This could be a compile-time test, but we'll do it at runtime
    return std::is_same<Resource, decltype(r)>::value;
}

bool check_move_constructor()
{
    Resource r1 = Resource(testfile);

    r1.data = C_RCAST<void*>(0x2);
    r1.size = 100;

    Resource r2 = std::move(r1);

    return r1.data == nullptr;
}

bool filestat_test()
{
    file_error ec;

    if(!File::Touch(FileType::File, testfile, ec))
        return false;
    if(File::Stat(testfile, ec) != FileType::File)
        return false;
    if(!File::Rm(testfile, ec))
        return false;
    if(File::Stat(testfile, ec) != FileType::None)
        return false;

    return true;
}

COFFEE_TESTS_BEGIN(4)

    {filedel_test, "File handling", "Creating and deleting a file"},
    {check_move_constructor,
     "Constructor test 1",
     "Verifying literal constructor"},
    {check_move_constructor,
     "Constructor test 2",
     "Verifying std::move for Resource"},
    {filestat_test, "File testing", "Using stat() to check file status"}

COFFEE_TESTS_END()
