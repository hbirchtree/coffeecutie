#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

using File = CResources::FileFun;
using Dir = CResources::DirFun;

const cstring testdir = "test_dir";

bool dirstat_test()
{
    if(!Dir::MkDir(testdir,false))
        return false;
    Profiler::Profile("Creating single-level directory");

    if(File::Stat(testdir) != File::Directory)
        return false;
    Profiler::Profile("lstat() on file, success");

    if(!Dir::RmDir(testdir))
        return false;
    Profiler::Profile("Deleting directory");

    if(File::Stat(testdir) != File::None)
        return false;
    Profiler::Profile("lstat() on file, failure");

    return true;
}

const cstring testpath = "test_level1/test_level2";

bool dirbasename_test()
{
    static const constexpr cstring result = "test_level1";

    CString bname = Dir::Basename(testpath);

    if(bname != result)
    {
        cDebug("{0} != {1}",bname,result);
        return false;
    }

    return true;
}

bool dirlist_test()
{
    CString file1 = Env::ConcatPath(testpath,"file1");
    CString file2 = Env::ConcatPath(testpath,"file 2");
    Profiler::Profile("Concatenating paths");

    if(!(Dir::MkDir(testpath,true)
         && File::Touch(File::File,file1.c_str())
         && File::Touch(File::File,file2.c_str())))
        return false;
    Profiler::Profile("Prerequisites");

    Dir::DirList list;

    if(!Dir::Ls(testpath,list))
        return false;
    Profiler::Profile("Directory listing");

    cDebug("File entries:");
    for(Dir::DirItem_t const& e : list)
        cBasicPrint("{0} : {1}",e.name,e.type);
    Profiler::Profile("Printing names");

    File::Rm(file1.c_str());
    File::Rm(file2.c_str());
    Dir::RmDir(testpath);
    Profiler::Profile("Deleting files");

    CString pdir = Dir::Basename(testpath);
    Profiler::Profile("Basename");
    Dir::RmDir(pdir.c_str());
    Profiler::Profile("Delete directory");

    return true;
}

const constexpr CoffeeTest::Test _tests[3] = {
    {dirstat_test,"Directory testing","Using lstat() to check directory status"},
    {dirbasename_test,"Directory basename","Getting the basename of a directory"},
    {dirlist_test,"Directory listing","Listing entries in a directory"},
};

COFFEE_RUN_TESTS(_tests);
