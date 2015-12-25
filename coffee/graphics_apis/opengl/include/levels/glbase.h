#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_BASE_H

#include <coffee/core/types/basetypes.h>

namespace Coffee{
namespace CGL{

struct CGL_Layer
{
    using InterceptFunction = void(*)(cstring);

    static void initialize()
    {
    }
    static void interceptCallPost(InterceptFunction)
    {
    }
};

struct CGL_Context
{
    void makeCurrent()
    {
    }
};

struct CGL_Implementation
{
    typedef uint32 CGenum;
    typedef uint32 CGuint;
    typedef int32 CGint;
    typedef uint64 CGuint64;
    typedef int64 CGint64;

    typedef uint32 CGhnd;

    //Feature support

    struct Debug
    {
        //Variables
        static bool b_isDebugging;

        //Strictly debugging

        static void InitDebugFlag(){b_isDebugging = false;}
        static void SetDebugLevel(Severity){}

        static void SetObjectLabel(CGenum,CGuint,cstring){}

        static void SetDebugGroup(cstring){}
        static void UnsetDebugGroup(){}

        static void DebugMessage(cstring msg){}

        static void DebugSetCallback(){}

        static void IsEnabledi(){}

        static cstring GetString(CGenum){return nullptr;}
        static cstring GetStringi(CGenum,CGuint){return nullptr;}

        //Get*v

        static CGint GetInteger(CGenum){return 0;}
        static CGuint GetUInteger(CGenum){return 0;}

        static CGint64 GetIntegerLL(CGenum){return 0;}
        static CGuint64 GetUIntegerLL(CGenum){return 0;}

        static scalar GetScalar(CGenum){return 0;}
        static bigscalar GetScalarLL(CGenum){return 0;}

        static bool GetBooleanv(CGenum){return false;}

        //Get*i_v

        static CGint GetIntegerI(CGenum,CGuint){return 0;}
        static CGuint GetUIntegerI(CGenum,CGuint){return 0;}

        static CGint64 GetIntegerLLI(CGenum,CGuint){return 0;}
        static CGuint64 GetUIntegerLLI(CGenum,CGuint){return 0;}

        static scalar GetScalarI(CGenum,CGuint){return 0;}
        static bigscalar GetScalarLLI(CGenum,CGuint){return 0;}

        static bool GetBooleanv(CGenum,CGuint){return false;}

        //Is*

        static bool IsBuffer(CGhnd){return false;}
        static bool IsVAO(CGhnd){return false;}

        static bool IsShader(CGhnd){return false;}
        static bool IsProgram(CGhnd){return false;}

        static bool IsSync(CGhnd){return false;}
        static bool IsQuery(CGhnd&){return false;}

        static bool IsTexture(CGhnd){return false;}
        static bool IsSampler(CGhnd){return false;}

        static bool IsTexHandle(CGhnd){return false;}

        //Internal format information

        static bool InternalFormatSupport(CGenum){return false;}
    };
};

}
}

#endif
