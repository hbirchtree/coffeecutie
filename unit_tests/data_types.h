#ifndef DATA_TYPES
#define DATA_TYPES

#include "coffee/cdebug.h"
#include "coffee_impl/graphics/cgraphicsdata.h"

namespace CoffeeTests{

static void vector2_test()
{
    CVec2 from_coffee(1.2f,3.4f);
    glm::vec2 *to_glm = reinterpret_cast<glm::vec2*>(&from_coffee);

    //Test for XY component, GLM to Coffee
    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y));

    //Test for ST component
    CASSERT((from_coffee.s==to_glm->s&&from_coffee.t==to_glm->t));
}

static void vector3_test()
{
    CVec3 from_coffee(1.2f,3.4f,5.6f);
    glm::vec3 *to_glm = reinterpret_cast<glm::vec3*>(&from_coffee);

    //Test for XYZ component, Coffee to GLM
    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y&&from_coffee.z==to_glm->z));

    //Test for RGB component
    CASSERT((from_coffee.r==to_glm->r&&from_coffee.g==to_glm->g&&from_coffee.b==to_glm->b));

    //Test for STP component
    CASSERT((from_coffee.s==to_glm->s&&from_coffee.t==to_glm->t&&from_coffee.p==to_glm->p));
}

static void vector4_test()
{
    CVec4 from_coffee(1.2f,3.4f,5.6f,7.8f);
    glm::vec4 *to_glm = reinterpret_cast<glm::vec4*>(&from_coffee);

    //Test for XYZW component, Coffee to GLM
    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y&&from_coffee.z==to_glm->z&&from_coffee.w==to_glm->w));

    //Test for RGBA component
    CASSERT((from_coffee.r==to_glm->r&&from_coffee.g==to_glm->g&&from_coffee.b==to_glm->b&&from_coffee.a==to_glm->a));
}

static void quaternion_test()
{
    CQuat from_coffee(1.2f,3.4f,5.6f,7.8f);
    glm::quat *to_glm = reinterpret_cast<glm::quat*>(&from_coffee);

    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y&&from_coffee.z==to_glm->z&&from_coffee.w==to_glm->w));
}

static void vector_tests()
{
    vector2_test();
    cMsg("Coffee Unit Tests","Vec2 test passed");
    vector3_test();
    cMsg("Coffee Unit Tests","Vec3 test passed");
    vector4_test();
    cMsg("Coffee Unit Tests","Vec4 test passed");
    quaternion_test();
    cMsg("Coffee Unit Tests","Quat test passed");
}

static void run_tests()
{
    vector_tests();
}


} //CoffeeTests

#endif // DATA_TYPES

