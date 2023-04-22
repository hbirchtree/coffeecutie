#include <coffee/core/CUnitTesting>
#include <peripherals/semantic/chunk.h>
#include <coffee/core/types/rgba.h>
#include <coffee/core/unit_tests/graphics_framework.h>

#include <coffee/graphics/apis/CGLeamRHI>

using namespace Coffee;

using GFX = RHI::GLEAM::GLEAM_API;

bool framebuffer_read()
{
    GFX::DefaultFramebuffer()->clear(0, Vecf4(0.0, 1.0, 0.0, 1.0));

    std::vector<byte_t> framebuffer;
    GFX::DumpFramebuffer(
        *GFX::DefaultFramebuffer(), PixFmt::RGBA8, BitFmt::UByte, framebuffer);

    rgba_t color = {0, 255, 0, 255};

    if(framebuffer.size() < sizeof(color))
        return false;

    return MemCmp(
        Bytes::CreateFrom(framebuffer).at(0, sizeof(color)),
        Bytes::From(color));
}

COFFEE_TESTS_BEGIN(1)

    {CoffeeTest::GraphicsWrap<framebuffer_read, GFX>,
     "Framebuffer read-back",
     "Verify that it is possible to read framebuffer pixels",
     false,
     false}

COFFEE_TESTS_END()
