#pragma once

#include "inputapplication.h"
#include <coffee/core/eventprocess.h>

#include <coffee/core/plat/plat_timing.h>

#if defined(__EMSCRIPTEN__)
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

    Renderer* renderer;
    ShareData* data;

    std::function<void(Renderer&, ShareData*)> setup;
    std::function<void(Renderer&, ShareData*)> loop;
    std::function<void(Renderer&, ShareData*)> cleanup;

    uint32 flags;

    union
    {
        struct{
            Timestamp start;
            Timestamp max;
        } time;
    };
};



class EventApplication : public InputApplication
{
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

    /*!
     * \brief Get current context time, can be returned by the context or the system
     * \return Current context time
     */
    virtual bigscalar contextTime() const = 0;

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
        m_closeFlag = true;
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
#if defined(__EMSCRIPTEN__)
        emscripten_resume_main_loop();
#endif
    }
    template<typename Renderer, typename Data>
    STATICINLINE void suspendExtra(EventLoopData<Renderer, Data>&)
    {
#if defined(__EMSCRIPTEN__)
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
            evdata.fun(*evdata.renderer, evdata.data); \
        } \
    }

    SUSPRESUME_FUN(resumeFunction, IsForeground, setup, resumeExtra)
    SUSPRESUME_FUN(suspendFunction, IsBackground, cleanup, suspendExtra)

#if defined(__EMSCRIPTEN__)
    template<typename R, typename D>
    static void emscripten_looper(void* arg)
    {
        auto eventloop = C_FCAST< EventLoopData<R,D>* >(arg);

        eventloop->loop(*eventloop->renderer, eventloop->data);
    }

#endif

#undef SUSPRESUME_FUN

    template<typename Renderer, typename Data>
    static int32 execEventLoop(EventLoopData<Renderer,Data>& ev,
                               CDProperties& visual, CString& err)
    {
        using ELD = EventLoopData<Renderer, Data>;

        static cstring suspend_str = "Suspend handler";
        static cstring resume_str = "Resume handler";

        Renderer& r = *ev.renderer;

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


#if defined(__EMSCRIPTEN__)
        emscripten_set_main_loop_arg(emscripten_looper<Renderer,Data>, &ev, 0, 0);
#endif

        if(!(*ev.renderer).init(visual, &err))
        {
            return -1;
        }

        ev.time.start = Time::CurrentTimestamp();

#if !defined(COFFEE_ANDROID)
        r.injectEvent(CDEvent{0, CDEvent::IsForeground}, nullptr);
#endif

#if !defined(__EMSCRIPTEN__)
        while(!ev.renderer->closeFlag())
        {
            ev.loop(*ev.renderer, ev.data);

            if(ev.flags & ELD::TimeLimited &&
                    Time::CurrentTimestamp() > (ev.time.start + ev.time.max))
            {
                r.injectEvent(CIEvent::Create(0, CIEvent::QuitSign), nullptr);
            }
        }
#endif

        (*ev.renderer).cleanup();
        return 0;
    }

    void exec()
    {

    }
};

}
}
