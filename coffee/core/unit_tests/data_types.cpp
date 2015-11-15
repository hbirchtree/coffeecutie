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
    CASSERT(c_memcmp(m1,m2,size));
}

void matrix_tests()
{
    CASSERT(sizeof(CQuat)==sizeof(glm::quat));
    CASSERT(sizeof(CMat4)==sizeof(glm::mat4));

    //Test quaternion cumulation
    {
        CQuat my_quat = CQuat(1,0,0.5,0)*CQuat(1,0,0.5,0.1);
        glm::quat glm_quat = glm::quat(1,0,0.5,0)*glm::quat(1,0,0.5,0.1);

        CASSERT_MEM(&my_quat,&glm_quat,sizeof(CQuat));
    }

    //Test matrix multiplication
    {
        CMat4 m1,m3;
        m1[0] = CVec4( 1, 2, 3, 4);
        m1[1] = CVec4( 5, 6, 7, 8);
        m1[2] = CVec4( 9,10,11,12);
        m1[3] = CVec4(13,14,15,16);

        CMat4 m2 = m1*m1;

        m3[0] = CVec4( 90,100,110,120);
        m3[1] = CVec4(202,228,254,280);
        m3[2] = CVec4(314,356,398,440);
        m3[3] = CVec4(426,484,542,600);

        CASSERT_MEM(&m2,&m3,sizeof(CMat4));
    }

    //Test data for camera transform

    CGraphicsData::CGCamera camera;
    camera.aspect = 1.6f;
    camera.fieldOfView = 60.f;
    camera.position.x() = 4;
    camera.position.y() = 5;
    camera.position.z() = 6;

    //Test perspective generation
    {
        CMat4 glm_perspective = CGraphicsData::_coffee_gen_perspective(
                    camera.fieldOfView,
                    camera.aspect,
                    camera.zVals);
        CMat4 my_perspective = CGraphicsData::coffee_graphics_gen_perspective(
                    camera.fieldOfView,
                    camera.aspect,
                    camera.zVals);

        CASSERT_MEM(&glm_perspective,&my_perspective,sizeof(CMat4));
    }

    //Test VP generation
    {
        //Test quaternion conversion

        CMat4 my_quat = CVectors::matrixify(camera.rotation);
        glm::mat4 glm_quat = glm::mat4_cast(*((glm::quat*)&camera.rotation));

        CASSERT_MEM(&glm_quat,&my_quat,sizeof(CMat4));

        //Test projection matrices

        CMat4 my_matrix = CGraphicsData::coffee_graphics_gen_perspective(
                    camera.fieldOfView,
                    camera.aspect,
                    camera.zVals);
        glm::mat4 glm_matrix = glm::perspective(glm::radians(camera.fieldOfView),
                                                camera.aspect,
                                                camera.zVals.near,camera.zVals.far);

        CASSERT_MEM(&glm_matrix,&my_matrix,sizeof(CMat4));

        //Test quaternions and matrix multiplication

        glm_matrix *= glm_quat;
        my_matrix *= my_quat;

        CASSERT_MEM(&glm_matrix,&my_matrix,sizeof(CMat4));

        //Test translation

        glm_matrix = glm::translate(glm_matrix,*((glm::vec3*)&camera.position));
        my_matrix = CVectors::translation(my_matrix,camera.position);

        CASSERT_MEM(&glm_matrix,&my_matrix,sizeof(CMat4));
    }

    //Test model transforms
    {
        CMat4 my_mat = CVectors::scale(CMat4(),CVec3(1.2,1.2,1.2));
        glm::mat4 glm_mat = CMath::scale(glm::mat4(),glm::vec3(1.2,1.2,1.2));

        CASSERT_MEM(&glm_mat,&my_mat,sizeof(CMat4));

        my_mat *= CVectors::matrixify(CQuat(1,0.0,0.5,0.0));
        glm_mat *= CMath::mat4_cast(glm::quat(1,0.0,0.5,0.0));

        CASSERT_MEM(&glm_mat,&my_mat,sizeof(CMat4));

        my_mat = CVectors::translation(my_mat,CVec3(1,2,3));
        glm_mat = CMath::translate(glm_mat,glm::vec3(1,2,3));

        CASSERT_MEM(&glm_mat,&my_mat,sizeof(CMat4));
    }
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
