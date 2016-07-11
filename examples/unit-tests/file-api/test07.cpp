#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

using File = CResources::FileFun;
using Dir = CResources::DirFun;

const cstring testpath = "test_level1/test_level2";
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


bool dirbasename_test()
{
    static const constexpr cstring result = "test_level1";

    CString bname = Env::DirName(testpath);

    if(bname != result)
    {
        cDebug("{0} != {1}",bname,result);
        return false;
    }else
        return true;
}

bool basename_test()
{
    if(Env::BaseName("/absolute/path/really/path") != "path")
        return false;

    if(Env::BaseName("relative/path") != "path")
        return false;

    if(Env::BaseName("") != "")
        return false;

    if(Env::BaseName("nothing") != "nothing")
        return false;

    if(Env::BaseName("/") != ".")
        return false;

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
        cBasicPrint("{0} : {1}",e.name,(uint32)e.type);
    Profiler::Profile("Printing names");

    File::Rm(file1.c_str());
    File::Rm(file2.c_str());
    Dir::RmDir(testpath);
    Profiler::Profile("Deleting files");

    CString pdir = Env::DirName(testpath);
    Profiler::Profile("Basename");
    Dir::RmDir(pdir.c_str());
    Profiler::Profile("Delete directory");

    return true;
}

const constexpr CoffeeTest::Test _tests[4] = {
    {dirbasename_test,"Directory basename","Getting the directory of a file"},
    {basename_test,"Basename operation","Getting the basename of paths"},

    {dirstat_test,"Directory testing","Using lstat() to check directory status"},
    {dirlist_test,"Directory listing","Listing entries in a directory"},
};

COFFEE_RUN_TESTS(_tests);
