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

    FORCEDINLINE Renderer& r() {return *renderer.get();}
    FORCEDINLINE ShareData* d() {return data.get();}
};

template<typename RendType, typename DataType>
void WrapEventFunction(void* data, int event)
{
    using ELD = EventLoopData<RendType,DataType>;
    
    ELD* edata = C_FCAST<ELD*>(data);

    switch(event)
    {
        case CoffeeHandle_Setup:
        edata->setup(*edata->renderer, edata->data);
        break;
        
        case CoffeeHandle_Loop:
        edata->loop(*edata->renderer, edata->data);
        break;
        
        case CoffeeHandle_Cleanup:
        edata->cleanup(*edata->renderer, edata->data);
        break;
        
        default:
        break;
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
        if(!EventProcess(5))
        {
            m_closeFlag = true;
        }
    }

    /*!
     * \brief Allow installation of event handlers without implementing the class
     */
    virtual bool installEventHandler(EventHandlerI)
    {
        return false;
    }
    /*!
     * \brief Allow installation of event handlers without implementing the class
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
     * \brief Get current context time, can be returned by the context or the system
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

#if defined(COFFEE_EMSCRIPTEN)
    template<typename R, typename D>
    static void emscripten_looper(void* arg)
    {
        using ELD = EventLoopData<R, D>;

        auto eventloop = C_FCAST< EventLoopData<R,D>* >(arg);

        eventloop->loop(*eventloop->renderer, eventloop->data);

        if(eventloop->flags & ELD::TimeLimited &&
                Time::CurrentTimestamp() > (eventloop->time.start + eventloop->time.max))
        {
            auto qevent = CIEvent::Create(0, CIEvent::QuitSign);
            eventloop->renderer->injectEvent(qevent, nullptr);
        }
    }
#endif

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


        if(!LoadHighestVersion(&ev.r(), visual, &err))
        {
            return -1;
        }

        ev.time.start = Time::CurrentTimestamp();
        
#if defined(COFFEE_USE_APPLE_GLKIT)
        /* Under GLKit, the entry point for setup, update and cleanup
         *  reside in AppDelegate.m
         * Lifecycle is manageed by UIKit in this case
         */
        coffee_event_handling_data = &ev;
        CoffeeEventHandle = WrapEventFunction<Renderer, Data>;
        
        return 0;
#else

#if defined(COFFEE_EMSCRIPTEN)
        // Under emscripten, only the loop function is used later
        emscripten_set_main_loop_arg(emscripten_looper<Renderer,Data>,
                                     &ev, 0, 1);
#endif


#if !defined(COFFEE_ANDROID)
        // Android awaits a foreground event, as it may not be there when started.
        {
            auto fevent = CDEvent::Create(0, CDEvent::IsForeground);
            r.injectEvent(fevent, nullptr);
        }
#else
        // On desktop and etc, we are always ready for setup
        ev.setup(ev.r(), ev.d());
#endif


#if !defined(COFFEE_EMSCRIPTEN)
        /* In this case, event processing happens in a tight loop that happens
            regardless of outside events
           Android might want something better
         */

        RuntimeQueue* rt_queue = RuntimeQueue::GetCurrentQueue();
        while(!ev.renderer->closeFlag())
        {
            ev.loop(ev.r(), ev.d());

            if(ev.flags & ELD::TimeLimited &&
                    Time::CurrentTimestamp() > (ev.time.start + ev.time.max))
            {
                auto qevent = CIEvent::Create(0, CIEvent::QuitSign);
                r.injectEvent(qevent, nullptr);
            }

            rt_queue->executeTasks();
        }
#endif

        using EH = EventExitHandler<Renderer, Data>;

#if defined(COFFEE_EMSCRIPTEN)
        // Emscripten will exit after main()
        EH::ev = &ev;
        Cmd::RegisterAtExit(EH::event_exitFunc);
#else
        // All others exit here
        EH::event_exitFunc(&ev);
#endif

        return 0;
#endif
    }

    void exec()
    {

    }
};

}
}
