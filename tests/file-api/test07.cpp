#include <coffee/core/CFiles>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>

#include <coffee/core/CUnitTesting>

using namespace Coffee;

using File = FileFun;
using Dir  = DirFun;

const Url testpath = MkUrl("test_level1/test_level2", RSCA::TemporaryFile);
const Url testdir  = MkUrl("test_dir", RSCA::TemporaryFile);

bool dirstat_test()
{
    file_error ec;

    if(!Dir::MkDir(testdir, false, ec))
        return false;
    Profiler::Profile("Creating single-level directory");

    if(File::Stat(testdir, ec) != FileType::Directory)
        return false;
    Profiler::Profile("lstat() on file, success");

    if(!Dir::RmDir(testdir, ec))
        return false;
    Profiler::Profile("Deleting directory");

    if(File::Stat(testdir, ec) != FileType::None)
        return false;
    Profiler::Profile("lstat() on file, failure");

    return true;
}

bool dirbasename_test()
{
    file_error ec;

    static const constexpr cstring result = "test_level1";

    Path bname = DirFun::Dirname(testpath.internUrl.c_str(), ec);

    if(bname != Path(result))
    {
        cDebug("{0} != {1}", bname, result);
        return false;
    } else
        return true;
}

bool basename_test()
{
    file_error ec;

    if(Path(DirFun::Basename("/absolute/path/really/path", ec)) != Path("path"))
        return false;

    if(Path(DirFun::Basename("relative/path", ec)) != Path("path"))
        return false;

    if(Path(DirFun::Basename("", ec)) != Path("."))
        return false;

    if(Path(DirFun::Basename("nothing", ec)) != Path("nothing"))
        return false;

    if(Path(DirFun::Basename("/", ec)) != Path("/"))
        return false;

    return true;
}

bool dirlist_test()
{
    file_error ec;
    Url        file1 = testpath + Path{"file1"};
    Url        file2 = testpath + Path{"file 2"};
    Profiler::Profile("Concatenating paths");

    if(!(Dir::MkDir(testpath, true, ec) &&
         File::Touch(FileType::File, file1, ec)) &&
       File::Touch(FileType::File, file2, ec))
        return false;
    Profiler::Profile("Prerequisites");

    Dir::DirList list;

    if(!Dir::Ls(testpath, list, ec))
        return false;
    Profiler::Profile("Directory listing");

    cDebug("File entries:");
    for(Dir::DirItem_t const& e : list)
        cBasicPrint("{0} : {1}", e.name, (u32)e.type);
    Profiler::Profile("Printing names");

    File::Rm(file1, ec);
    File::Rm(file2, ec);
    Dir::RmDir(testpath, ec);
    Profiler::Profile("Deleting files");

    Profiler::Profile("Basename");
    auto pdir = C_CAST<Path>(testpath).dirname();
    Dir::RmDir(MkUrl(pdir.internUrl.c_str(), RSCA::TemporaryFile), ec);
    Profiler::Profile("Delete directory");

    return true;
}

COFFEE_TEST_SUITE(4) = {
    {dirbasename_test, "Directory basename", "Getting the directory of a file"},
    {basename_test, "Basename operation", "Getting the basename of paths"},

    {dirstat_test,
     "Directory testing",
     "Using lstat() to check directory status"},
    {dirlist_test, "Directory listing", "Listing entries in a directory"},
};

COFFEE_EXEC_TESTS();
