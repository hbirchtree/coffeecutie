#ifndef COFFEE
#define COFFEE

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <stdint.h>

namespace Coffee{
//Core types, might become base type for all classes in place of QObject
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
typedef std::string CString; //In case we want to switch it

//Light-weight data types

//Size type
struct CSize{
    int32_t w;
    int32_t h;
};
struct CSizeF{
    float w;
    float h;
};
//Point type
struct CPoint{
    int32_t x;
    int32_t y;
};
struct CPointF{
    float x;
    float y;
};
//Rectangle type
struct CRect{
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
};
struct CRectF{
    float x;
    float y;
    float w;
    float h;
};

//These are not meant to replace glm
//Will be used as a dumbed-down version for C# and similar
//The memory layout is the same anyway
struct CVec2{
    float x;
    float y;
};
struct CVec3{
    float x;
    float y;
    float z;
};
struct CVec4{
    float x;
    float y;
    float z;
    float w;
};
struct CQuat{
    float w;
    float x;
    float y;
    float z;
};
}

//Wrappers for GL features
namespace CGraphicsWrappers{

}

//Physics-related types
namespace CPhysicsSystem{
class CPhyObject;

//Physics libraries here, should make an interface soon
class CBulletSystem;
}

//Misc. resources in engine, should not interface with GL directly
namespace CResourceTypes{
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

using namespace Coffee::CPrimitiveDataTypes;
using namespace Coffee::CFunctional;

#endif // COFFEE
