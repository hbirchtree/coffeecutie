#pragma once

#include <coffee/components/components.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/image/cimage.h>

namespace Coffee {
namespace IMG {

template<typename... Args>
auto create_decoder(std::future<mem_chunk<u8>>&& source, Args... args)
{
    return rq::dependent_task<mem_chunk<u8>, stb::image_rw>::CreateTask(
        std::move(source),
        [args...](mem_chunk<u8>* source) {
            stb::stb_error ec;
            stb::image_rw img;
            if (!stb::LoadData(&img, *source, ec, args...))
                return stb::image_rw();
            return img;
        });
}

} // namespace IMG
} // namespace Coffee
