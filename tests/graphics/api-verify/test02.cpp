#include <coffee/core/unit_tests/graphics_framework.h>
#include <coffee/interfaces/cgraphics_api.h>

#if defined(COFFEE_GLEAM_DESKTOP)
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/graphics/apis/CGLeam>
#endif

using namespace Coffee;

//using GFXAPI = RHI::NullAPI;

template<typename GFXAPI>
void create_tex_format(PixFmt fmt, PixCmp cmp)
{
    typename GFXAPI::S_2D surface(fmt, 1, 0);

    surface.allocate({128, 128}, cmp);

    surface.dealloc();
}

template<typename GFXAPI>
bool test_texture_formats()
{
    using A = GFXAPI;

    for(auto i : Range<>(C_CAST<u32>(PixFmt::MAX_PIXFMT) - 1))
    {
        auto fmt = C_CAST<PixFmt>(i + 1);
        create_tex_format<A>(fmt, convert::to<PixCmp>(fmt));
    }

    return true;
}

using GFX = RHI::GLEAM::GLEAM_API;

COFFEE_TEST_SUITE(1) = {
{
        CoffeeTest::GraphicsWrap<test_texture_formats<GFX>, GFX>,
        "Texture format verification"
}
};

COFFEE_GFX_RUN_TESTS(_tests);
