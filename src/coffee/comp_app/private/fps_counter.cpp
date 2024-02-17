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
    get() = 0;
}

void FrameCounter::start_frame(ContainerProxy& p, const time_point& current)
{
    get()++;

    if(next_print < current)
    {
        next_print = current + std::chrono::seconds(1);

        Coffee::cDebug("FPS: {0}", get());
        get() = 0;
    }
    if(close_time.has_value() && *close_time < current)
    {
        if(auto window = p.underlying().service<comp_app::Windowing>())
            window->close();
    }
}

} // namespace comp_app
