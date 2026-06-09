#pragma once

#include <coffee/comp_app/services.h>

#include <queue>
#include <termios.h>

namespace terminal {

struct TerminalInput
    : comp_app::interfaces::BasicKeyboardInput
    , comp_app::AppService<TerminalInput>
    , comp_app::AppLoadableService
{
    using readable_services = comp_app::subsystem_list<
        comp_app::BasicEventBus<Coffee::Input::CIEvent>>;
    using proxy_type = comp_app::detail::restricted::proxy_t<TerminalInput>;

    TerminalInput()
    {
        priority = 514;
    }

    virtual void load(entity_container& e, comp_app::app_error&) override;
    virtual void unload(entity_container& e, comp_app::app_error&) override;

    void start_restricted(proxy_type& p, time_point const&);

  private:
    struct termios                                          m_original_termios;
    std::queue<std::pair<libc_types::u16, libc_types::u32>> m_pressed;
    bool                                                    m_active = false;
};

using Services = comp_app::subsystem_list<TerminalInput>;

} // namespace terminal
