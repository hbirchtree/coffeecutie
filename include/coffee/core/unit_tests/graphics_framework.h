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

    application_data->setup = [&](Display::RendererInterface& r, Empty*)
    {
        result = run_tests(num, tests, argc, argv);

        r.closeWindow();
    };

    application_data->loop = [](Display::RendererInterface&,Empty*){};
    application_data->cleanup = [](Display::RendererInterface&,Empty*){};

    Display::AutoExec<API,Display::RendererInterface,Empty>(
                *application_data);

    return result;
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
