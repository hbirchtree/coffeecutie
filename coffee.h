#ifndef COFFEE
#define COFFEE

#include "coffee_types.h"
#include "coffee_macros.h"

//Platform core functions
#include "plat/plat_core.h"
#include "plat/platform_detect.h"

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <string.h>
#include <exception>
#include <thread>

namespace Coffee{

//Can be switched for other types from libraries
typedef std::string         CString;
typedef std::runtime_error  CStdFault;
//

static void CoffeeInit()
{
    //Allow core dump by default
    coffee_enable_core_dump();
}

//A simpler implementation of QObject
class CObject;

namespace CDisplay{
//Required for some things!
struct CDWindow;
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

namespace CThreading{
typedef std::thread::id thread_id;
}

template<typename RType, typename... AType>
class CRFunction;

//These are queued up, run and deleted
typedef std::function<void()> QueueFunction;
}

//Input classes
namespace CInput{
//Rest of it is in "coffee/cinput.h"
typedef CFunctional::CRFunction<void,void*,uint32> CInputHandlerFunction;
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

using namespace Coffee::CFunctional;

#endif // COFFEE
