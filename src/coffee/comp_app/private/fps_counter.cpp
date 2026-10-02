#include <coffee/comp_app/fps_counter.h>

#include <coffee/comp_app/services.h>
#include <coffee/components/proxy.h>
#include <coffee/core/debug/formatting.h>

namespace comp_app {

FrameCounter::FrameCounter()
{
    if(auto interval = platform::env::var("FRAMECOUNTER_RUNTIME_SECONDS"))
    {
        auto num_seconds = std::stoi(*interval);
        close_time = compo::clock::now() + std::chrono::seconds(num_seconds);
    }
    current = 0;
}

void FrameCounter::start_frame(ContainerProxy& p, const time_point& current)
{
    this->current++;

    if(next_print < current)
    {
        next_print = current + std::chrono::seconds(1);

        Coffee::cDebug("FPS: {0}", this->current);
        this->current = 0;
    }
    if(close_time.has_value() && *close_time < current)
    {
        if(auto window = p.service<comp_app::Windowing>())
            window->close();
    }
}

} // namespace comp_app
