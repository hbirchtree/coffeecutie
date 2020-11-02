#include <coffee/comp_app/app_wrap.h>
#include <coffee/comp_app/bundle.h>
#include <coffee/core/CApplication>
#include <coffee/core/profiler/profiling-export.h>

#if defined(FEATURE_ENABLE_GLeamRHI)
#define HAS_GRAPHICS 1
#include <coffee/graphics/apis/CGLeamRHI>
using GFX = Coffee::RHI::GLEAM::GLEAM_API;
#elif defined(FEATURE_ENABLE_Gexxo)
#define HAS_GRAPHICS 1
#include <coffee/gexxo/gexxo_api.h>
using GFX = gexxo::GXAPI;
#endif

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

using namespace ::Coffee;

i32 systeminfo_main(i32, cstring_w*)
{
#if defined(HAS_GRAPHICS)
    using container_type = comp_app::AppContainer<u32>;

    auto& container = comp_app::createContainer();
    auto& loader    = comp_app::AppLoader::register_service(container);

    comp_app::app_error ec;
    comp_app::configureDefaults(loader);
    comp_app::addDefaults(container, loader, ec);

    container_type::addTo(
        container,
        [](decltype(container), u32, container_type::time_point const&) {
            if(GFX::GetLoadAPI()(true))
                GFX::UnloadAPI();
        });
#endif

    CString profile;
    Profiling::ExportChromeTracerData(profile);

    cOutputPrint("{0}", profile);

    return 0;
}

COFFEE_APPLICATION_MAIN(systeminfo_main)
