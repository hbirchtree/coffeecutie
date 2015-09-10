#ifndef DATA_TYPES
#define DATA_TYPES

#include "coffee/cdebug.h"
#include "coffee_impl/graphics/cgraphicsdata.h"

#include <glbinding/gl/gl.h>

/*
 * Most of development has been done purely on 64-bit systems.
 * I have no idea how it will react to 32-bit.
 * As such, I have formulated these tests to check for most of the important assumptions
 *   we are operating under.
 * If something fails here, the rest will surely not work.
 *
 */

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

static void vector_tests(bool silent = true)
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
}

static void int_tests()
{
    //We want this for safe buffer operations
    CASSERT((sizeof(gl::GLint64)==sizeof(int64_t)));
    CASSERT((sizeof(gl::GLint)==sizeof(int32_t)));
    CASSERT((sizeof(gl::GLuint)==sizeof(uint32_t)));
    CASSERT((sizeof(gl::GLuint64)==sizeof(uint64_t)));

    CASSERT((sizeof(Coffee::byte)==sizeof(gl::GLbyte)));
    CASSERT((sizeof(char)==sizeof(gl::GLchar)));

    CASSERT((sizeof(uint32_t)==sizeof(int)));
    CASSERT((sizeof(uint64_t)==sizeof(unsigned long long)));

    CASSERT((sizeof(uintptr_t)==sizeof(void*)));
}

static void floating_tests()
{
    //Just to make sure...
    CASSERT((sizeof(Coffee::scalar)==sizeof(float)));
    CASSERT((sizeof(Coffee::bigscalar)==sizeof(double)));

    //I want to know when this is not valid, okay?!
    CASSERT((sizeof(float)==4));
    CASSERT((sizeof(double)==8));

    CASSERT((sizeof(float)==sizeof(gl::GLfloat)));
    CASSERT((sizeof(double)==sizeof(gl::GLdouble)));
}

static void run_tests(bool silent = true)
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


} //CoffeeTests

#endif // DATA_TYPES

