#ifndef COFFEE
#define COFFEE

#include <sys/resource.h>

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <exception>

//For unused variables
#define C_UNUSED(v) do{(void)(v);}while(0);
//Assertion for unit tests
#define CASSERT(exp) (!exp)?throw std::runtime_error("Assert failed"):fprintf(stderr,"Assert passed\n");

namespace Coffee{
//Core typedefs
typedef float              scalar;
typedef double             bigscalar;
typedef char               byte;
//Your typical C-string
typedef const char*        cstring;
typedef char*              cstring_w;
//Wide string
typedef const int16_t*     cwstring;
typedef int16_t*           cwstring_w;
//std::string can be switched for other string types, like QString
typedef std::string CString;
//

static void CoffeeInit();

//A simpler implementation of QObject
class CObject;

//Different data types we implement
namespace CPrimitiveDataTypes{
namespace CVectorTypes{
class VectorData;
class ScalarValue;

class Vector2Value;
class Vector3Value;
class Vector4Value;

class QuatValue;

class Matrix3Value;
class Matrix4Value;
}

//General data types

static cstring coffee_cpy_string(cstring str){
    cstring_w buf = reinterpret_cast<cstring_w>(malloc(strlen(str)+1));
    strcpy(buf,str);
    return buf;
}

//Light-weight data types

//Size type
struct CSize{
    CSize(int32_t w,int32_t h){
        this->w = w;
        this->h = h;
    }
    CSize(){}
    int32_t w;
    int32_t h;
};
struct CSizeF{
    CSizeF(scalar w,scalar h){
        this->w = w;
        this->h = h;
    }
    CSizeF(){}
    scalar w;
    scalar h;
};
//Point type
struct CPoint{
    CPoint(int32_t x,int32_t y){
        this->x = x;
        this->y = y;
    }
    CPoint(){}
    int32_t x;
    int32_t y;
};
struct CPointF{
    CPointF(scalar x,scalar y){
        this->x = x;
        this->y = y;
    }
    CPointF(){}
    scalar x;
    scalar y;
};
//Rectangle type
struct CRect{
    CRect(int32_t x,int32_t y,int32_t w,int32_t h){
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    CRect(){}
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
};
struct CRectF{
    CRectF(scalar x,scalar y,scalar w,scalar h){
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    CRectF(){}
    scalar x;
    scalar y;
    scalar w;
    scalar h;
};

//These are not meant to replace glm
//Will be used as a dumbed-down version for C# and similar
//The memory layout is the same anyway
struct CVec2{
    CVec2(scalar x,scalar y){
        this->x = x;
        this->y = y;
    }
    CVec2(){}
    scalar x;
    scalar y;
};
struct CVec3{
    CVec3(scalar x,scalar y,scalar z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    CVec3(){}
    scalar x;
    scalar y;
    scalar z;
};
struct CVec4{
    CVec4(scalar x,scalar y,scalar z,scalar w){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    CVec4(){}
    scalar x;
    scalar y;
    scalar z;
    scalar w;
};
struct CQuat{
    CQuat(scalar w,scalar x,scalar y,scalar z){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    CQuat(){}
    scalar w;
    scalar x;
    scalar y;
    scalar z;
};
}

//Wrappers for GL features
namespace CGraphicsWrappers{}

//Physics-related types
namespace CPhysicsSystem{
class CPhyObject;

//Physics libraries here, should make an interface soon
class CBulletSystem;
}

//Misc. resources in engine, should not interface with GL directly
namespace CResourceTypes{

namespace CAssimp{
class CAssimpImporters;
struct CAssimpData;
}

class CMesh;
class CTexture;
class CInstanceContainer;

class CCamera;
class CMaterial;
class CLight;
}

//Core classes
namespace CFunctional{
class CQueueRunner;

template<typename RType, typename... AType>
class CRFunction;

//These are queued up, run and deleted
typedef std::function<void()> QueueFunction;
}

//Input classes
namespace CInput{
//Rest of it is in "coffee/cinput.h"
typedef CFunctional::CRFunction<void,void*,uint32_t> CInputHandlerFunction;
}

//Windowing and rendering contexts
namespace CDisplay{
struct CDMonitor; //Represents a monitor and its properties
struct CDWindow; //Represents a window, most commonly the context window

//Parts of the renderer class, should allow switching context manager without hassle
class CDRendererBase;
class CGLFWRenderer; //One of several context managers, we might use SDL2 sometime.
class CGPUMonitor;

class CDRenderer; //This is a placeholder for the final implemented class

typedef CFunctional::CRFunction<void> RenderFunction;
}

//File management
namespace CResources{
class CFAssetStorage;
class CFObjectImport;
}

//Scripting
namespace CScripting{
class CScriptEngine;
class CSObject;
}

} //Coffee


void Coffee::CoffeeInit(){
    //Allow core dump by default
    struct rlimit core_limits;
    core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE,&core_limits);
}

using namespace Coffee::CPrimitiveDataTypes;
using namespace Coffee::CFunctional;

#endif // COFFEE
