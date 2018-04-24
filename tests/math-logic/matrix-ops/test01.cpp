#include <coffee/core/CUnitTesting>
#include <coffee/core/CMath>
#include <coffee/CGraphics>

using namespace Coffee;
using namespace CGraphicsData;

bool matrix_translation_test()
{
    CTransform testt;
    testt.position = CVec3(3,3,3);
    testt.scale = CVec3(1);
    testt.rotation = CQuat(2,1,1.1,1);

    CMat4 testmat;
    testmat *= GenTransform<scalar>(testt);
    testt.position = CVec3(3,2,3);
    testmat *= GenTransform<scalar>(testt);
    testt.position = CVec3(3,3,2);
    testmat *= GenTransform<scalar>(testt);

    CVec4 trans = get_translation(testmat);
    Profiler::Profile("Acquire matrix translation");

    /* TODO: Verify this result */
    cDebug("Translation: {0}",trans);

    return true;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {matrix_translation_test, "Matrix translation"}
};

COFFEE_RUN_TESTS(_tests);
