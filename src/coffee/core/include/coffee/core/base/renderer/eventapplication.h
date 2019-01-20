#pragma once

#include "inputapplication.h"
#include <coffee/core/base/renderer_loader.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/foreign/foreign.h>
#include <peripherals/libc/signals.h>
#include <peripherals/stl/time_types.h>

#include <coffee/core/CProfiling>

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace Coffee {
namespace Display {

template<typename R, typename D>
using LoopFunction = Function<void(R&, D*)>;

template<
    typename Renderer,
    typename ShareData

#if !defined(COFFEE_WINDOWS)
    ,
    typename implements<InitApplication, Renderer>::type*  = nullptr,
    typename implements<EventApplication, Renderer>::type* = nullptr
#endif

    >
struct EventLoopData
{
    struct DummyDeprecationWorkaround
    {
        struct
        {
            Timestamp _v1, _v2;
        };
    };

    using LFun = LoopFunction<Renderer, ShareData>;

    EventLoopData(
        UqPtr<Renderer>&&  r,
        UqPtr<ShareData>&& d,
        LFun&&             s,
        LFun&&             l,
        LFun&&             c,
        Properties&&       visual,
        u32                flags = 0) :

        renderer(std::move(r)),
        data(std::move(d)), setup(s), loop(l), cleanup(c), visual(visual),
        flags(flags)
    {
    }

    C_DEPRECATED_S("Use leaner initializer")
    EventLoopData(
        UqPtr<Renderer>&&  r,
        UqPtr<ShareData>&& d,
        LFun&&             s,
        LFun&&             l,
        LFun&&             c,
        u32                flags,
        DummyDeprecationWorkaround&&,
        Properties&& visual) :
        EventLoopData(
            std::move(r),
            std::move(d),
            std::move(s),
            std::move(l),
            std::move(c),
            std::move(visual),
            flags)
    {
    }

    enum Flags
    {
        TimeLimited = 0x1,
    };

    UqPtr<Renderer>  renderer;
    UqPtr<ShareData> data;

    LoopFunction<Renderer, ShareData> setup;
    LoopFunction<Renderer, ShareData> loop;
    LoopFunction<Renderer, ShareData> cleanup;

    u32 flags;

    union
    {
        struct
        {
            Timestamp start;
            Timestamp max;
        } time;
    };

    Properties visual;

    FORCEDINLINE Renderer& r()
    {
        return *renderer.get();
    }
    FORCEDINLINE ShareData* d()
    {
        return data.get();
    }
};

template<typename R, typename D>
FORCEDINLINE EventLoopData<R, D>* MkEventLoop(
    UqPtr<R>&& renderer, UqPtr<D>&& data)
{
    return new EventLoopData<R, D>{std::move(renderer),
                                   std::move(data),
                                   {},
                                   {},
                                   {},

                                   0,
                                   {},
                                   {}};
}

namespace CfEventFunctions {
template<typename RendType, typename DataType>
void WrapEventFunction(void* data, int event)
{
    using ELD = EventLoopData<RendType, DataType>;

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
            CString error;
            Profiler::DeepPushContext("Renderer::Setup");
            /* By default, try to load the highest GL version */
            if(LoadHighestVersion(&edata->r(), edata->visual, &error))
            {
                Profiler::DeepPopContext();
                Profiler::DeepPushContext("Event::Setup");
                edata->setup(edata->r(), edata->d());
                Profiler::DeepPopContext();
                CurrentState = 1;
            } else
            {
                Profiler::DeepPopContext();
                throw undefined_behavior(
                    "EventApplication::setup failed: " + error);
            }
        }
        break;

    case CoffeeHandle_Loop:
    {
        /* We retrieve the current thread's RuntimeQueue if it exists,
         *  and process it regularly. */
        if(CurrentState == 1)
        {
            Profiler::DeepPushContext("Event::Loop");
            edata->loop(edata->r(), edata->d());
            Profiler::DeepPopContext();
        }

        runtime_queue_error ec;

        if(RuntimeQueue::GetCurrentQueue(ec) && !ec)
        {
            RuntimeQueue::GetCurrentQueue(ec)->executeTasks();
            C_ERROR_CHECK(ec);
        }

        break;
    }
    case CoffeeHandle_Cleanup:
        if(CurrentState == 1)
        {
            CurrentState = 0;
            Profiler::DeepPushContext("Event::Cleanup");
            edata->cleanup(edata->r(), edata->d());
            Profiler::DeepPopContext();

            Profiler::DeepPushContext("Renderer::Cleanup");
            edata->r().cleanup();
            Profiler::DeepPopContext();

#if defined(COFFEE_NO_ATEXIT)
            /* TODO: Fix atexit() handlers */
            libc::signal::exit(libc::signal::sig::abort);
//            auto const& ex = Cmd::GetAtExit();
//            for(auto it = ex.rbegin(); it != ex.rend(); it++)
//                (*it)();
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
    using ELD = EventLoopData<RendType, DataType>;

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

        EventHandlingVector[g->type].func(&edata->r(), event, p1, p2, p3);

        break;
    }
    default:
        break;
    }
}
} // namespace CfEventFunctions

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
        using FPtr = void (*)(void*, EvType const& e, c_cptr);

        FPtr    func;
        cstring name;
        void*   user_ptr;
    };

    using EventHandlerI = EventHandler<Input::CIEvent>;
    using EventHandlerD = EventHandler<Event>;

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

    virtual void injectEvent(Input::CIEvent const&, c_cptr) = 0;

    virtual void injectEvent(Event const&, c_cptr) = 0;

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
            start_time = Time<>::CurrentTimestamp<Chrono::microseconds>();

        return bigscalar(
                   Time<>::CurrentTimestamp<Chrono::microseconds>() -
                   start_time) *
               1_us;
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
     * \brief Stores a reference to an event loop to be used with the
     * application. \param eventloop Generic pointer to EventLoopData<>
     * structure
     */
    virtual void registerEventLoop(void*)
    {
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

#define SUSPRESUME_FUN(var, cond, fun, extrafun)               \
    template<typename Renderer, typename Data>                 \
    static void var(void* ptr, Event const& e, c_cptr)         \
    {                                                          \
        if(e.type == Event::cond)                              \
        {                                                      \
            EventLoopData<Renderer, Data>& evdata =            \
                *C_FCAST<EventLoopData<Renderer, Data>*>(ptr); \
            extrafun(evdata);                                  \
            evdata.fun(evdata.r(), evdata.d());                \
        }                                                      \
    }

    SUSPRESUME_FUN(resumeFunction, IsForeground, setup, resumeExtra)
    SUSPRESUME_FUN(suspendFunction, IsBackground, cleanup, suspendExtra)

#undef SUSPRESUME_FUN

    template<typename Renderer, typename Data>
    struct EventExitHandler
    {
        using ELD = EventLoopData<Renderer, Data>;

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
    C_DEPRECATED static i32 execEventLoop(
        EventLoopData<Renderer, Data>& ev, Properties& visual, CString& err)
    {
        return execEventLoop(
            MkUqWrap<EventLoopData<Renderer, Data>>(&ev), visual, err);
    }

    template<
        typename Renderer,
        typename Data,

        typename implements<EventApplication, Renderer>::type* = nullptr

        >
    static i32 execEventLoop(
        UqPtr<EventLoopData<Renderer, Data>>&& ev, Properties& visual, CString&)
    {
        if(!ev)
            return -1;

        Profiler::DeepPushContext("EventApplication::execEventLoop");

        using ELD = EventLoopData<Renderer, Data>;

        static cstring suspend_str = "Suspend handler";
        static cstring resume_str  = "Resume handler";

        Renderer& r = ev->r();

        /* Because MSVC++ sucks, I can't use a struct initializer list for this
         * :( */
        EventHandlerD suspend_data = {};
        EventHandlerD resume_data  = {};
        suspend_data.user_ptr = resume_data.user_ptr = ev.get();
        suspend_data.func = suspendFunction<Renderer, Data>;
        resume_data.func  = resumeFunction<Renderer, Data>;
        suspend_data.name = suspend_str;
        resume_data.name  = resume_str;

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

        static UqPtr<ELD> local_event_data = std::move(ev);

        local_event_data->visual   = visual;
        coffee_event_handling_data = local_event_data.get();
        CoffeeEventHandle          = WrapEventFunction<Renderer, Data>;
        CoffeeEventHandleNA        = WrapEventFunctionNA<Renderer, Data>;

#if defined(COFFEE_EMSCRIPTEN)
        /* Under emscripten, only the loop function is used later */
        emscripten_set_main_loop_arg(
            [](void* data) {
                CoffeeEventHandle(data, CoffeeHandle_Setup);
                CoffeeEventHandle(data, CoffeeHandle_Loop);
            },
            local_event_data.get(),
            0,
            1);
#endif

        libc::signal::register_atexit([]() { local_event_data.release(); });

#if defined(COFFEE_USE_APPLE_GLKIT) || \
    defined(COFFEE_USE_ANDROID_NATIVEWIN) || defined(COFFEE_EMSCRIPTEN)
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

        auto time     = local_event_data->time;
        auto ev_flags = local_event_data->flags;

        /* For timed runs, set the starting time */
        time.start = Time<>::CurrentTimestamp();

        /* In this case, event processing happens in a tight
         *  loop that happens regardless of outside events.
         * On platforms with their own event loops, this does not work.
         */

        Profiler::DeepPushContext("EventApplication::Event loop");
        while(!local_event_data->renderer->closeFlag())
        {
            CoffeeEventHandleCall(CoffeeHandle_Loop);

            if(ev_flags & ELD::TimeLimited &&
               Time<>::CurrentTimestamp() > (time.start + time.max))
            {
                auto qevent =
                    Input::CIEvent::Create(0, Input::CIEvent::QuitSign);
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

} // namespace Display
} // namespace Coffee
