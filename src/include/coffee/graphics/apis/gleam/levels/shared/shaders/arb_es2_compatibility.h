#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_ES2Compatibility
{
    STATICINLINE void ShaderReleaseCompiler()
    {glReleaseShaderCompiler();}
    STATICINLINE void ShaderGetPrecisionFormat(ShaderStage s,CGenum e,int32* r,int32* p)
    {glGetShaderPrecisionFormat(to_enum1(s),e,r,p);}

    STATICINLINE void ShaderBinary(uint32 n,const CGhnd* h,CGenum t,c_cptr d,int32 s)
    {glShaderBinary(n,h,t,d,s);}
};

}
}
