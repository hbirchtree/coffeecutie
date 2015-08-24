#ifndef COFFEE
#define COFFEE

#include <QDebug>

namespace Coffee{
class CObject;

//Core classes
namespace CFunctional{
class CRQueueRunner;

template<typename RType, typename... AType>
class CRFunction;
template<typename RType,typename... AType>
class CRStaticFunction;
template<class TClass, class FClass, typename RType, typename... AType>
class CRMemberFunction;

typedef std::function<void()> QueueFunction; //This needs to be light

}

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

namespace CDisplay{
class CDRendererBase;
class CDRenderer;
}

namespace CFiles{
class CFResource;
class CFAssetStorage;
class CFObjectImport;
}

namespace CScripting{
class CSScriptEngine;
class CSObject;
}

} //Coffee


#endif // COFFEE
