#pragma once

#include <coffee/core/CDebug>
#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/windowing/renderer/renderer.h>
#include <coffee/core/CUnitTesting>

#include <coffee/graphics/apis/CGLeamRHI>

namespace CoffeeTest
{

using API = Coffee::RHI::GLEAM::GLEAM_API;

int gfx_main_proper(Coffee::u32 num, Test const* tests,
                    int argc, char** argv)
{
    struct Empty
    {
    };

    using namespace Coffee;
    auto application_data = Display::MkEventLoop(
                Display::CreateRendererUq(),
                MkUq<Empty>());

    int result = 1;

#if defined(COFFEE_CUSTOM_EXIT_HANDLING)
    application_data->setup = [=](Display::RendererInterface& r, Empty*)
    {
        int result = run_tests(num, tests, argc, argv);

        cDebug("CoffeeTest::result ==> {0}", result);

        r.closeWindow();
    };
#else
    application_data->setup = [&](Display::RendererInterface& r, Empty*)
    {
        result = run_tests(num, tests, argc, argv);

        r.closeWindow();
    };
#endif

    application_data->loop = [](Display::RendererInterface&,Empty*){};
    application_data->cleanup = [](Display::RendererInterface&,Empty*){};

    Display::AutoExec<API,Display::RendererInterface,Empty>(
                *application_data);

    return result;
}

template<bool(*TFun)(), typename GFX>
/*!
 * \brief Template for testing graphics API functionaltiy without
 *  writing all the API code every time.
 * \return
 */
bool GraphicsWrap()
{
    bool status = false;
    bool gfxLoaded = false;
    auto GFXLoader = GFX::GetLoadAPI();

    if(!GFXLoader(true))
    {
        status = false;
        goto unload;
    }
    gfxLoaded = true;

    status = TFun();

unload:
    if(gfxLoaded)
        GFX::UnloadAPI();

    return status;
}

}

#define COFFEE_GFX_RUN_TESTS(test_list) \
    int ref_main(int argc, char** argv){ \
        size_t num = sizeof(test_list)/sizeof(CoffeeTest::Test); \
        \
        return CoffeeTest::gfx_main_proper(C_CAST<Coffee::u32>(num),\
                                           test_list,\
                                           argc, argv); \
    } \
    COFFEE_APPLICATION_MAIN(ref_main)
