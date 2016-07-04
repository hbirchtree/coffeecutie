#pragma once

#include "inputapplication.h"
#include <coffee/core/eventprocess.h>

namespace Coffee{
namespace Display{

class EventApplication : public InputApplication
{
    bool m_closeFlag = false;
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
};

}
}
