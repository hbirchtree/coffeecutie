#include <coffee/osmesa/osmesa_comp.h>

#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/core/debug/formatting.h>
#include <coffee/image/cimage.h>

using Coffee::Logging::cVerbose;
using Coffee::Logging::cWarning;

namespace osmesa {
namespace {
libc_types::u32 numFrames = 0;
}

void Windowing::load(entity_container& e, comp_app::app_error& ec)
{
    using typing::pixels::pix_fmt;

    auto const& glConfig =
        e.service<comp_app::AppLoader>()->config<comp_app::GLConfig>();

    auto const&             depthFmt = glConfig.depthFmt;
    std::array<int, 12 + 1> attribs  = {{
        // clang-format off
        OSMESA_FORMAT,                OSMESA_RGBA,
        OSMESA_DEPTH_BITS, depthFmt == comp_app::pix_fmt::Depth16 ? 16 : 32,
        OSMESA_STENCIL_BITS,          8,
        OSMESA_PROFILE,               OSMESA_CORE_PROFILE,
        OSMESA_CONTEXT_MAJOR_VERSION, glConfig.version.major,
        OSMESA_CONTEXT_MINOR_VERSION, glConfig.version.minor,
        0,
        // clang-format on
    }};

    m_context = OSMesaCreateContextAttribs(attribs.data(), nullptr);
    if(!m_context)
    {
        cWarning("Failed to create OSMesa context");
        ec = "Failed to create OSMesa context";
        ec = comp_app::AppError::NoContext;
        return;
    }
    auto const& windowConfig =
        e.service<comp_app::AppLoader>()->config<comp_app::WindowConfig>();
    m_size = windowConfig.size;
    m_framebuffer.resize(m_size.w * m_size.h);
    if(!OSMesaMakeCurrent(
           m_context,
           m_framebuffer.data(),
           GL_UNSIGNED_BYTE,
           m_size.w,
           m_size.h))
    {
        cWarning("Failed to create OSMesa framebuffer");
        ec = comp_app::AppError::NoFramebuffer;
        return;
    }
    OSMesaPixelStore(OSMESA_Y_UP, GL_FALSE);
    {
        auto& config = e.service<comp_app::AppLoader>()
                           ->config<comp_app::GraphicsBindingConfig>();
        config.loader = [](const char* name) {
            return reinterpret_cast<void*>(OSMesaGetProcAddress(name));
        };
    }

    using Coffee::Display::Event;
    e.service<comp_app::BasicEventBus<Event>>()->addEventData({
        .prio = 0,
        .handler =
            [this](Event& event, libc_types::c_ptr) {
                if(event.type != Event::TransitionBackground)
                    return;
                dump_framebuffer();
            },
    });
}

comp_app::size_2d_t Windowing::size() const
{
    return m_size;
}

comp_app::window_flags_t Windowing::state() const
{
    return comp_app::window_flags_t::fullscreen;
}

void Windowing::setState(comp_app::window_flags_t)
{
}

using libc_types::i32;
using libc_types::u32;

template<typename T>
struct descriptor_t
{
    gsl::span<T>     buffer;
    u32              components;
    i32              width;
    i32              height;
    Coffee::Resource output;
};

void Windowing::dump_framebuffer()
{
    Coffee::DProfContext _;

    using namespace Coffee::resource_literals;
    using namespace Coffee;
    using semantic::RSCA;

    auto save_buffer = []<typename T>(descriptor_t<T>&& desc) {
        semantic::Bytes  encoded;
        stb::stb_error   ec;
        stb::image_const source = stb::image_const::From(
            gsl::span(
                reinterpret_cast<const char*>(desc.buffer.data()),
                desc.buffer.size() * desc.components),
            {static_cast<u32>(desc.width), static_cast<u32>(desc.height)},
            desc.components);

        if(!stb::SavePNG(encoded, source, ec))
        {
            cWarning("Failed to dump screenshot: {0}", ec.message());
            return;
        }
        auto screenshot_file = std::move(desc.output);
        screenshot_file.data_ro =
            semantic::BytesConst::ofBytes(encoded.data, encoded.size);
        FileCommit(
            screenshot_file, RSCA::WriteOnly | RSCA::Discard | RSCA::NewFile);
    };
    save_buffer(
        descriptor_t<typing::pixels::rgba_t>{
            .buffer = gsl::span<typing::pixels::rgba_t>(
                m_framebuffer.data(), m_framebuffer.size()),
            .components = 4,
            .width      = m_size.w,
            .height     = m_size.h,
            .output     = "framebuffer_color.png"_tmpfile,
        });

    int   depthHeight{}, depthWidth{}, depthBytes{};
    void* depthPtr{};
    auto  res = OSMesaGetDepthBuffer(
        m_context, &depthWidth, &depthHeight, &depthBytes, &depthPtr);

    if(!res)
    {
        cWarning("Failed to save depth buffer");
        return;
    }

    using libc_types::f32;
    using libc_types::u8;
    gsl::span<u32> depthBuffer_(
        static_cast<u32*>(depthPtr), depthHeight * depthWidth);
    std::vector<u8> depthBuffer;
    u32             max_depth = 0, min_depth = std::numeric_limits<u32>::max();
    for(auto pixel : depthBuffer_)
    {
        max_depth = std::max(max_depth, pixel);
        min_depth = std::min(min_depth, pixel);
    }
    f64 scale = 255. / static_cast<f64>(max_depth - min_depth);
    if((max_depth - min_depth) == 0)
    {
        scale     = 1.f;
        min_depth = 0u;
    }
    cDebug("Depth properties: max={}, min={}", max_depth, min_depth);
    for(auto pixel : depthBuffer_)
    {
        depthBuffer.push_back(static_cast<u8>((pixel - min_depth) * scale));
    }
    save_buffer(
        descriptor_t<u8>{
            .buffer     = gsl::span<u8>(depthBuffer.data(), depthBuffer.size()),
            .components = 1,
            .width      = depthWidth,
            .height     = depthHeight,
            .output     = "framebuffer_depth.png"_tmpfile,
        });
    cDebug("Saved framebuffer");
}

void GraphicsFramebuffer::load(entity_container& e, comp_app::app_error&)
{
    m_size = e.service<comp_app::Windowing>()->size();
}

void GraphicsFramebuffer::end_frame(
    compo::ContainerProxy&, compo::time_point const&)
{
    defaultSwap();
}

void GraphicsFramebuffer::swapBuffers(comp_app::app_error&)
{
    glFinish();
}

} // namespace osmesa
