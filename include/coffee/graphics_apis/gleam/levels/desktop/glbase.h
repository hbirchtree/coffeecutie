#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H

#include <coffee/core/CDebug>

#include "../gl_shared.h"

#include "../glxml.h"

namespace Coffee{
namespace CGL{

struct CGL_Implementation : CGL_Shared_Functions
{
    /* Base OpenGL, all implemented in GL3.3 */
    STATICINLINE void Enable(Feature e,uint32 o = 0){glEnable(to_enum(e,o));}
    STATICINLINE void Disable(Feature e,uint32 o = 0){glDisable(to_enum(e,o));}

    STATICINLINE void Enablei(Feature e,uint32 i,uint32 o = 0){glEnablei(to_enum(e,o),i);}
    STATICINLINE void Disablei(Feature e,uint32 i,uint32 o = 0){glDisablei(to_enum(e,o),i);}

    STATICINLINE void ColorMaski(uint32 i, CColorMask op){glColorMaski(i,op.r,op.g,op.b,op.a);}

    STATICINLINE void ColorLogicOp(LogicOp op){glLogicOp(to_enum(op));}
    STATICINLINE void DepthSet(ZField64 const* d){glDepthRange(d->near,d->far);}

    STATICINLINE void PointParameteriv(CGenum f,const int32* d){glPointParameteriv(f,d);}
    STATICINLINE void PointParameterfv(CGenum f,const scalar* d){glPointParameterfv(f,d);}
    STATICINLINE void PointSize(scalar f){glPointSize(f);}

    STATICINLINE void PolyMode(Face f1,DrawMode f2){glPolygonMode(to_enum(f1),to_enum(f2));}

    /* SAMPLE_ALPHA_TO_COVERAGE,SAMPLE_ALPHA_TO_ONE */
    STATICINLINE void SampleMaski(uint32 d,CGflag f){glSampleMaski(d,f);}

    struct Debug : CGL_Shared_Functions::Debug
    {
        STATICINLINE bool VerifyInit()
	{
            return (bool)glEnable;
        }

        /* Strictly debugging */

        STATICINLINE void InitDebugFlag()
        {
            b_isDebugging = false;
        }

        STATICINLINE void SetDebugMode(bool enabled)
        {
            if(enabled == b_isDebugging)
                return;
            if(enabled)
            {
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                b_isDebugging = true;
            }
            else
            {
                glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                b_isDebugging = true;
            }
        }

        STATICINLINE void SetDebugLevel(Severity s,bool enabled)
        {
            glDebugMessageControl(
                        GL_DONT_CARE,GL_DONT_CARE,
                        to_enum(s),0,nullptr,
                        (enabled)?GL_TRUE:GL_FALSE);
        }

        STATICINLINE void SetObjectLabel(Object t,CGhnd h,cstring s)
        {
            glObjectLabel(to_enum(t),h,-1,s);
        }

        STATICINLINE void SetDebugGroup(cstring n, uint32 id)
        {
            glPushDebugGroup(GL_DEBUG_TYPE_PUSH_GROUP,id,-1,n);
        }
        STATICINLINE void UnsetDebugGroup()
        {
            glPopDebugGroup();
        }

        STATICINLINE void DebugMessage(Severity s,DebugType t,cstring n)
        {
            glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION,
                                 to_enum(t),0,
                                 to_enum(s),
                                 -1,n);
        }

        STATICINLINE void DebugSetCallback(CGcallback c, void* param)
        {
            glDebugMessageCallback(c,param);
        }

        /* Get*v */

        STATICINLINE bigscalar GetScalarLL(CGenum e)
        {
            bigscalar i = 0.0;
            glGetDoublev(e,&i);
            return i;
        }

        /* Get*i_v */

        STATICINLINE scalar GetScalarI(CGenum e,uint32 i)
        {
            scalar v = 0.f;
            glGetFloati_v(e,i,&v);
            return v;
        }
        STATICINLINE bigscalar GetScalarLLI(CGenum e,uint32 i)
        {
            bigscalar v = 0.0;
            glGetDoublei_v(e,i,&v);
            return v;
        }

        STATICINLINE bool GetBooleanvI(CGenum e,uint32 i)
        {
            GLboolean v = GL_FALSE;
            glGetBooleani_v(e,i,&v);
            return v==GL_TRUE;
        }

        /* Is* */

        STATICINLINE bool IsEnabledi(Feature f,int32 i){
            return glIsEnabledi(to_enum(f),i)==GL_TRUE;}

        STATICINLINE bool IsPipeline(CGhnd h){return glIsProgramPipeline(h)==GL_TRUE;}

        /* Features */

        STATICINLINE void IsEnabled(Feature f){
            glIsEnabled(to_enum(f));}
    };

};

}
}

#endif

#include "../gl_to_enum.inl"
