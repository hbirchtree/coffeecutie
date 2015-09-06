#ifndef DATA_TYPES
#define DATA_TYPES

#include "coffee/cdebug.h"
#include "coffee_impl/graphics/cgraphicsdata.h"

namespace CoffeeTests{

static void vector2_test()
{
    glm::vec2 from_glm(1.2f,3.4f);
    CVec2 *to_coffee = reinterpret_cast<CVec2*>(&from_glm);

    CASSERT((from_glm.x==to_coffee->x&&from_glm.y==to_coffee->y));

    CVec2 from_coffee(1.2f,3.4f);
    glm::vec2 *to_glm = reinterpret_cast<glm::vec2*>(&from_coffee);

    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y));

    try{
        CASSERT((from_coffee.x==from_glm.y));
    }catch(std::runtime_error e){
        return;
    }
    throw std::runtime_error("Vector2 test failed!");
}

static void vector3_test()
{
    glm::vec3 from_glm(1.2f,3.4f,5.6f);
    CVec3 *to_coffee = reinterpret_cast<CVec3*>(&from_glm);

    CASSERT((from_glm.x==to_coffee->x&&from_glm.y==to_coffee->y&&from_glm.z==to_coffee->z));

    CVec3 from_coffee(1.2f,3.4f,5.6f);
    glm::vec3 *to_glm = reinterpret_cast<glm::vec3*>(&from_coffee);

    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y&&from_coffee.z==to_glm->z));

    try{
        CASSERT((from_coffee.x==from_glm.y));
    }catch(std::runtime_error e){
        return;
    }
    throw std::runtime_error("Vector3 test failed!");
}

static void vector4_test()
{
    glm::vec4 from_glm(1.2f,3.4f,5.6f,7.8f);
    CVec4 *to_coffee = reinterpret_cast<CVec4*>(&from_glm);

    CASSERT((from_glm.x==to_coffee->x&&from_glm.y==to_coffee->y&&from_glm.z==to_coffee->z&&from_glm.w==to_coffee->w));

    CVec4 from_coffee(1.2f,3.4f,5.6f,7.8f);
    glm::vec4 *to_glm = reinterpret_cast<glm::vec4*>(&from_coffee);

    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y&&from_coffee.z==to_glm->z&&from_coffee.w==to_glm->w));

    try{
        CASSERT((from_coffee.x==from_glm.y));
    }catch(std::runtime_error e){
        return;
    }
    throw std::runtime_error("Vector4 test failed!");
}

static void quaternion_test()
{
    glm::quat from_glm(1.2f,3.4f,5.6f,7.8f);
    CQuat *to_coffee = reinterpret_cast<CQuat*>(&from_glm);

    CASSERT((from_glm.x==to_coffee->x&&from_glm.y==to_coffee->y&&from_glm.z==to_coffee->z&&from_glm.w==to_coffee->w));

    CQuat from_coffee(1.2f,3.4f,5.6f,7.8f);
    glm::quat *to_glm = reinterpret_cast<glm::quat*>(&from_coffee);

    CASSERT((from_coffee.x==to_glm->x&&from_coffee.y==to_glm->y&&from_coffee.z==to_glm->z&&from_coffee.w==to_glm->w));

    try{
        CASSERT((from_coffee.x==from_glm.y));
    }catch(std::runtime_error e){
        return;
    }
    throw std::runtime_error("Quaternion test failed!");
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

