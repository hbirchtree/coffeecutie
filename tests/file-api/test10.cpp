#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>

using namespace assertion;

using namespace Coffee;

bool url_bad_deref()
{
#ifdef COFFEE_USE_EXCEPTIONS
    Url f = {};

    try
    {
        auto deref = *f;
        return false;
    } catch(std::runtime_error const&)
    {
        return true;
    }
#else
    return true;
#endif
}

bool url_get_dereferenced()
{
#ifdef COFFEE_USE_EXCEPTIONS
    Url f = MkUrl("testfile");

    try
    {
        auto deref = *f;
        return true;
    } catch(std::runtime_error const&)
    {
        return false;
    }
#else
    return true;
#endif
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

bool url_parsing_http()
{
    const Url http_url = "http://coffee.birchtrees.me/some/path/with/Stuff"_web;
    const Url https_url     = "https://coffee.birchtrees.me/examples"_web;
    const Url http_port_url = "http://coffee.birchtrees.me:8080/stuff"_web;
    const Url http_port2_url = "http://coffee.birchtrees.me:8080"_web;

    auto http_parse = UrlParse::From(http_url);

    assertTrue(http_parse.valid());
    assertEquals(http_parse.protocol(), "http");
    assertEquals(http_parse.host(), "coffee.birchtrees.me");
    assertEquals(http_parse.port(), 0);
    assertEquals(http_parse.resource(), "some/path/with/Stuff");

    auto https_parse = UrlParse::From(https_url);

    assertTrue(https_parse.valid());
    assertEquals(https_parse.protocol(), "https");
    assertEquals(https_parse.host(), "coffee.birchtrees.me");
    assertEquals(https_parse.port(), 0);
    assertEquals(https_parse.resource(), "examples");

    auto http_port_parse = UrlParse::From(http_port_url);

    assertTrue(http_port_parse.valid());
    assertEquals(http_port_parse.protocol(), "http");
    assertEquals(http_port_parse.host(), "coffee.birchtrees.me");
    assertEquals(http_port_parse.port(), 8080);
    assertEquals(http_port_parse.resource(), "stuff");

    auto http_port2_parse = UrlParse::From(http_port2_url);

    assertTrue(http_port2_parse.valid());
    assertEquals(http_port2_parse.protocol(), "http");
    assertEquals(http_port2_parse.host(), "coffee.birchtrees.me");
    assertEquals(http_port2_parse.port(), 8080);
    assertEquals(http_port2_parse.resource(), "");

    const Url http_bad_port_url  = "http://coffee.birchtrees.me:8d/stuff"_web;
    const Url http_bad_port2_url = "http://coffee.birchtrees.me:8d"_web;

    auto http_bad_port_parse = UrlParse::From(http_bad_port_url);
    assertFalse(http_bad_port_parse.valid());

    auto http_bad_port2_parse = UrlParse::From(http_bad_port2_url);
    assertFalse(http_bad_port2_parse.valid());

    return true;
}

bool url_parsing_custom_uri()
{
    const Url custom_url = "me.birchtrees.tests://path-_$stuff!/with/comp"_web;

    auto custom_parse = UrlParse::From(custom_url);

    assertTrue(custom_parse.valid());
    assertEquals(custom_parse.protocol(), "me.birchtrees.tests");
    assertEquals(custom_parse.host(), "path-_$stuff!");
    assertEquals(custom_parse.resource(), "with/comp");

    return true;
}

COFFEE_TEST_SUITE(16) = {
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
    {url_parsing_http, "HTTP URL parsing"},
    {url_parsing_custom_uri, "Parsing custom URI scheme"},
};

COFFEE_EXEC_TESTS()
