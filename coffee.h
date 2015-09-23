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
#define CASSERT(exp) if(!exp)throw std::runtime_error("Assert failed");
//void* to char*
#define C_CHARDATA(ptr) reinterpret_cast<byte*>(ptr)

namespace Coffee{
//Core typedefs

//Integer types
typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

//Small letters = basic type, mostly C types
//Big letters = class/struct type, mostly C++ types
typedef float               scalar;
typedef double              bigscalar;
typedef int8                byte;
typedef uint8               ubyte;
//Your typical C-string
typedef const byte*         cstring;
typedef byte*               cstring_w;
//Wide string
typedef const int16*        cwstring;
typedef int16*              cwstring_w;

typedef intptr_t            ptr_s;
typedef uintptr_t           ptr_u;
typedef unsigned long long  szptr;

typedef size_t              csize_t;

//Can be switched for other types from libraries
typedef std::string         CString;
typedef std::runtime_error  CStdFault;
//

static void CoffeeInit();

//A simpler implementation of QObject
class CObject;

//Different data types we implement
namespace CPrimitiveDataTypes{

//General data types
static cstring coffee_cpy_string(cstring str){
    cstring_w buf = reinterpret_cast<cstring_w>(malloc(strlen(str)+1));
    strcpy(buf,str);
    return buf;
}

struct CDataChunk{
    void* ptr   = nullptr;
    ptr_u size  = 0;
};

//Size type
template<typename T>
struct _cbasic_size
{
    _cbasic_size(T w,T h){
        this->w = w;
        this->h = h;
    }
    _cbasic_size(){}
    T w = 0;
    T h = 0;
};

typedef _cbasic_size<int32> CSize;
typedef _cbasic_size<scalar> CSizeF;
typedef _cbasic_size<bigscalar> CSizeD;

//Point type
template<typename T>
struct _cbasic_point
{
    _cbasic_point(T x,T y){
        this->x = x;
        this->y = y;
    }
    _cbasic_point(){}
    T x = 0;
    T y = 0;
};

typedef _cbasic_point<int32> CPoint;
typedef _cbasic_point<scalar> CPointF;
typedef _cbasic_point<bigscalar> CPointD;
//Rectangle type
template<typename T>
struct _cbasic_rect
{
    _cbasic_rect(T x,T y,T w,T h){
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    _cbasic_rect(){}
    T x = 0;
    T y = 0;
    T w = 0;
    T h = 0;
};

typedef _cbasic_rect<int32> CRect;
typedef _cbasic_rect<scalar> CRectF;
typedef _cbasic_rect<bigscalar> CRectD;

//These are not meant to replace glm
//Will be used as a dumbed-down version for C# and similar
//The memory layout is the same anyway
struct CVec2{
    CVec2(scalar x,scalar y){
        this->x = x;
        this->y = y;
    }
    CVec2(){}
    union{
        scalar x;
        scalar s;
        scalar u;
    };
    union{
        scalar y;
        scalar t;
        scalar v;
    };
};
struct CVec3{
    CVec3(scalar x,scalar y,scalar z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    CVec3(){}
    union{
        scalar x;
        scalar r;
        scalar s;
        scalar u;
    };
    union{
        scalar y;
        scalar g;
        scalar t;
        scalar v;
    };
    union{
        scalar z;
        scalar b;
        scalar p;
        scalar w;
    };
};
struct CVec4{
    CVec4(scalar x,scalar y,scalar z,scalar w){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    CVec4(){}
    union{
        scalar x = 0.f;
        scalar r;
    };
    union{
        scalar y = 0.f;
        scalar g;
    };
    union{
        scalar z = 0.f;
        scalar b;
    };
    union{
        scalar w = 0.f;
        scalar a;
    };
};
struct CQuat{
    CQuat(scalar w,scalar x,scalar y,scalar z){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    CQuat(){}
    scalar x = 0.f;
    scalar y = 0.f;
    scalar z = 0.f;
    scalar w = 0.f;
};

struct CMat3{
    scalar m[3][3];
};
struct CMat4{
    scalar m[4][4];
};

}

//Wrappers for GL features
namespace CGraphicsWrappers{}

//Physics-related types
namespace CPhysicsSystem{}

//Misc. resources in engine, should not interface with GL directly
namespace CResourceTypes{
namespace CAssimp{}
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
typedef CFunctional::CRFunction<void> RenderFunction;
}

//File management
namespace CResources{}

//Scripting
namespace CScripting{}

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
