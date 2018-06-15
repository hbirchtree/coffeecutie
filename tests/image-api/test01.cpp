#include <coffee/core/CUnitTesting>
#include <coffee/image/cimage.h>

using namespace Coffee;

bool touch_functions()
{
    stb::stb_error ec;
    stb::image_rw  test;

    stb::LoadData(&test, {}, ec);

    stb::image_float test2;

    stb::LoadData(&test2, {}, ec);

    return true;
}

bool float_cast()
{
    static u8 test_image[] = {
        0x00, 0xFF, 0x0, 0xAA, 0x30, 0x0, 0x50, 0x30, 0x0};

    stb::image_const source = stb::image_const::From(test_image, {3, 3}, 1);

    stb::image_float output = stb::ToFloat(source);

    return CMath::floating_equal(output.data[1], 1.f) &&
           CMath::floating_equal(output.data[2], 0.f);
}

COFFEE_TEST_SUITE(2) = {{touch_functions, "Touching functions"},
                        {float_cast, "Casting an image to floating-point"}};

COFFEE_EXEC_TESTS()
