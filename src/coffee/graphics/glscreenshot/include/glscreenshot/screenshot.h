#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/core/task_queue/task.h>

namespace glscreenshot {

struct ScreenshotProvider
    : comp_app::interfaces::ScreenshotProvider,
      comp_app::AppService<ScreenshotProvider, comp_app::ScreenshotProvider>,
      std::enable_shared_from_this<ScreenshotProvider>
{
    using type = ScreenshotProvider;

    ScreenshotProvider();

    comp_app::size_2d_t size() const;
    std::future<dump_t> pixels();

    void set_worker(rq::runtime_queue* worker)
    {
        m_work_queue = worker;
    }

    libc_types::u32    m_pbo{0};
    void*              m_pbo_fence{nullptr};
    bool               m_waiting{false};
    rq::runtime_queue* m_main_queue{nullptr};
    rq::runtime_queue* m_work_queue{nullptr};
};

} // namespace glscreenshot
