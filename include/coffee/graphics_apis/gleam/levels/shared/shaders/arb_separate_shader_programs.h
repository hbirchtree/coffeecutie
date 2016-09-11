#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_SeparableShaderPrograms
{
    //TODO: Create off-thread program compiler when binary storage is available
    STATICINLINE CGhnd ProgramCreate(ShaderStage t,uint32 c,cstring* d)
    {
        CGhnd h = glCreateShaderProgramv(to_enum1(t),c,d);
        ProgramParameter(h,GL_PROGRAM_SEPARABLE,GL_TRUE);
        return h;
    }

    //TODO: Create ProgramParameter enum
    STATICINLINE void ProgramParameter(CGhnd h,CGenum e,int32 v)
    {glProgramParameteri(h,e,v);}

    /* Pipelines */
    STATICINLINE void PipelineBind(CGhnd h){glBindProgramPipeline(h);}
    STATICINLINE bool PipelineUseStages(CGhnd p,ShaderStage e,CGhnd prg)
    {
        glUseProgramStages(p,to_enum2(e),prg);
        //TODO: Add strict check in debug mode
        return true;
    }
    //TODO: Create PipelineProperty enum
    STATICINLINE void PipelineGetiv(CGhnd h,CGenum e,int32* v){glGetProgramPipelineiv(h,e,v);}
    STATICINLINE cstring_w PipelineGetLog(CGhnd h)
    {
        int32 len = 0;
        PipelineGetiv(h,GL_INFO_LOG_LENGTH,&len);
        if(len<=0)
            return nullptr;
        cstring_w s = new int8[len+1];
        glGetProgramPipelineInfoLog(h,len,nullptr,s);
        return s;
    }

    STATICINLINE ShaderStage PipelineGetStages(CGhnd p)
    {
        static const CGenum shader_types[] = {
            GL_VERTEX_SHADER,
            GL_FRAGMENT_SHADER,
            GL_TESS_CONTROL_SHADER,
            GL_TESS_EVALUATION_SHADER,
            GL_GEOMETRY_SHADER,
            GL_COMPUTE_SHADER
        };
        static const ShaderStage shader_values[] = {
            ShaderStage::Vertex,
            ShaderStage::Fragment,
            ShaderStage::TessControl,
            ShaderStage::TessEval,
            ShaderStage::Geometry,
            ShaderStage::Compute
        };

        ShaderStage out = {};

        int32 v;
        for(uint8 i=0;i<6;i++)
        {
            v = 0;
            PipelineGetiv(p,shader_types[i],&v);
            if(v != 0)
                out |= shader_values[i];
        }

        return out;
    }

    STATICINLINE bool PipelineValidate(CGhnd h)
    {
        glValidateProgramPipeline(h);
        int32 outStat;
        PipelineGetiv(h,GL_VALIDATE_STATUS,&outStat);
        return outStat == GL_TRUE;
    }

    /* ProgramUniform* functions */
    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,const scalar* d)
    {glProgramUniform1fv(h,l,c,d);}
    STATICINLINE void Uniformiv(CGhnd h,int32 l,int32 c,const int32* d)
    {glProgramUniform1iv(h,l,c,d);}
    STATICINLINE void Uniformuiv(CGhnd h,int32 l,int32 c,const uint32* d)
    {glProgramUniform1uiv(h,l,c,d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,const _cbasic_vec2<scalar>* d)
    {glProgramUniform2fv(h,l,c,(scalar*)d);}
    STATICINLINE void Uniformiv(CGhnd h,int32 l,int32 c,const _cbasic_vec2<int32>* d)
    {glProgramUniform2iv(h,l,c,(int32*)d);}
    STATICINLINE void Uniformuiv(CGhnd h,int32 l,int32 c,const _cbasic_vec2<uint32>* d)
    {glProgramUniform2uiv(h,l,c,(uint32*)d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,const _cbasic_vec3<scalar>* d)
    {glProgramUniform3fv(h,l,c,(scalar*)d);}
    STATICINLINE void Uniformiv(CGhnd h,int32 l,int32 c,const _cbasic_vec3<int32>* d)
    {glProgramUniform3iv(h,l,c,(int32*)d);}
    STATICINLINE void Uniformuiv(CGhnd h,int32 l,int32 c,const _cbasic_vec3<uint32>* d)
    {glProgramUniform3uiv(h,l,c,(uint32*)d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,const _cbasic_vec4<scalar>* d)
    {glProgramUniform4fv(h,l,c,(scalar*)d);}
    STATICINLINE void Uniformiv(CGhnd h,int32 l,int32 c,const _cbasic_vec4<int32>* d)
    {glProgramUniform4iv(h,l,c,(int32*)d);}
    STATICINLINE void Uniformuiv(CGhnd h,int32 l,int32 c,const _cbasic_vec4<uint32>* d)
    {glProgramUniform4uiv(h,l,c,(uint32*)d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,2>* d)
    {glProgramUniformMatrix2fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,2,3>* d)
    {glProgramUniformMatrix2x3fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,3>* d)
    {glProgramUniformMatrix3fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,3,2>* d)
    {glProgramUniformMatrix3x2fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,3,4>* d)
    {glProgramUniformMatrix3x4fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,4>* d)
    {glProgramUniformMatrix4fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,4,3>* d)
    {glProgramUniformMatrix4x3fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,4,2>* d)
    {glProgramUniformMatrix4x2fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}
};

}
}
