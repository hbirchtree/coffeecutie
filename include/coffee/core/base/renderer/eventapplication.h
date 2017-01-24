#pragma once

#include "inputapplication.h"
#include <coffee/core/eventprocess.h>

namespace Coffee{
namespace Display{

template<typename Renderer, typename ShareData>
struct EventLoopData
{
    Renderer* renderer;
    ShareData* data;

    std::function<void(Renderer&, ShareData*)> setup;
    std::function<void(Renderer&, ShareData*)> loop;
    std::function<void(Renderer&, ShareData*)> cleanup;

    uint32 flags;
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

    /*!
     * \brief Stores a reference to an event loop to be used with the application.
     * \param eventloop Generic pointer to EventLoopData<> structure
     */
    virtual void registerEventLoop(void* eventloop)
    {
        C_UNUSED(eventloop);
    }

    template<typename Renderer, typename Data>
    static void execEventLoop(EventLoopData<Renderer,Data>& ev)
    {
        while(!ev.renderer->closeFlag())
            ev.loop(*ev.renderer, *ev.data);
    }
};

}
}
