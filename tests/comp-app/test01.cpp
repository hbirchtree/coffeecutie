#include <coffee/core/CUnitTesting>

#include <coffee/comp_app/bundle.h>
#include <coffee/comp_app/services.h>

using comp_app::AppLoader;
using comp_app::AppService;
using compo::TagType;

struct SomeInterface
{
    virtual void do_thing() = 0;
};

struct SomeService : SomeInterface,
                     AppService<SomeService, TagType<SomeInterface>>
{
    virtual void do_thing() {}
};

bool casting_services()
{
    comp_app::app_error ec;
    auto& container = comp_app::createContainer();

    AppLoader& loader = AppLoader::register_service(container);
    loader.loadAll<type_safety::type_list_t<SomeService>>(container, ec);

    CASSERT(container.service<TagType<SomeInterface>>());
    CASSERT(container.service<SomeService>());

    return true;
}

COFFEE_TESTS_BEGIN(1)

    {casting_services, "Casting services between provided types"},

    COFFEE_TESTS_END()
