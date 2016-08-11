#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_GetProgramBinary
{
    /* Programs */
    STATICINLINE void ProgramBinary(CGhnd h,CGenum t,uint64 size,c_cptr d)
    {glProgramBinary(h,t,d,size);}
    STATICINLINE bool ProgramGetBinary(CGhnd h,CGenum* t,Vector<byte_t>* d)
    {
        if(!glGetProgramBinary || !glGetProgramiv)
            return false;
        int32 p_size = 0;
        glGetProgramiv(h,GL_PROGRAM_BINARY_LENGTH,&p_size);
        szptr pre_size = d->size();
        d->resize(d->size()+p_size);
        glGetProgramBinary(h,p_size,nullptr,t,&(*d)[pre_size]);
        return true;
    }
};

}
}
