#ifndef COFFEE_GRAPHICS_APIS_OPENGL_BUFFER_TYPES_H
#define COFFEE_GRAPHICS_APIS_OPENGL_BUFFER_TYPES_H

#include "coffee/core/types/composite_types.h"
#include "copengl_types.h"
#include "crenderfence.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CBuffer : _cbasic_graphics_buffer_mappable
{
    CBuffer();
};

using CSubBuffer = _cbasic_graphics_buffer_section;

template<size_t Size>
class CNBuffer : public _cbasic_nbuffer<CBuffer,Size>
{
    using _cbasic_nbuffer<CBuffer,Size>::current_idx;
    using _cbasic_nbuffer<CBuffer,Size>::next_idx;
public:
    CNBuffer()
    {
        for(size_t i=0;i<Size;i++)
            fences[i] = nullptr;
    }

    virtual bool isCurrentLocked()
    {
        return coffee_graphics_fence_wait(fences[current_idx()],0);
    }
    virtual void lockCurrent()
    {
        fences[current_idx()] = coffee_graphics_fence_create();
    }
    virtual bool awaitCurrent(uint64 max_timeout = 10)
    {
        if(!fences[current_idx()])
            return true;
        bool status = coffee_graphics_fence_wait(fences[current_idx()],max_timeout);
        if(status)
        {
            coffee_graphics_free(fences[current_idx()]);
            fences[current_idx()] = nullptr;
        }
        return status;
    }
protected:
    CGSync* fences[Size];
};

static const CBufferAccess CBuffer_PersistentBufferFlags =
        CBufferAccess::Persistent|CBufferAccess::Coherent|
        CBufferAccess::WriteBit;

using SubBufferData = void*(*)(CSubBuffer&);
using BufferCopy = void(*)(CBuffer const&,CBuffer&,CGszptr const&,CGszptr const&,CGsize const&);
using BufferStore = void(*)(CBuffer&,const void*,CGsize const&,CBufferUsage const&);
using BufferStoreImmutable = void(*)(CBuffer&, const void*,CGsize const&, CBufferStorage const&);
using BufferSubStore = void(*)(CBuffer&,c_cptr,CGszptr const& offset, CGsize const& size);
using BufferMap = void*(*)(CBuffer&,CBufferAccess const&);
using BufferUnmap = bool(*)(CBuffer&);
using BufferDownload = void*(*)(CBuffer&,CGszptr const&,CGsize const&);
using BufferInvalidate = void(*)(CBuffer&);

struct CBufferPFN
{
    BufferStore             store;/*!< Function for storing data */
    BufferCopy              copy;/*!< Function for copying data */
    BufferStoreImmutable    store_immutable;/*!< Function for storing immutable data */
    BufferSubStore          substore;/*!< Function for storing sections of data */
    BufferMap               map;/*!< Function for mapping buffer */
    BufferUnmap             unmap;/*!< Function for unmapping buffer */
    BufferDownload          download;/*!< Function for downloading data */
    BufferInvalidate        invalidate;/*!< Function for invalidating data */
    SubBufferData           subdata;/*!< Function for acquiring pointer to subdata */
};

}
}

#endif
