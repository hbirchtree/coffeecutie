#pragma once

#include "inputapplication.h"

namespace Coffee{
namespace CDisplay{

class EventApplication : public InputApplication
{
public:

    template<typename EvType>
    struct EventHandler
    {
        using FPtr = void(*)(void*,EvType const& e, c_cptr);

        FPtr func;
        cstring name;
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
    virtual void pollEvents() = 0;

    /*!
     * \brief Allow installation of event handlers without implementing the class
     */
    virtual void installEventHandler(EventHandlerI)
    {
    }
    /*!
     * \brief Allow installation of event handlers without implementing the class
     */
    virtual void installEventHandler(EventHandlerD)
    {
    }

    /*!
     * \brief Get current context time, can be returned by the context or the system
     * \return Current context time
     */
    virtual bigscalar contextTime() const = 0;

    /*!
     * \brief A flag used to determine when a render loop should terminate
     * \return Whether or not a render loop should terminate
     */
    virtual bool closeFlag() const = 0;
};

}
}
