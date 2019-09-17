#pragma once

#include <coffee/comp_app/eventapp_wrapper.h>
#include <coffee/comp_app/services.h>
#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/typing/enum/debug/severity.h>

namespace Coffee {
namespace Display {

struct EventApplication
{
    struct dummy_type
    {
        using event_type = void;
    };

    template<typename EvType>
    using EvWrapper = typename comp_app::BasicEventBus<EvType>::EvData;

    template<class EvType>
    struct EventHandler
    {
        template<typename Handler>
        static EvWrapper<EvType> MkHandler(Handler&& hnd, cstring = nullptr)
        {
            return {
                100, [hnd](EvType& event, c_ptr data) mutable {
                    if(event.type == Handler::event_type::event_type)
                        hnd(event,
                            C_RCAST<typename Handler::event_type const*>(data));
                }};
        }

        template<typename SubEventType>
        static EvWrapper<EvType> MkFunc(
            Function<void(EvType const&, SubEventType const*)>&& func,
            cstring = nullptr)
        {
            return {100, [func](EvType& event, c_ptr data) mutable {
                        func(event, C_RCAST<SubEventType const*>(data));
                    }};
        }
    };
};

template<class EvType>
using EHandle = EventApplication::EventHandler<EvType>;

struct CSDL2Renderer
{
    void swapBuffers()
    {
    }
    void pollEvents()
    {
    }

    Size windowSize() const
    {
        auto window = m_container->service<comp_app::Windowing>();
        if(window)
            return window->size();
        return {};
    }
    Size framebufferSize() const
    {
        auto fb = m_container->service<comp_app::GraphicsFramebuffer>();
        if(fb)
            return fb->size();
        return {};
    }
    int monitor() const
    {
        return 0;
    }
    void popErrorMessage(
        semantic::debug::Severity, CString const&, CString const&) const
    {
    }
    void setWindowTitle(CString const& title)
    {
        auto winInfo = m_container->service<comp_app::WindowInfo>();
        if(winInfo)
            winInfo->setName(title);
    }
    void showWindow()
    {
        auto window = m_container->service<comp_app::Windowing>();
        if(window)
            window->show();
    }
    void closeWindow()
    {
        auto window = m_container->service<comp_app::Windowing>();
        if(window)
            window->close();
    }

    comp_app::detail::ServiceRef<comp_app::Windowing> window()
    {
        return m_container->service_ref<comp_app::Windowing>();
    }

    void installEventHandler(
        EventApplication::EvWrapper<Input::CIEvent> handler)
    {
        auto bus =
            m_container->service<comp_app::BasicEventBus<Input::CIEvent>>();
        bus->addEventData(std::move(handler));
    }

    void installEventHandler(EventApplication::EvWrapper<Event> handler)
    {
        auto bus = m_container->service<comp_app::BasicEventBus<Event>>();
        bus->addEventData(std::move(handler));
    }

    comp_app::detail::EntityContainer* m_container;
};

using RendererInterface = CSDL2Renderer;

} // namespace Display
} // namespace Coffee
