#ifndef CGRAPHICSDATA_H
#define CGRAPHICSDATA_H

#include "coffee.h"
#include "coffee/cmath.h"

namespace Coffee{
namespace CGraphicsData{

struct CZField
{
    scalar near = 0;
    scalar far  = 0;
};

struct CGCamera
{
    enum CameraFlags{
        OrthographicFlag        = 0x1, //When off, assume perspective
    };

    byte        flags            = 0;

    scalar      aspect           = 1.f;
    CZField     zVals;
    scalar      fieldOfView      = 90.f;

    CMath::vec3   position;
    CMath::quat   rotation;

    CRectF      orthoview;

    CMath::mat4 matrix;
};

struct CModelTransform
{
    CMath::vec3   position;
    CMath::quat   rotation;
    CMath::vec3   scale;

    CMath::mat4 matrix;
};

/*!
 * \brief Update a model transform matrix
 * \param mat
 */
extern void coffee_graphics_gen_matrix(CModelTransform* mat);
/*!
 * \brief Update projection matrix for camera
 * \param cam
 */
extern void coffee_graphics_gen_matrix_perspective(CGCamera* cam);
/*!
 * \brief Update orthographic matrix for camera
 * \param cam
 */
extern void coffee_graphics_gen_matrix_orthographic(CGCamera* cam);

struct CBlock
        //A block of data, f.ex. a light or material
        //Lights should not be treated differently than other uniform blocks
{
    uint16    blockSize       = 0; //The size of the whole block, including this struct
    uint16    numProperties   = 0; //Determines size of types* and sizes*
    uint8*    propertyTypes   = nullptr;
    uint16*   propertySizes   = nullptr;

    uint16 dataSize() const;
    void*       dataPtr         () const;
    void        setPropertyData (uint16 index, const void* data, uint16 size);

    void*       data_ptr        = nullptr;
    uint16    data_size       = 0;
};

//Datasize: Size of data without struct
extern CBlock* coffee_create_block(uint16 dataSize,
                                   uint16 numProperties,
                                   szptr* sizes = nullptr);

} //CGraphicsData
} //Coffee

#endif // CGRAPHICSDATA_H
