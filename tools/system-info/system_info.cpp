#include <coffee/comp_app/app_wrap.h>
#include <coffee/comp_app/bundle.h>
#include <coffee/core/CApplication>
#include <coffee/core/profiler/profiling-export.h>

#if defined(FEATURE_ENABLE_GLeamRHI_ES)
#define HAS_GRAPHICS 1
#include <coffee/graphics/apis/CGLeam>
using GFX = gleam::api;
#elif defined(FEATURE_ENABLE_Gexxo)
#define HAS_GRAPHICS 1
#include <coffee/gexxo/gexxo_api.h>
using GFX = gexxo::GXAPI;
#endif

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/net_profiling.h>
#endif

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

using namespace ::Coffee;

i32 systeminfo_main(i32, cstring_w*)
{
#if defined(FEATURE_ENABLE_ASIO)
    auto _ = Net::RegisterProfiling();
#endif

#if defined(HAS_GRAPHICS)
    using container_type = comp_app::AppContainer<u32>;

    auto& container = comp_app::createContainer();
    auto& loader    = comp_app::AppLoader::register_service(container);

    cDebug("Checking graphics...");

    comp_app::app_error ec;
    comp_app::configureDefaults(loader);
    comp_app::addDefaults(container, loader, ec);
    C_ERROR_CHECK(ec);

    container_type::addTo(
        container,
        [](decltype(container)& e, u32, container_type::time_point const&) {
            auto api = GFX();
            api.load();

            ExtraData::Add("graphics:api", api.api_name());
            auto [major, minor] = api.api_version();
            ExtraData::Add("gl:version", Strings::fmt("{0}.{1}", major, minor));
            auto [vendor, renderer] = api.device();
            ExtraData::Add("gl:renderer", renderer);
            ExtraData::Add("gl:vendor", vendor);
            cDebug("Graphics success");

            auto battery = e.service<comp_app::BatteryProvider>();

            if(!battery)
                return;
            ExtraData::Add(
                "power:source",
                battery->source() ==
                        comp_app::interfaces::BatteryProvider::PowerSource::AC
                    ? "ac"
                    : "battery");
            ExtraData::Add(
                "power:level", std::to_string(battery->percentage()));
        });
    container.service<container_type>()->load(container, ec);
    C_ERROR_CHECK(ec);

#endif

    CString profile;
    Profiling::ExportChromeTracerData(profile);

    cOutputPrint("{0}", profile);

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM(systeminfo_main, 0x2)
