#ifndef COFFEE
#define COFFEE

#include <QDebug>

namespace Coffee{
//Core types, might become base type for all classes in place of QObject
class CObject;

//Wrappers of data types
namespace CPrimitiveDataTypes{
class ScalarValue;

class Vector2Value;
class Vector3Value;
class Vector4Value;

class QuatValue;

class Matrix3Value;
class Matrix4Value;
}

//Wrappers for GL features
namespace CGraphicsWrappers{
class CBuffer;
class CVertexArrayObject;

class CUniformBlock;
class CUniformValue; //Bound to a uniform or uniform block field
class CShader;

class CFramebuffer;
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

//Separating animation-related types
namespace CAnimationDataTypes{
class CSkeleton;
class CAnimationState;
}
}

//Core classes
namespace CFunctional{
class CRQueueRunner;

//Templates for function wrappers, all under a common CRFunction type
template<typename RType, typename... AType>
class CRFunction;
template<typename RType,typename... AType>
class CRStaticFunction;
template<class TClass, class FClass, typename RType, typename... AType>
class CRMemberFunction;

typedef std::function<void()> QueueFunction; //This needs to be light

}

//Input classes
namespace CInput{
typedef CFunctional::CRFunction<void,void*,uint32_t> CInputHandlerFunction;

struct CIEvent;
struct CIKeyEvent;
struct CIMouseEvent;
struct CIScrollEvent;
struct CIDropEvent;

struct CIJoyInfo;
struct CIJoyState;

class CIEventParser;
}

//Windowing and rendering contexts
namespace CDisplay{
class CDRendererBase;
class CDRenderer;

class CGLFWRenderer;
class CGPUMonitor;
}

//File management
namespace CFiles{
class CResource;
class CFAssetStorage;
class CFObjectImport;
}

//Scripting
namespace CScripting{
class CScriptEngine;
class CSObject;
}

} //Coffee


#endif // COFFEE
