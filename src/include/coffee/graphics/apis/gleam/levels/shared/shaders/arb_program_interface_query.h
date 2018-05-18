#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_ProgramInterfaceQuery
{
    //TODO: Create PInterfaceProperty enum
    STATICINLINE void ProgramGetInterfaceiv(CGhnd h,CGenum i,CGenum e,int32* v)
    {glGetProgramInterfaceiv(h,i,e,v);}

    STATICINLINE void ProgramGetResourceiv(CGhnd h)
    {
        (void)h;
        //TODO: Implement this shit
    }

    STATICINLINE uint32 ProgramGetResourceIdx(CGhnd h,CGenum i,cstring n)
    {return glGetProgramResourceIndex(h,i,n);}
    STATICINLINE cstring_w ProgramGetResourceName(CGhnd h,CGenum i,uint32 d)
    {
        int32 len = 0;
        ProgramGetInterfaceiv(h,i,GL_MAX_NAME_LENGTH,&len);
        cstring_w name = new char[len+1];
        name[len] = 0;
        glGetProgramResourceName(h,i,d,len+1,nullptr,name);
        return name;
    }
    STATICINLINE int32 ProgramGetResourceLoc(CGhnd h,CGenum e,cstring n)
    {return glGetProgramResourceLocation(h,e,n);}
#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
    STATICINLINE int32 ProgramGetResourceLocIdx(CGhnd h,CGenum e,cstring n)
    {return glGetProgramResourceLocationIndex(h,e,n);}
#endif
};

}
}
