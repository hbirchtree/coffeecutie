#pragma once

#include "glw.h"

#include <coffee/core/task_queue/task.h>

namespace gl {

#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
template<typename Dependency, typename Out, bool Moveable = true>
struct gpu_dependent_task : rq::dependent_task<Dependency, Out, Moveable>
{
    virtual bool ready() override final
    {
        if(!rq::dependent_task<Dependency, Out, Moveable>::ready())
            return false;
        return gpu_ready();
    }

    virtual bool cancelled() override final
    {
        if(rq::dependent_task<Dependency, Out, Moveable>::cancelled())
            return true;
        using glw =
            std::conditional_t<gl::core::enabled, gl::core::v33, gl::es::v30>;
        return !glw::is_sync(fence);
    }

    bool gpu_ready()
    {
        using glw =
            std::conditional_t<gl::core::enabled, gl::core::v33, gl::es::v30>;
        using namespace std::chrono_literals;

        if(rq::detail::clock::now() <= next_check)
            return false;
        next_check = rq::detail::clock::now() + 1s;

        gl::group::sync_status status = glw::client_wait_sync(
            fence, gl::group::sync_object_mask::sync_flush_commands_bit, 0);
        switch(status)
        {
        case gl::group::sync_status::already_signaled:
        case gl::group::sync_status::condition_satisfied:
        case gl::group::sync_status::wait_failed:
            return true;
        default:
            return false;
        }
    }

    GLsync                        fence{};
    rq::detail::clock::time_point next_check{};
};
#endif

} // namespace gl
