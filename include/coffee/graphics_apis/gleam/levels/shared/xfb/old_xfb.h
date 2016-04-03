#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"

namespace Coffee{
namespace CGL{

struct CGL_Old_XFB
{
    STATICINLINE void XFBegin(CGenum p){glBeginTransformFeedback(p);}
    STATICINLINE void XFEnd(){glEndTransformFeedback();}

    STATICINLINE void XFVaryings(CGhnd h,int32 n,cstring* names,AttribMode a)
    {glTransformFeedbackVaryings(h,n,names,to_enum(a));}

    STATICINLINE void XFGetVaryings(CGhnd h,uint32* n,cstring_w** names,CGenum** type, int32** size)
    {
        int32 num = 0;
        ProgramGetiv(h,GL_TRANSFORM_FEEDBACK_VARYINGS,&num);

        type[0] = new CGenum[num];
        size[0] = new int32[num];
        names[0] = new cstring_w[num];
        int32 namelen = 0;
        ProgramGetiv(h,GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH,&namelen);
        for(int32 i=0;i<num;i++)
            names[0][i] = new int8[namelen];

        for(int32 i=0;i<num;i++)
            glGetTransformFeedbackVarying(h,i,namelen,nullptr,&size[0][i],&type[0][i],names[0][i]);
        *n = num;
    }
};

}
}
