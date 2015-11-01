#include "data_types.h"

#include "coffee/core/coffee.h"
#include "coffee/core/base/cdebug.h"
#include "coffee/core/base/cmath.h"

#include <glbinding/gl/gl.h>

namespace CoffeeTests{

using namespace Coffee;

void vector2_test()
{
    CVec2 from_coffee(1.2f,3.4f);
    CMath::vec2 *to_glm = reinterpret_cast<CMath::vec2*>(&from_coffee);

    //Test for XY component, GLM to Coffee
    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y));

    //Test for ST component
    CASSERT((from_coffee.s==to_glm->s&&from_coffee.t==to_glm->t));
}

void vector3_test()
{
    CVec3 from_coffee(1.2f,3.4f,5.6f);
    CMath::vec3 *to_glm = reinterpret_cast<CMath::vec3*>(&from_coffee);

    //Test for XYZ component, Coffee to GLM
    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y&&from_coffee.z==to_glm->z));

    //Test for RGB component
    CASSERT((from_coffee.r==to_glm->r&&from_coffee.g==to_glm->g&&from_coffee.b==to_glm->b));

    //Test for STP component
    CASSERT((from_coffee.s==to_glm->s&&from_coffee.t==to_glm->t&&from_coffee.p==to_glm->p));
}

void vector4_test()
{
    CVec4 from_coffee(1.2f,3.4f,5.6f,7.8f);
    CMath::vec4 *to_glm = reinterpret_cast<CMath::vec4*>(&from_coffee);

    //Test for XYZW component, Coffee to GLM
    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y&&from_coffee.z==to_glm->z&&from_coffee.w==to_glm->w));

    //Test for RGBA component
    CASSERT((from_coffee.r==to_glm->r&&from_coffee.g==to_glm->g&&from_coffee.b==to_glm->b&&from_coffee.a==to_glm->a));
}

void quaternion_test()
{
    CQuat from_coffee(1.2f,3.4f,5.6f,7.8f);
    CMath::quat *to_glm = reinterpret_cast<CMath::quat*>(&from_coffee);

    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y&&from_coffee.z==to_glm->z&&from_coffee.w==to_glm->w));
}

void matrix_test()
{
    CMat4 from_coffee;
    from_coffee.m[0][0] = 1.f;
    from_coffee.m[2][3] = 2.f;
    from_coffee.m[3][1] = 3.f;
    CMath::mat4 *to_glm = reinterpret_cast<CMath::mat4*>(&from_coffee);

    CASSERT((from_coffee.m[0][0]==(*to_glm)[0][0]&&
            from_coffee.m[2][3]==(*to_glm)[2][3]&&
            from_coffee.m[3][1]==(*to_glm)[3][1]));
}

void vector_tests(bool silent)
{
    if(!silent)
        cMsg("Coffee Unit Tests","Vec2 test starting");
    vector2_test();
    if(!silent){
        cMsg("Coffee Unit Tests","Vec2 test passed");
        cMsg("Coffee Unit Tests","Vec3 test starting");
    }
    vector3_test();
    if(!silent){
        cMsg("Coffee Unit Tests","Vec3 test passed");
        cMsg("Coffee Unit Tests","Vec4 test starting");
    }
    vector4_test();
    if(!silent){
        cMsg("Coffee Unit Tests","Vec4 test passed");
        cMsg("Coffee Unit Tests","Quat test starting");
    }
    quaternion_test();
    if(!silent)
        cMsg("Coffee Unit Tests","Quat test passed");
    matrix_test();
    if(!silent)
        cMsg("Coffee Unit Tests","The Matrix test passed");
}

void int_tests()
{
    //We want this for safe buffer operations
    CASSERT((sizeof(gl::GLint64)==sizeof(int64)));
    CASSERT((sizeof(gl::GLint)==sizeof(int32)));
    CASSERT((sizeof(gl::GLuint)==sizeof(uint32)));
    CASSERT((sizeof(gl::GLuint64)==sizeof(uint64)));

    CASSERT((sizeof(byte)==sizeof(gl::GLbyte)));
    CASSERT((sizeof(byte)==sizeof(gl::GLchar)));

    CASSERT((sizeof(uint32)==sizeof(int)));
    CASSERT((sizeof(uint64)==sizeof(unsigned long long)));

    //Check for unwanted integer overflow, ensure consistency
    uint8 n8 = std::pow(2,8)-1;
    CASSERT((n8==std::pow(2,8)-1));

    uint16 n16 = std::pow(2,16)-1;
    CASSERT((n16==std::pow(2,16)-1));

    uint32 n32 = std::pow(2,32)-1;
    CASSERT((n32==std::pow(2,32)-1));
}

void floating_tests()
{
    CASSERT((sizeof(Coffee::scalar)==sizeof(float)));
    CASSERT((sizeof(Coffee::bigscalar)==sizeof(double)));

    CASSERT((sizeof(float)==4));
    CASSERT((sizeof(double)==8));

    CASSERT((sizeof(float)==sizeof(gl::GLfloat)));
    CASSERT((sizeof(double)==sizeof(gl::GLdouble)));
}

void run_tests(bool silent)
{
    vector_tests(silent);
    if(!silent)
        cMsg("Coffee Unit Tests","Integer tests starting");
    int_tests();
    if(!silent){
        cMsg("Coffee Unit Tests","Integer tests passed");
        cMsg("Coffee Unit Tests","Floating-point tests starting");
    }
    floating_tests();
    if(!silent)
        cMsg("Coffee Unit Tests","Floating-point tests passed");
}

}
