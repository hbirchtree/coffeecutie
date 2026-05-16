#include <glscreenshot/screenshot.h>

#include <coffee/core/CProfiling>
#include <coffee/core/debug/formatting.h>
#include <coffee/core/task_queue/task.h>
#include <glw/glw.h>
#include <glw/gpu_dependent_task.h>
#include <peripherals/typing/geometry/size.h>
#include <peripherals/typing/vectors/vector_types.h>

#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
#include <coffee/comp_app/dummy_plug.h>
#endif

namespace glscreenshot {

using namespace gl::group;

using glw = std::conditional<
    gl::core::enabled,
    gl::core::v33,
#if GLEAM_MAX_VERSION_ES >= 0x300
    gl::es::v30
#else
    gl::es::v20
#endif
    >::type;

ScreenshotProvider::ScreenshotProvider()
    : m_main_queue(rq::runtime_queue::GetCurrentQueue().assume_value())
{
}

comp_app::size_2d_t ScreenshotProvider::size() const
{
    std::array<GLint, 4> tmp{};
    glw::get_integerv(get_prop::viewport, semantic::Span<GLint>(tmp));
    return {tmp[2], tmp[3]};
}

std::future<ScreenshotProvider::dump_t> ScreenshotProvider::pixels()
{
    using namespace ::typing::vector_types;

    if(m_pbo_fence)
        return {};

    Coffee::DProfContext _("glscreenshot::ScreenshotProvider::pixels");

    auto major_version = std::min(m_config->version.major, 99);
#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
    if(m_dummy_config)
        if(auto dummy_ver =
               m_dummy_config->graphics_config.value("major", i32(99));
           dummy_ver != 99)
            major_version = dummy_ver;
#endif
    const bool use_pbo = major_version >= 3
#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
                         && !m_dummy_config->enabled
#endif
        ;

    auto read_pixels = [this, use_pbo] {
        auto                        size_ = size();
        std::vector<libc_types::u8> data(size_.area() * 4);

        Coffee::cDebug(
            "Capturing {}x{} screenshot, alloc'ing {} bytes",
            size_.w,
            size_.h,
            data.size());

        // First, stash away the current framebuffer binding
        i32 currentBinding = 0;
        glw::get_integerv(
#if GLEAM_MAX_VERSION >= 0x100 || GLEAM_MAX_VERSION_ES >= 0x300
            use_pbo ? gl::group::get_prop::draw_framebuffer_binding :
#endif
                    gl::group::get_prop::framebuffer_binding,
            semantic::SpanOne(currentBinding));
#if GLEAM_MAX_VERSION >= 0x100 || GLEAM_MAX_VERSION_ES >= 0x300
        if(use_pbo)
        {
            glw::bind_framebuffer(
                gl::group::framebuffer_target::draw_framebuffer, 0);
            glw::read_buffer(gl::group::read_buffer_mode::back);
        } else
#endif
            glw::bind_framebuffer(
                gl::group::framebuffer_target::framebuffer, 0);

        // Now set up the framebuffer copy
        semantic::concepts::offset_span offset;
#if GLEAM_MAX_VERSION >= 0x330 || GLEAM_MAX_VERSION_ES >= 0x300
        constexpr auto pixel_pack_buffer = buffer_target_arb::pixel_pack_buffer;
        if(use_pbo)
        {
            if(!m_pbo)
            {
                glw::gen_buffers(semantic::SpanOne(m_pbo));
            }
            glw::bind_buffer(pixel_pack_buffer, m_pbo);
            glw::buffer_data(
                pixel_pack_buffer,
                semantic::concepts::null_span<>::of(data.size()),
                gl::group::buffer_usage_arb::dynamic_copy);
            // offset is already correctly initialized, no work to do
        } else
#endif
        {
            offset = semantic::concepts::offset_span::of(data.data());
        }
        glw::read_pixels(
            Veci2{0, 0},
            size_,
            pixel_format::rgba,
            pixel_type::unsigned_byte,
            offset);

#if GLEAM_MAX_VERSION >= 0x330 || GLEAM_MAX_VERSION_ES >= 0x300
        if(use_pbo)
        {
            m_pbo_fence = glw::fence_sync(
                gl::group::sync_condition::sync_gpu_commands_complete,
                gl::group::sync_behavior_flags::none);
            glw::bind_buffer(pixel_pack_buffer, 0);
        }
#endif

        // Aaaand restore state
        glw::bind_framebuffer(
#if GLEAM_MAX_VERSION >= 0x100 || GLEAM_MAX_VERSION_ES >= 0x300
            use_pbo ? gl::group::framebuffer_target::draw_framebuffer :
#endif
                    gl::group::framebuffer_target::framebuffer,
            currentBinding);
        return dump_t{
            .size   = size_,
            .format = typing::pixels::pix_fmt::RGBA8,
            .data   = std::move(data),
        };
    };
#if GLEAM_MAX_VERSION >= 0x100 || GLEAM_MAX_VERSION_ES >= 0x300
    if(use_pbo)
    {
        using libc_types::u8;
        constexpr auto pixel_pack_buffer = buffer_target_arb::pixel_pack_buffer;

        using gpu_buffer = semantic::Span<const u8>;

        /* Synchronously set up the GPU copy to the PBO,
         * should take virtually no time */
        auto dump = read_pixels();

        /* Set up copy from PBO after it's complete */
        auto map_buffer = [this, size = dump.data.size()] {
            Coffee::DProfContext _("ScreenshotProvider::PBO copy to CPU");
            glw::bind_buffer(pixel_pack_buffer, m_pbo);
            auto ptr = glw::map_buffer(
                pixel_pack_buffer, gl::group::buffer_access_arb::read_only);
            if(!ptr)
            {
                auto err = glw::get_error(gl::error_check::off);
                Coffee::cDebug(
                    "Failed to map_buffer: {}", static_cast<u32>(err));
            }
            glw::bind_buffer(pixel_pack_buffer, 0);
            return gpu_buffer(reinterpret_cast<u8 const*>(ptr), size);
        };
        auto copy_buffer = [dump =
                                std::move(dump)](gpu_buffer* buffer) mutable {
            Coffee::DProfContext _("ScreenshotProvider::Copy to CPU memory");
            std::copy(buffer->begin(), buffer->end(), dump.data.begin());
            return dump;
        };
        auto unmap_buffer = [this](dump_t* dump) {
            Coffee::DProfContext _("ScreenshotProvider::Unmapping buffer");
            glw::bind_buffer(pixel_pack_buffer, m_pbo);
            glw::unmap_buffer(pixel_pack_buffer);
            glw::bind_buffer(pixel_pack_buffer, 0);

            glw::delete_sync(reinterpret_cast<GLsync>(m_pbo_fence));
            m_pbo_fence = nullptr;

            return std::move(*dump);
        };

        /* Set up wait for PBO fence + copy to CPU,
         * gpu_dependent_task does the waiting for us */
        auto map_op = gl::gpu_dependent_task<void, gpu_buffer>::CreateSource<
            gl::gpu_dependent_task<void, gpu_buffer>>(std::move(map_buffer));
        map_op->fence = reinterpret_cast<GLsync>(m_pbo_fence);
        auto copy_op  = rq::dependent_task<gpu_buffer, dump_t>::CreateProcessor(
            map_op->output.get_future(), std::move(copy_buffer));
        auto unmap_op = rq::dependent_task<dump_t, dump_t>::CreateProcessor(
            copy_op->output.get_future(), std::move(unmap_buffer));
        auto unmap_future = unmap_op->output.get_future();

        /* Then queue and wait... */
        rq::runtime_queue::Queue(m_main_queue, std::move(unmap_op))
            .assume_value();
        rq::runtime_queue::Queue(m_work_queue, std::move(copy_op))
            .assume_value();
        rq::runtime_queue::Queue(m_main_queue, std::move(map_op))
            .assume_value();

        m_waiting = true;
        return unmap_future;
    } else
#endif
    {
        /* Set up a task to run the read + copy */
        auto out = rq::dependent_task<void, dump_t>::CreateSource(read_pixels);
        auto dump_future = out->output.get_future();
        rq::runtime_queue::Queue(m_main_queue, std::move(out)).assume_value();
        return dump_future;
    }
}

} // namespace glscreenshot
