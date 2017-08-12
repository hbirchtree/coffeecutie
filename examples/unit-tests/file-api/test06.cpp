#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

const cstring testfile = "fileapi_testfile.txt";

using Resource = CResources::Resource;
using File = CResources::FileFun;

bool filedel_test()
{
    if(!File::Touch(File::File,testfile))
        return false;

    if(!File::Exists(testfile))
        return false;

    if(!File::Rm(testfile))
        return false;

    return !File::Exists(testfile);
}

bool check_literal_constructor()
{
    auto r = "testfile.txt"_rsc;

    // This could be a compile-time test, but we'll do it at runtime
    return std::is_same<Resource, decltype (r)>::value;
}

bool check_move_constructor()
{
    Resource r1 = Resource(testfile,
                           ResourceAccess::SpecifyStorage|
                           ResourceAccess::AssetFile);

    bool status = true;

    FileMap(r1, ResourceAccess::ReadWrite|ResourceAccess::NewFile, 100);

    Resource r2 = std::move(r1);

    if(r1.data)
        status = false;

    FileUnmap(r1);
    FileUnmap(r2);

    return status;
}

bool filestat_test()
{
    if(!File::Touch(File::File,testfile))
        return false;
    if(File::Stat(testfile) != File::File)
        return false;
    if(!File::Rm(testfile))
        return false;
    if(File::Stat(testfile) != File::None)
        return false;

    return true;
}

const constexpr CoffeeTest::Test _tests[4] = {
    {filedel_test,"File handling","Creating and deleting a file"},
    {check_move_constructor,"Constructor test 1","Verifying literal constructor"},
    {check_move_constructor,"Constructor test 2","Verifying std::move for Resource"},
    {filestat_test,"File testing","Using stat() to check file status"},
};

COFFEE_RUN_TESTS(_tests);
