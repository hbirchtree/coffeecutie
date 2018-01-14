#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

bool url_bad_deref()
{
    /**/
    Url f = {};

    try {
        auto deref = *f;
        return false;
    } catch (std::runtime_error const&) {
        return true;
    }
}

bool url_get_dereferenced()
{
    Url f = MkUrl("testfile");

    try {
        auto deref = *f;
        return true;
    } catch (std::runtime_error const&) {
        return false;
    }
}

template<RSCA Storage>
bool url_get_storage()
{
    Url f = MkUrl("testfile", Storage);

    return (*f).size() != 0;
}

COFFEE_TEST_SUITE(7) = {
{url_bad_deref, "Url bad dereferencing"},
{url_get_dereferenced, "Url dereferencing"},
{url_get_storage<RSCA::SystemFile>, "Url dereference class", "SystemFile"},
{url_get_storage<RSCA::AssetFile>, "Url dereference class", "AssetFile"},
{url_get_storage<RSCA::ConfigFile>, "Url dereference class", "ConfigFile"},
{url_get_storage<RSCA::TemporaryFile>, "Url dereference class", "TempFile"},
{url_get_storage<RSCA::CachedFile>, "Url dereference class", "CachedFile"}
        };

COFFEE_EXEC_TESTS()
