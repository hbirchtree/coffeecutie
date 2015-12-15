#ifndef COFFEE_H
#define COFFEE_H

#include "types/types.h"
#include "coffee_macros.h"

#include "plat/platform_detect.h"
#include "plat/cmemory.h"

#include <thread>
#include <functional>

namespace Coffee{

enum CoffeeExitCode
{
    CoffeeExit_UnknownBad  = 100,

    CoffeeExit_Termination = 120,
    CoffeeExit_Interrupt   = 121,

    CoffeeExit_Kill        = 122,
};

constexpr cstring _coffee_build_date = __DATE__;
constexpr cstring _coffee_build_time = __TIME__;

/*!
 * \brief This function handles initial program startup
 */
extern int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argv, byte_t** argc);

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

#endif // COFFEE_H
