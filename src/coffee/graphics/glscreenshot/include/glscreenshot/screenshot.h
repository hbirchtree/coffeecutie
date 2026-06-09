#pragma once

#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/services.h>
#include <coffee/core/task_queue/task.h>

namespace comp_app::dummy_plug {
struct Config;
}

namespace glscreenshot {

struct ScreenshotProvider
    : comp_app::interfaces::ScreenshotProvider
    , comp_app::AppService<ScreenshotProvider, comp_app::ScreenshotProvider>
    , std::enable_shared_from_this<ScreenshotProvider>
{
    using type = ScreenshotProvider;

    ScreenshotProvider();

    comp_app::size_2d_t size() const;
    std::future<dump_t> pixels();

    /* Synchronous framebuffer readback on the calling (GL) thread. Used by the
     * dummy plug, which tears the app down before the async queues can flush. */
    dump_t capture_sync();

    void set_worker(rq::runtime_queue* worker)
    {
        m_work_queue = worker;
    }

    bool captureRequested()
    {
        return m_capture_requested;
    }

    void signalCaptureReady(libc_types::u32 hnd);

    comp_app::GLConfig const*           m_config{nullptr};
    comp_app::dummy_plug::Config const* m_dummy_config{nullptr};

    libc_types::u32    m_pbo{0};
    void*              m_pbo_fence{nullptr};
    bool               m_waiting{false};
    rq::runtime_queue* m_main_queue{nullptr};
    rq::runtime_queue* m_work_queue{nullptr};

    bool                                   m_capture_requested{false};
    std::promise<dump_t>                   m_dump_promise{};
    std::function<dump_t(libc_types::u32)> m_pending_capture{};

private:
    dump_t read_pixels(libc_types::u32 fbo, bool use_pbo, bool use_fbo);
};

} // namespace glscreenshot
