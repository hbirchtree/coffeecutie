#include <unit_tests.h>

#include <coffee/graphics_apis/scene/cnode.h>
#include <coffee/graphics_apis/cgraphicsdata.h>
#include <coffee/core/coffee_macros.h>
#include <coffee/core/base/cmath.h>
#include <coffee/core/base/cmath_glm.h>
namespace CoffeeTests{

using namespace Coffee;

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
        CMat4 glm_perspective = CGraphicsData::GenPerspective(
                    camera.fieldOfView,
                    camera.aspect,
                    camera.zVals);
        CMat4 my_perspective = CGraphicsData::GenPerspective(
                    camera.fieldOfView,
                    camera.aspect,
                    camera.zVals);

        CASSERT_MEM(&glm_perspective,&my_perspective,sizeof(CMat4));
    }

    //Test VP generation
    {
        //Test quaternion conversion

        CMat4 my_quat = matrixify(normalize_quat(CQuat(1,0.2,0.2,0.2)));
        glm::mat4 glm_quat = glm::mat4_cast(glm::normalize(glm::quat(1,0.2,0.2,0.2)));

        CASSERT_MEM(&glm_quat,&my_quat,sizeof(CMat4));

        //Test projection matrices

        CMat4 my_matrix = CGraphicsData::GenPerspective(
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

        glm_matrix = glm::translate(glm_matrix,glm::vec3(4,5,6));
        my_matrix = translation(my_matrix,camera.position);

        CASSERT_MEM(&glm_matrix,&my_matrix,sizeof(CMat4));

        //Test model matrix

        CMat4 md_mat = CGraphicsData::GenTransform(
                    CVec3(1,2,3),CVec3(0.1,0.1,0.1),CQuat(1,0,0,0));

        glm::mat4 gmd_mat = glm::scale(glm::mat4(),glm::vec3(0.1));
        gmd_mat *= glm::mat4_cast(glm::quat(1,0,0,0));
        gmd_mat = glm::translate(gmd_mat,glm::vec3(1,2,3));

        CASSERT_MEM(&md_mat,&gmd_mat,sizeof(CMat4));

        //Accumulating the transformations

        my_matrix = my_matrix * md_mat;
        glm_matrix = glm_matrix * gmd_mat;

        CASSERT_MEM(&glm_matrix,&my_matrix,sizeof(CMat4));
    }

    //Test model transforms
    {
        CMat4 my_mat = scale(CMat4(),CVec3(1.2,1.2,1.2));
        glm::mat4 glm_mat = CMath::scale(glm::mat4(),glm::vec3(1.2,1.2,1.2));

        CASSERT_MEM(&glm_mat,&my_mat,sizeof(CMat4));

        my_mat *= matrixify(CQuat(1,0.0,0.5,0.0));
        glm_mat *= CMath::mat4_cast(glm::quat(1,0.0,0.5,0.0));

        CASSERT_MEM(&glm_mat,&my_mat,sizeof(CMat4));

        my_mat = translation(my_mat,CVec3(1,2,3));
        glm_mat = CMath::translate(glm_mat,glm::vec3(1,2,3));

        CASSERT_MEM(&glm_mat,&my_mat,sizeof(CMat4));
    }

    //Test node transformations
    {
        CGraphicsData::CTransform t1,t2,t3;

        t1.position = CVec3(1,2,3);
        t2.position = CVec3(4,5,6);
        t3.position = CVec3(7,8,9);

        t1.scale = CVec3(3);
        t2.scale = CVec3(0.5);
        t3.scale = CVec3(5);

        t1.rotation = t2.rotation = t3.rotation = CQuat(1,0,0.5,0);

        CMat4 t1m = CGraphicsData::GenTransform(t1);
        CMat4 t2m = CGraphicsData::GenTransform(t2);
        CMat4 t3m = CGraphicsData::GenTransform(t3);

        CGraphicsData::CNode root;
        root.transform = &t1m;
        CGraphicsData::CNode inherited;
        inherited.transform = &t2m;
        CGraphicsData::CNode inherited2;
        inherited2.transform = &t3m;

        inherited2.parent = &inherited;
        inherited.parent = &root;

        //        CMat4 d1 = CGraphicsData::coffee_node_get_transform(&inherited2);
        CMat4 d1 = t1m * (t2m * t3m);

        glm::vec3 p1(1,2,3);
        glm::vec3 p2(4,5,6);
        glm::vec3 p3(7,8,9);

        glm::vec3 s1(3);
        glm::vec3 s2(0.5);
        glm::vec3 s3(5);

        glm::quat r1(1,0,0.5,0);

        glm::mat4 m1 = glm::mat4_cast(r1)
                * glm::scale(glm::mat4(),s1)
                * glm::translate(glm::mat4(),p1);
        glm::mat4 m2 = glm::mat4_cast(r1)
                * glm::scale(glm::mat4(),s2)
                * glm::translate(glm::mat4(),p2);
        glm::mat4 m3 = glm::mat4_cast(r1)
                * glm::scale(glm::mat4(),s3)
                * glm::translate(glm::mat4(),p3);

        glm::mat4 d2 = m1 * (m2 * m3);

        CASSERT_MEM(&d1,&d2,sizeof(CMat4));
    }
}
}
