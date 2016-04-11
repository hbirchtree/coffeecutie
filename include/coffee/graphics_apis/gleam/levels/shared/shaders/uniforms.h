#pragma once

#include "../gl_shared_include.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/vector_types.h>

namespace Coffee{
namespace CGL{

struct CGL_Old_Uniforms
{
    /* Using Uniform* */
    STATICINLINE void Uniformfv(int32 l,int32 c,const scalar* d)
    {glUniform1fv(l,c,d);}
    STATICINLINE void Uniformiv(int32 l,int32 c,const int32* d)
    {glUniform1iv(l,c,d);}
    STATICINLINE void Uniformuiv(int32 l,int32 c,const uint32* d)
    {glUniform2uiv(l,c,(uint32*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,const _cbasic_vec2<scalar>* d)
    {glUniform2fv(l,c,(scalar*)d);}
    STATICINLINE void Uniformiv(int32 l,int32 c,const _cbasic_vec2<int32>* d)
    {glUniform2iv(l,c,(int32*)d);}
    STATICINLINE void Uniformuiv(int32 l,int32 c,const _cbasic_vec2<uint32>* d)
    {glUniform2uiv(l,c,(uint32*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,const _cbasic_vec3<scalar>* d)
    {glUniform3fv(l,c,(scalar*)d);}
    STATICINLINE void Uniformiv(int32 l,int32 c,const _cbasic_vec3<int32>* d)
    {glUniform3iv(l,c,(int32*)d);}
    STATICINLINE void Uniformuiv(int32 l,int32 c,const _cbasic_vec3<uint32>* d)
    {glUniform4uiv(l,c,(uint32*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,const _cbasic_vec4<scalar>* d)
    {glUniform4fv(l,c,(scalar*)d);}
    STATICINLINE void Uniformiv(int32 l,int32 c,const _cbasic_vec4<int32>* d)
    {glUniform4iv(l,c,(int32*)d);}
    STATICINLINE void Uniformuiv(int32 l,int32 c,const _cbasic_vec4<uint32>* d)
    {glUniform4uiv(l,c,(uint32*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,2>* d)
    {glUniformMatrix2fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,2,3>* d)
    {glUniformMatrix2x3fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,2,4>* d)
    {glUniformMatrix2x4fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,3>* d)
    {glUniformMatrix3fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,3,2>* d)
    {glUniformMatrix3x2fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,3,4>* d)
    {glUniformMatrix3x4fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,4>* d)
    {glUniformMatrix4fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,4,2>* d)
    {glUniformMatrix4x2fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,4,3>* d)
    {glUniformMatrix4x3fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}
};

}
}
