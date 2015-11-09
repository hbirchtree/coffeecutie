#ifndef COFFEE
#define COFFEE

#include "coffee_types.h"
#include "coffee_macros.h"

#include "plat/platform_detect.h"

#include <thread>
#include <functional>

namespace Coffee{

/*!
 * \brief This function handles initial program startup
 */
extern int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argv, byte** argc);

/*!
 * \brief Initializes process state before running. Mainly core dumping.
 */
extern void CoffeeInit();

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
namespace CResourceTypes{}

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
