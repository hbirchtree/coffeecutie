#ifndef CUNIFORMCHUNK
#define CUNIFORMCHUNK

#include "cgraphicswrappers.h"
#include "cgraphicsdata.h"
#include "cshader.h"

namespace Coffee{
namespace CGraphicsWrappers{

//Envelope for CBlock+CUniformBlock, keeping them in-sync
struct CUniformChunk{
    CSubBuffer*             buffer  = nullptr; //Should be partition of bigger buffer
    CUniformBlock           ublock;
    CGraphicsData::CBlock*  block   = nullptr;
};

static CUniformChunk* coffee_create_uchunk(CSubBuffer* part,szptr size,szptr numProperties,
                                    szptr* sizes,cstring chunkName)
{
    CUniformChunk* chunk = reinterpret_cast<CUniformChunk*>(calloc(1,sizeof(CUniformChunk)));

    chunk->buffer = part;
    CASSERT((part->size==size));

    chunk->block = CGraphicsData::coffee_create_block(size,numProperties,sizes);

    chunk->ublock.name = chunkName;
    chunk->ublock.size = size;
}


}
}

#endif // CUNIFORMCHUNK

