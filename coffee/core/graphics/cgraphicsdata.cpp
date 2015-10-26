#include "cgraphicsdata.h"

namespace Coffee{
namespace CGraphicsData{

uint16 CBlock::dataSize() const
{
    return data_size;
}

void *CBlock::dataPtr() const
{
    return data_ptr;
}

void CBlock::setPropertyData(uint16 index, const void *data, uint16 size)
{
    CASSERT((index<numProperties));
    CASSERT((size==propertySizes[index]));

    szptr offset = 0;
    for(int i=0;i<index;i++)
        offset+=propertySizes[i];

    byte* bytes = reinterpret_cast<byte*>(data_ptr);
    memcpy(&bytes[offset],data,size);
}

CBlock *coffee_create_block(uint16 dataSize, uint16 numProperties, szptr *sizes)
{
    szptr chunk_size = dataSize
            +sizeof(CBlock)
            +numProperties*sizeof(uint8)
            +numProperties*sizeof(uint16);
    void* chunk = calloc(sizeof(byte),chunk_size); //We want all zeros instead of undefined
    byte* chunk_bytes = reinterpret_cast<byte*>(chunk);

    CBlock* block = reinterpret_cast<CBlock*>(chunk);
    block->numProperties = numProperties;
    block->blockSize = chunk_size;
    block->propertyTypes = reinterpret_cast<uint8*>(&chunk_bytes[sizeof(CBlock)]);
    block->propertySizes = reinterpret_cast<uint16*>(&chunk_bytes[sizeof(CBlock)+numProperties*sizeof(uint8)]);
    block->data_ptr = &chunk_bytes[sizeof(CBlock)
            +numProperties*sizeof(uint8)
            +numProperties*sizeof(uint16)];
    block->data_size = chunk_size
            -sizeof(CBlock)
            -numProperties*sizeof(uint8)
            -numProperties*sizeof(uint16);

    if(sizes){
        for(int i=0;i<numProperties;i++)
            block->propertySizes[i] = sizes[i];
    }

    return block;
}

void coffee_graphics_gen_matrix(CModelTransform *mat)
{
    mat->matrix = CMath::scale(CMath::mat4(),mat->scale);
    mat->matrix = CMath::translate(mat->matrix,mat->position) * CMath::mat4_cast(mat->rotation);
}

void _coffee_graphics_rotate_translate(
        CMath::mat4* matrix, const CMath::quat& rotation, const CMath::vec3& position)
{
    *matrix *= CMath::mat4_cast(rotation);
    *matrix = CMath::translate(*matrix,position);
}

void coffee_graphics_gen_matrix_perspective(CGCamera *cam)
{
    cam->matrix = CMath::perspective(
                CMath::radians(cam->fieldOfView), cam->aspect,
                cam->zVals.near, cam->zVals.far);
    _coffee_graphics_rotate_translate(&cam->matrix,cam->rotation,cam->position);
}

void coffee_graphics_gen_matrix_orthographic(CGCamera *cam)
{
    cam->matrix = CMath::ortho(cam->orthoview.x,cam->orthoview.w,
                          cam->orthoview.y,cam->orthoview.h);
    _coffee_graphics_rotate_translate(&cam->matrix,cam->rotation,cam->position);
}

}
}
