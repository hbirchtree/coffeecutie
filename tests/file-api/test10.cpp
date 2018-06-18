#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>

using namespace Coffee;

bool url_bad_deref()
{
    /**/
    Url f = {};

    try
    {
        auto deref = *f;
        return false;
    } catch(std::runtime_error const&)
    {
        return true;
    }
}

bool url_get_dereferenced()
{
    Url f = MkUrl("testfile");

    try
    {
        auto deref = *f;
        return true;
    } catch(std::runtime_error const&)
    {
        return false;
    }
}

template<RSCA Storage>
bool url_get_storage()
{
    Url f = MkUrl("testfile", Storage);

    return (*f).size() != 0;
}

bool url_path_get_extension()
{
    Path p("test.txt");
    if(p.extension() != "txt")
        return false;

    p = p.removeExt();

    if(p.fileBasename().internUrl != "test")
        return false;

    return true;
}

bool url_path_get_extension_extra()
{
    return Path("directory.dir/test").extension() == "";
}

bool url_path_get_concat()
{
    Path p("test.txt");

    Path r = Path("basedir") + p;
    return r.internUrl == "basedir/test.txt";
}

bool url_path_get_basename()
{
    return Path("dir/file.txt").fileBasename().internUrl == "file.txt";
}

bool url_path_get_dirname()
{
    return Path("dir/file.txt").dirname().internUrl == "dir";
}

bool url_path_get_from_url()
{
    Path p = MkUrl("test.txt");

    return p.internUrl == "test.txt";
}

bool url_get_string()
{
    cStringFormat("{0}", Path("dir/file.txt"));
    cStringFormat("{0}", MkUrl("test.txt"));
    return true;
}

COFFEE_TEST_SUITE(14) = {
    {url_bad_deref, "Url bad dereferencing"},
    {url_get_dereferenced, "Url dereferencing"},
    {url_get_storage<RSCA::SystemFile>, "Url dereference class", "SystemFile"},
    {url_get_storage<RSCA::AssetFile>, "Url dereference class", "AssetFile"},
    {url_get_storage<RSCA::ConfigFile>, "Url dereference class", "ConfigFile"},
    {url_get_storage<RSCA::TemporaryFile>, "Url dereference class", "TempFile"},
    {url_get_storage<RSCA::CachedFile>, "Url dereference class", "CachedFile"},
    {url_path_get_extension, "Path get extension"},
    {url_path_get_extension_extra, "Path get extension", "Edge case"},
    {url_path_get_concat, "Path get concatenated"},
    {url_path_get_basename, "Path get basename"},
    {url_path_get_dirname, "Path get dirname"},
    {url_get_string, "Path get to_string"},
    {url_path_get_from_url, "Path get from Url"},
};

COFFEE_EXEC_TESTS()
