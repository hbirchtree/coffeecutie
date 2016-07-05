#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

const cstring testfile = "fileapi_testfile.txt";

using Resource = CResources::Resource;
using File = CResources::FileFun;
using Dir = CResources::DirFun;

bool filedel_test()
{
    if(File::Exists(testfile))
    {
        cDebug("Unclean environment! Remove file: {0}",testfile);
        return false;
    }
    if(!File::Touch(File::File,testfile))
        return false;

    if(!File::Exists(testfile))
        return false;

    if(!File::Rm(testfile))
        return false;

    return !File::Exists(testfile);
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

const cstring testdir = "test_dir";
bool dirstat_test()
{
    if(!Dir::MkDir(testdir,false))
        return false;

    if(File::Stat(testdir) != File::Directory)
        return false;

    if(!Dir::RmDir(testdir))
        return false;

    if(File::Stat(testdir) != File::None)
        return false;

    return true;
}

const constexpr CoffeeTest::Test _tests[3] = {
    {filedel_test,"File handling","Creating and deleting a file"},
    {filestat_test,"File testing","Using stat() to check file status"},
    {dirstat_test,"Directory testing","Using stat() to check directory status"}
};

COFFEE_RUN_TESTS(_tests);
