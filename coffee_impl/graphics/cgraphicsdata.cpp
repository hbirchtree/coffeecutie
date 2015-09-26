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
    CASSERT((size==propertySizes[index]))

    szptr offset = 0;
    for(int i=0;i<index;i++)
        offset+=propertySizes[i];

    byte* bytes = reinterpret_cast<byte*>(data_ptr);
    memcpy(&bytes[offset],data,size);
}

}
}
