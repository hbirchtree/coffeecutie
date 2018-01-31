#pragma once

#include "inputapplication.h"
#include <coffee/core/eventprocess.h>

#include <coffee/core/plat/plat_timing.h>
#include <coffee/core/task_queue/task.h>

#include <coffee/core/base/renderer/eventapplication_wrapper.h>
#include <coffee/core/base/renderer_loader.h>

#include <coffee/core/CMD>

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace Coffee{
namespace Display{

template<typename Renderer, typename ShareData>
struct EventLoopData
{
    enum Flags
    {
        TimeLimited = 0x1,
    };

    UqPtr<Renderer> renderer;
    UqPtr<ShareData> data;

    Function<void(Renderer&, ShareData*)> setup;
    Function<void(Renderer&, ShareData*)> loop;
    Function<void(Renderer&, ShareData*)> cleanup;

    uint32 flags;

    union
    {
        struct{
            Timestamp start;
            Timestamp max;
        } time;
    };

    CDProperties visual;

    FORCEDINLINE Renderer& r() {return *renderer.get();}
    FORCEDINLINE ShareData* d() {return data.get();}
};

template<typename R, typename D>
FORCEDINLINE
EventLoopData<R,D>* MkEventLoop(UqPtr<R>&& renderer,
                                UqPtr<D>&& data)
{
    return new EventLoopData<R,D>{
        std::move(renderer), std::move(data),
        {},  {}, {},

        0, {}, {}
    };
}

namespace CfEventFunctions {
template<typename RendType, typename DataType>
void WrapEventFunction(void* data, int event)
{
    using ELD = EventLoopData<RendType,DataType>;
    
    static int CurrentState;
    
    ELD* edata = C_FCAST<ELD*>(data);

    switch(event)
    {
    case CoffeeHandle_IsForeground:
    {
        break;
    }
    case CoffeeHandle_TransForeground:
    {
        break;
    }
    case CoffeeHandle_IsBackground:
    {
        break;
    }
    case CoffeeHandle_TransBackground:
    {
        break;
    }
        
    case CoffeeHandle_Setup:
        if(CurrentState == 0)
        {
            Profiler::DeepPushContext("Renderer-side setup");
            /* By default, try to load the highest GL version */
            if(LoadHighestVersion(&edata->r(), edata->visual, nullptr))
            {
                Profiler::DeepPopContext();
                Profiler::DeepPushContext("Application-side setup");
                edata->setup(edata->r(), edata->d());
                Profiler::DeepPopContext();
                CurrentState = 1;
            }else
                Profiler::DeepPopContext();
        }
        break;
        
    case CoffeeHandle_Loop:
        /* We retrieve the current thread's RuntimeQueue if it exists,
         *  and process it regularly. */
        if(CurrentState == 1)
            edata->loop(edata->r(), edata->d());
        if(RuntimeQueue::GetCurrentQueue())
            RuntimeQueue::GetCurrentQueue()->executeTasks();
        break;
        
    case CoffeeHandle_Cleanup:
        if(CurrentState == 1)
        {
            CurrentState = 0;
            Profiler::DeepPushContext("Application-side cleanup");
            edata->cleanup(edata->r(), edata->d());
            Profiler::DeepPopContext();
            
            Profiler::DeepPushContext("Renderer-side cleanup");
            edata->r().cleanup();
            Profiler::DeepPopContext();
            
#if defined(COFFEE_NO_ATEXIT)
            auto const& ex = Cmd::GetAtExit();
            for(auto it=ex.rbegin(); it != ex.rend(); it++)
                (*it)();
#endif
        }
        break;
        
    default:
        break;
    }
}

template<typename RendType, typename DataType>
void WrapEventFunctionNA(void* data, int event, void* p1, void* p2, void* p3)
{
    using ELD = EventLoopData<RendType,DataType>;
    
    static const constexpr CfAdaptors::CfAdaptor EventHandlingVector[10] = {
        {},
        {CfResizeEvent, CfAdaptors::CfResizeHandler},
        {CfTouchEvent, CfAdaptors::CfTouchHandler},
    };
    
    ELD* edata = C_FCAST<ELD*>(data);
    
    switch(event)
    {
        case CoffeeHandle_GeneralEvent:
        {
            struct CfGeneralEvent* g = C_FCAST<CfGeneralEvent*>(p1);
            
            auto func = EventHandlingVector[g->type].func;
            
            if(!func)
            {
                fprintf(stderr, "Hit NULL event handler!\n");
                return;
            }
            
            EventHandlingVector[g->type].func(&edata->r(), event,
                                              p1, p2, p3);
            
            break;
        }
        default:
        break;
    }
}
}

class EventApplication : public InputApplication
{
protected:
    bool m_closeFlag = false;
public:

    virtual ~EventApplication()
    {
    }

    template<typename EvType>
    struct EventHandler
    {
        using FPtr = void(*)(void*,EvType const& e, c_cptr);

        FPtr func;
        cstring name;
        void* user_ptr;
    };

    using EventHandlerI = EventHandler<CIEvent>;
    using EventHandlerD = EventHandler<CDEvent>;

    /*!
     * \brief Function driving the render loop
     */
    virtual void run() = 0;

    /*!
     * \brief Function used for polling events
     */
    virtual void pollEvents()
    {
    }

    /*!
     * \brief Allow installation of event handlers without
     *  implementing the class
     */
    virtual bool installEventHandler(EventHandlerI)
    {
        return false;
    }
    /*!
     * \brief Allow installation of event handlers without
     *  implementing the class
     */
    virtual bool installEventHandler(EventHandlerD)
    {
        return false;
    }

    virtual void injectEvent(CIEvent const&, c_cptr) = 0;

    virtual void injectEvent(CDEvent const&, c_cptr) = 0;

    virtual Vector<EventHandlerI>* getEventHandlersI()
    {
        return nullptr;
    }
    virtual Vector<EventHandlerD>* getEventHandlersD()
    {
        return nullptr;
    }

    /*!
     * \brief Get current context time, can be returned by the
     *  context or the system
     * \return Current context time
     */
    virtual bigscalar contextTime() const
    {
        static u64 start_time = 0;
        if(start_time == 0)
            start_time = Time::CurrentTimestamp<std::chrono::microseconds>();

        return bigscalar(Time::CurrentTimestamp<std::chrono::microseconds>() - start_time) * 1_us;
    }

    /*!
     * \brief A flag used to determine when a render loop should terminate
     * \return Whether or not a render loop should terminate
     */
    virtual bool closeFlag() const
    {
        return m_closeFlag;
    }

    virtual bool applyCloseFlag()
    {
        return m_closeFlag = true;
    }

    /*!
     * \brief Stores a reference to an event loop to be used with the application.
     * \param eventloop Generic pointer to EventLoopData<> structure
     */
    virtual void registerEventLoop(void* eventloop)
    {
        C_UNUSED(eventloop);
    }

    template<typename Renderer, typename Data>
    STATICINLINE void resumeExtra(EventLoopData<Renderer, Data>&)
    {
#if defined(COFFEE_EMSCRIPTEN)
        emscripten_resume_main_loop();
#endif
    }
    template<typename Renderer, typename Data>
    STATICINLINE void suspendExtra(EventLoopData<Renderer, Data>&)
    {
#if defined(COFFEE_EMSCRIPTEN)
        emscripten_pause_main_loop();
#endif
    }

#define SUSPRESUME_FUN(var, cond, fun, extrafun) \
    template<typename Renderer, typename Data> \
    static void var(void* ptr, CDEvent const& e, c_cptr) \
    { \
        if(e.type == CDEvent::cond) \
        { \
            EventLoopData<Renderer, Data>& evdata = *C_FCAST<EventLoopData<Renderer, Data>* >(ptr); \
            extrafun(evdata); \
            evdata.fun(evdata.r(), evdata.d()); \
        } \
    }

    SUSPRESUME_FUN(resumeFunction, IsForeground, setup, resumeExtra)
    SUSPRESUME_FUN(suspendFunction, IsBackground, cleanup, suspendExtra)

#undef SUSPRESUME_FUN

    template<typename Renderer, typename Data>
    struct EventExitHandler
    {
        using ELD = EventLoopData<Renderer,Data>;

        static ELD* ev;

        static void event_exitFunc()
        {
            event_exitFunc(ev);
        }
        static void event_exitFunc(ELD* ev)
        {
            if(!ev)
            {
                fprintf(stderr, "Event exit handler could not be triggered");
                return;
            }
            ev->cleanup(ev->r(), ev->d());
            ev->r().cleanup();
            delete ev;
        }
    };

    template<typename Renderer, typename Data>
    static int32 execEventLoop(EventLoopData<Renderer,Data>& ev,
                               CDProperties& visual, CString& err)
    {
        Profiler::DeepPushContext("Event loop creation");

        using ELD = EventLoopData<Renderer, Data>;

        static cstring suspend_str = "Suspend handler";
        static cstring resume_str = "Resume handler";

        Renderer& r = ev.r();

        /* Because MSVC++ sucks, I can't use a struct initializer list for this :( */
		EventHandlerD suspend_data = {};
		EventHandlerD resume_data = {};
		suspend_data.user_ptr = resume_data.user_ptr = &ev;
		suspend_data.func = suspendFunction<Renderer, Data>;
		resume_data.func = resumeFunction<Renderer, Data>;
		suspend_data.name = suspend_str;
		resume_data.name = resume_str;

        r.installEventHandler(suspend_data);
        r.installEventHandler(resume_data);

        /* This data is used in order to reload the event loop while running.
         * This is useful if you want to reload the entire user-part of
         *  the program.
         * In order for this to work, the setup and cleanup steps must
         *  be written properly without memory leaks.
         * This behavior is vital for the iOS and Android ports.
         */

        using namespace CfEventFunctions;

        ev.visual = visual;
        coffee_event_handling_data = &ev;
        CoffeeEventHandle = WrapEventFunction<Renderer, Data>;
        CoffeeEventHandleNA = WrapEventFunctionNA<Renderer, Data>;

#if defined(COFFEE_EMSCRIPTEN)
        /* Under emscripten, only the loop function is used later */
        emscripten_set_main_loop_arg([](void* data){
            CoffeeEventHandle(data, CoffeeHandle_Setup);
            CoffeeEventHandle(data, CoffeeHandle_Loop);
        }, &ev, 0, 1);
#endif


#if defined(COFFEE_USE_APPLE_GLKIT) || \
    defined(COFFEE_USE_ANDROID_NATIVEWIN) || \
    defined(COFFEE_EMSCRIPTEN)
        /* Under GLKit, the entry point for setup, update and cleanup
         *  reside in AppDelegate.m
         * Lifecycle is manageed by UIKit in this case
         */
        /* The same behavior is used with Android, where an external
         *  event loop handles events.
         */
        /* This is where Emscripten jumps off to Javascript land,
         *  with setTimeout and stuff */

        Profiler::DeepPopContext();

        return 0;
#else


        /* Pump a setup event to get everything started */
        CoffeeEventHandleCall(CoffeeHandle_Setup);

        /* For timed runs, set the starting time */
        ev.time.start = Time::CurrentTimestamp();

        /* In this case, event processing happens in a tight
         *  loop that happens regardless of outside events.
         * On platforms with their own event loops, this does not work.
         */

        while(!ev.renderer->closeFlag())
        {
            CoffeeEventHandleCall(CoffeeHandle_Loop);

            if(ev.flags & ELD::TimeLimited &&
                    Time::CurrentTimestamp() > (ev.time.start
                                                + ev.time.max))
            {
                auto qevent = CIEvent::Create(0, CIEvent::QuitSign);
                r.injectEvent(qevent, nullptr);
            }
        }

        CoffeeEventHandleCall(CoffeeHandle_Cleanup);

        return 0;
#endif
    }

    void exec()
    {

    }
};

template<typename GAPI, typename R, typename D>
int32 AutoExec(EventLoopData<R,D>& event)
{
    CString error;
    auto visual = Display::GetDefaultVisual<GAPI>();

    return EventApplication::execEventLoop(event, visual, error);
}

}
}
