#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

using File = CResources::FileFun;
using Dir = CResources::DirFun;

const Url testpath = MkUrl("test_level1/test_level2",
                           ResourceAccess::SpecifyStorage
                           |ResourceAccess::TemporaryFile);
const Url testdir = MkUrl("test_dir",
                          ResourceAccess::SpecifyStorage
                          |ResourceAccess::TemporaryFile);

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

    CString bname = Env::DirName(testpath.internUrl.c_str());

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

    if(Env::BaseName("") != ".")
        return false;

    if(Env::BaseName("nothing") != "nothing")
        return false;

    if(Env::BaseName("/") != "/")
        return false;

    return true;
}

bool dirlist_test()
{
    Url file1 = testpath + Path{"file1"};
    Url file2 = testpath + Path{"file 2"};
    Profiler::Profile("Concatenating paths");

    if(!(Dir::MkDir(testpath,true)
         && File::Touch(File::File,file1))
         && File::Touch(File::File,file2))
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

    File::Rm(file1);
    File::Rm(file2);
    Dir::RmDir(testpath);
    Profiler::Profile("Deleting files");

    Profiler::Profile("Basename");
    auto pdir = C_CAST<Path>(testpath).dirname();
    Dir::RmDir(MkUrl(pdir.internUrl.c_str(),
                     ResourceAccess::SpecifyStorage
                     |ResourceAccess::TemporaryFile));
    Profiler::Profile("Delete directory");

    return true;
}

COFFEE_TEST_SUITE(4) = {
    {dirbasename_test,"Directory basename","Getting the directory of a file"},
    {basename_test,"Basename operation","Getting the basename of paths"},

    {dirstat_test,"Directory testing","Using lstat() to check directory status"},
    {dirlist_test,"Directory listing","Listing entries in a directory"},
};

COFFEE_EXEC_TESTS();
