#include <coffee/osmesa/osmesa_comp.h>

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/core/debug/formatting.h>
#include <coffee/image/cimage.h>

using Coffee::Logging::cVerbose;

namespace osmesa {
namespace {
libc_types::u32 numFrames = 0;
}

void Windowing::load(entity_container& e, comp_app::app_error& ec)
{
    std::array<int, 12 + 1> attribs = {{
        // clang-format off
        OSMESA_FORMAT,                OSMESA_RGBA,
        OSMESA_DEPTH_BITS,            32,
        OSMESA_STENCIL_BITS,          8,
        OSMESA_PROFILE,               OSMESA_CORE_PROFILE,
        OSMESA_CONTEXT_MAJOR_VERSION, 4,
        OSMESA_CONTEXT_MINOR_VERSION, 5,
        0,
        // clang-format on
    }};

    m_context = OSMesaCreateContextAttribs(attribs.data(), nullptr);
    if(!m_context)
    {
        ec = "Failed to create OSMesa context";
        ec = comp_app::AppError::NoContext;
        return;
    }
    m_framebuffer.resize(1280u * 720u);
    if(!OSMesaMakeCurrent(m_context, m_framebuffer.data(), GL_UNSIGNED_BYTE, 1280, 720))
    {
        ec = comp_app::AppError::NoFramebuffer;
        return;  
    }
    {
        auto& config = e.service<comp_app::AppLoader>()
                           ->config<comp_app::GraphicsBindingConfig>();
        config.loader = [](const char* name) {
            return reinterpret_cast<void*>(OSMesaGetProcAddress(name));
        };
    }
    m_targetFrames = 200;
}

void Windowing::end_frame(compo::ContainerProxy&, compo::time_point const&)
{
    if(auto currFrames = ++numFrames; currFrames == m_targetFrames)
    {
        dump_framebuffer();
        close();
    } else
        Coffee::cDebug("Frame #{}/{}", numFrames, m_targetFrames);
}

comp_app::size_2d_t Windowing::size() const
{
    return comp_app::size_2d_t::from_values<comp_app::size_2d_t>(1280, 720);
}

comp_app::window_flags_t Windowing::state() const
{
    return comp_app::window_flags_t::fullscreen;
}

void Windowing::setState(comp_app::window_flags_t)
{
}

using libc_types::u32;

template<typename T>
struct descriptor_t
{
    gsl::span<T> buffer;
    u32 components;
    u32 width;
    u32 height;
    Coffee::Resource output;
};

void Windowing::dump_framebuffer()
{
    using namespace Coffee::resource_literals;
    using namespace Coffee;
    using semantic::RSCA;

    auto save_buffer = []<typename T>(descriptor_t<T>&& desc) {
        semantic::Bytes  encoded;
        stb::stb_error   ec;
        stb::image_const source = stb::image_const::From(
            gsl::span(reinterpret_cast<const char*>(desc.buffer.data()), desc.buffer.size() * desc.components),
            {desc.width, desc.height},
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
            screenshot_file,
            RSCA::WriteOnly | RSCA::Discard | RSCA::NewFile);
    };
    save_buffer(descriptor_t<typing::pixels::rgba_t>{
        .buffer = gsl::span<typing::pixels::rgba_t>(m_framebuffer.data(), m_framebuffer.size()),
        .components = 4,
        .width = 1280,
        .height = 720,
        .output = "framebuffer_color.png"_tmpfile,
    });

    int depthHeight{}, depthWidth{}, depthBytes{};
    void* depthPtr{};
    auto res = OSMesaGetDepthBuffer(
        m_context, &depthWidth, &depthHeight, &depthBytes, &depthPtr);

    if(!res)
    {
        cWarning("Failed to save depth buffer");
        return;
    } else
        cDebug("Depth buffer dimensions: bits={}, height={}, width={}", depthBytes * 8, depthHeight, depthWidth);

    using libc_types::u8;
    using libc_types::f32;
    gsl::span<u32> depthBuffer_(
        static_cast<u32*>(depthPtr), depthHeight * depthWidth);
    std::vector<u8> depthBuffer;
    u32 max_depth = 0, min_depth = 0;
    for(auto pixel : depthBuffer_)
    {
        max_depth = std::max(max_depth, pixel);
        min_depth = std::min(min_depth, pixel);
    }
    f32 scale = 255.f / (max_depth - min_depth);
    cDebug("Depth properties: max={}, min={}", max_depth, min_depth);
    for(auto pixel : depthBuffer_)
    {
        depthBuffer.push_back(static_cast<u8>((pixel - min_depth) * scale));
    }
    save_buffer(descriptor_t<u8>{
        .buffer = gsl::span<u8>(depthBuffer.data(), depthBuffer.size()),
        .components = 1,
        .width = static_cast<u32>(depthWidth),
        .height = static_cast<u32>(depthHeight),
        .output = "framebuffer_depth.png"_tmpfile,
    });
    cDebug("Saved framebuffer");
}

void GraphicsFramebuffer::start_frame(compo::ContainerProxy&, compo::time_point const&)
{
    defaultSwap();
}

void GraphicsFramebuffer::swapBuffers(comp_app::app_error&)
{
    glFinish();
}

} // namespace osmesa
