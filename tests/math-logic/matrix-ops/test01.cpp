#include <coffee/core/CMath>
#include <coffee/core/CUnitTesting>
#include <coffee/core/types/graphics_types.h>

using namespace Coffee;
using namespace SceneGraph;

bool matrix_translation_test()
{
    Transform testt;
    testt.position = Vecf3(3, 3, 3);
    testt.scale    = Vecf3(1);
    testt.rotation = Quatf(2, 1, 1.1, 1);

    Matf4 testmat;
    testmat *= GenTransform<scalar>(testt);
    testt.position = Vecf3(3, 2, 3);
    testmat *= GenTransform<scalar>(testt);
    testt.position = Vecf3(3, 3, 2);
    testmat *= GenTransform<scalar>(testt);

    Vecf4 trans = get_translation(testmat);
    Profiler::Profile("Acquire matrix translation");

    /* TODO: Verify this result */
    cDebug("Translation: {0}", trans);

    return true;
}

bool matrix_decompose()
{
    Matf4 rotation_xf = matrixify(normalize_quat(Quatf{1.f, 0.f, 0.5f, 0.3f}));

    Matf4 xf = translation(Matf4(), {1, 2, 3}) * rotation_xf *
               scale(Matf4(), {20, 30, 40});

    Vecf3 trans_    = decompose::get<decompose::translation>(xf);
    Vecf3 scale_    = decompose::get<decompose::scale>(xf);
    Matf4 rotation_ = decompose::get<decompose::rotation>(xf);

    xf = translation(Matf4(), {1, 2, 3}) * scale(Matf4(), {20, 30, 40}) *
         rotation_xf;

    scale_    = decompose::get<decompose::scale>(xf);
    rotation_ = decompose::get<decompose::rotation_post>(xf);

    return true;
}

const constexpr CoffeeTest::Test _tests[2] = {
    {matrix_translation_test, "Matrix translation"},
    {matrix_decompose, "Matrix decomposition"}};

COFFEE_RUN_TESTS(_tests);
