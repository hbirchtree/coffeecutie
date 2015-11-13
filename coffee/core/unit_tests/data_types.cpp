#include "data_types.h"

#include "coffee/core/coffee.h"
#include "coffee/core/base/cdebug.h"
#include "coffee/core/base/cmath.h"
#include "coffee/core/base/cmath_glm.h"
#include "coffee/core/graphics/cgraphicsdata.h"

#include <glbinding/gl/gl.h>

namespace CoffeeTests{

using namespace Coffee;

static void CASSERT_MEM(c_cptr m1, c_cptr m2, szptr size)
{
    CASSERT(c_memcmp(m1,m2,size)==0);
}

void matrix_tests()
{
    CASSERT(sizeof(CQuat)==sizeof(glm::quat));
    CASSERT(sizeof(CMat4)==sizeof(glm::mat4));

    CGraphicsData::CGCamera camera;
    camera.aspect = 1.6f;
    camera.fieldOfView = 60.f;

    //Test quaternion conversion

    CMat4 my_quat = CVectors::matrixify(camera.rotation);
    glm::mat4 glm_quat = glm::mat4_cast(*((glm::quat*)&camera.rotation));

    CASSERT_MEM(&glm_quat,&my_quat,sizeof(CMat4));

    //Test projection matrices

    CMat4 my_matrix = CGraphicsData::coffee_graphics_gen_matrix_perspective(&camera);
    glm::mat4 glm_matrix = glm::perspective(camera.fieldOfView,camera.aspect,
                                            camera.zVals.near,camera.zVals.far);

    CASSERT_MEM(&glm_matrix,&my_matrix,sizeof(CMat4));

    //Test matrix multiplication and rotation

    glm_matrix *= glm_quat;
    my_matrix = my_matrix*my_quat;

    CASSERT_MEM(&glm_matrix,&my_matrix,sizeof(CMat4));

    //Test translation

    glm_matrix = glm::translate(glm_matrix,*((glm::vec3*)&camera.position));
    my_matrix = CVectors::translate(my_matrix,camera.position);

    CASSERT_MEM(&glm_matrix,&my_matrix,sizeof(CMat4));
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
    if(!silent)
        cMsg("Coffee Unit Tests","Matrix tests starting");
    matrix_tests();
    if(!silent)
    {
        cMsg("Coffee Unit Tests","Matrix tests passed");
        cMsg("Coffee Unit Tests","Integer tests starting");
    }
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
