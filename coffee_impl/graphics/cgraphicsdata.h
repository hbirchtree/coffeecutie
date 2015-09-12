#ifndef CGRAPHICSDATA_H
#define CGRAPHICSDATA_H

#include "coffee.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

namespace Coffee{
namespace CGraphicsData{

struct CZField
{
    scalar near = 0.f;
    scalar far  = 0.f;
};

struct CGCamera
{
    enum CameraFlags{
        OrthographicFlag        = 0b1, //When off, assume perspective
    };

    byte        flags            = 0;

    scalar      aspect           = 1.f;
    CZField     zVals;
    scalar      fieldOfView      = 90.f;

    glm::vec3   position;
    glm::quat   rotation;

    CRectF      orthoview;

    glm::mat4 matrix;

    void genPerspective(){
        matrix = glm::perspective(glm::radians(fieldOfView),
                                  aspect,
                                  zVals.near,zVals.far);
        rotate();
        translate();
    }
    void genOrthographic(){
        matrix = glm::ortho(orthoview.x,orthoview.w,orthoview.y,orthoview.h,
                                  zVals.near,zVals.far);
        rotate();
        translate();
    }
    void rotate(){
        matrix *= glm::mat4_cast(rotation);
    }
    void translate(){
        matrix = glm::translate(matrix,position);
    }
};

struct CModelTransform
{
    glm::vec3   position;
    glm::quat   rotation;
    glm::vec3   scale;

    glm::mat4 matrix;

    void genMatrix(){
        matrix = glm::scale(glm::mat4(),scale);
        matrix = glm::translate(matrix,position)
                * glm::mat4_cast(rotation);
    }
};

struct CBlock
        //A block of data, f.ex. a light or material
        //Lights should not be treated differently than other uniform blocks
{
    uint16_t    blockSize       = 0; //The size of the whole block, including this struct
    uint16_t    numProperties   = 0; //Determines size of types* and sizes*
    uint8_t*    propertyTypes   = nullptr;
    uint16_t*   propertySizes   = nullptr;

    uint16_t    dataSize        () const;
    void*       dataPtr         () const;
    void        setPropertyData (uint16_t index, const void* data, uint16_t size);

    void*       data_ptr        = nullptr;
    uint16_t    data_size       = 0;
};

//Datasize: Size of data without struct
static CBlock* coffee_create_block(uint16_t dataSize, uint16_t numProperties)
{
    szptr chunk_size = dataSize
            +sizeof(CBlock)
            +numProperties*sizeof(uint8_t)
            +numProperties*sizeof(uint16_t);
    void* chunk = calloc(sizeof(byte),chunk_size); //We want all zeros instead of undefined
    byte* chunk_bytes = reinterpret_cast<byte*>(chunk);

    CBlock* block = reinterpret_cast<CBlock*>(chunk);
    block->numProperties = numProperties;
    block->blockSize = chunk_size;
    block->propertyTypes = reinterpret_cast<uint8_t*>(&chunk_bytes[sizeof(CBlock)]);
    block->propertySizes = reinterpret_cast<uint16_t*>(&chunk_bytes[sizeof(CBlock)+numProperties*sizeof(uint8_t)]);
    block->data_ptr = &chunk_bytes[sizeof(CBlock)
            +numProperties*sizeof(uint8_t)
            +numProperties*sizeof(uint16_t)];
    block->data_size = chunk_size
            -sizeof(CBlock)
            -numProperties*sizeof(uint8_t)
            -numProperties*sizeof(uint16_t);

    return block;
}

} //CGraphicsData
} //Coffee

#endif // CGRAPHICSDATA_H
