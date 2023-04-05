#include <coffee/imgui/imgui_binding.h>

#include <coffee/graphics/apis/CGLeam>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/interfaces/cgraphics_pixops.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

namespace imgui::detail {

using Coffee::cDebug;
using Coffee::cWarning;

using Coffee::DProfContext;
using libc_types::byte_t;
using libc_types::f32;
using libc_types::f64;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;
using semantic::mem_chunk;
using stl_types::ShPtr;
using typing::geometry::size_2d;
using typing::geometry::size_3d;

using namespace typing::vector_types;

template<typename T>
static auto to_bytes(ImVector<T>& data)
{
    return mem_chunk<byte_t>::ofBytes(data.Data, data.Size).view;
}

struct ImGuiGraphicsData
{
    ShPtr<gfx::vertex_array_t> vao;
    ShPtr<gfx::program_t>      pipeline;
    ShPtr<gfx::buffer_t>       vertices;
    ShPtr<gfx::buffer_t>       elements;

    ShPtr<gfx::texture_2d_t> font_atlas;
    ShPtr<gfx::sampler_t>    font_sampler;

    ShPtr<gfx::texture_2d_t> shell_texture;
    ShPtr<gfx::sampler_t>    shell_sampler;

    Matf4 projection_matrix;
};

void ImGuiGraphicsDataDeleter::operator()(ImGuiGraphicsData* p)
{
    delete p;
}

// This is the main rendering function that you have to implement and provide to
// ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure) If text
// or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by
// (0.5f,0.5f) or (0.375f,0.375f)
void ImGuiSystem::submit_draws(Proxy& e)
{
    using namespace std::string_view_literals;

    gleam::system& api = e.subsystem<gleam::system>();

    auto& data      = *m_gfx_data;
    auto  draw_data = ImGui::GetDrawData();

    // Avoid rendering when minimized, scale coordinates for retina displays
    // (screen coordinates != framebuffer coordinates)
    [[maybe_unused]] auto draw_scope = api.debug().scope(IM_API "ImGui render");
    DProfContext          _(IM_API "Rendering draw lists");

    ImGuiIO& io        = ImGui::GetIO();
    int      fb_width  = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int      fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if(fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    data.projection_matrix.d[0] = Vecf4{2.f / io.DisplaySize.x, 0, 0, 0};
    data.projection_matrix.d[1] = Vecf4{0, 2.f / -io.DisplaySize.y, 0, 0};
    data.projection_matrix.d[2] = Vecf4{0, 0, -1.f, 0};
    data.projection_matrix.d[3] = Vecf4{-1.f, 1.f, 0, 1.f};

    auto const& ro_projection = data.projection_matrix;

    auto uniform_list = gfx::make_uniform_list(
        typing::graphics::ShaderStage::Vertex,
        gfx::uniform_pair<const Matf4>{
            {"ProjMtx"sv}, semantic::SpanOne(ro_projection)});

    data.vertices->commit(draw_data->TotalVtxCount * sizeof(ImDrawVert));
    data.elements->commit(draw_data->TotalIdxCount * sizeof(ImDrawIdx));

    auto vtx_mapping = data.vertices->map<ImDrawVert>(0);
    auto idx_mapping = data.elements->map<ImDrawIdx>(0);

    u32 vtx_off = 0, idx_off = 0;
    for(auto n : stl_types::Range<int>(draw_data->CmdListsCount))
    {
        auto cmd_list = draw_data->CmdLists[n];
        std::copy(
            cmd_list->VtxBuffer.begin(),
            cmd_list->VtxBuffer.end(),
            vtx_mapping.begin() + vtx_off);
        std::copy(
            cmd_list->IdxBuffer.begin(),
            cmd_list->IdxBuffer.end(),
            idx_mapping.begin() + idx_off);
        for(auto& cmd : cmd_list->CmdBuffer)
        {
            cmd.VtxOffset += vtx_off;
            cmd.IdxOffset += idx_off;
        }
        vtx_off += cmd_list->VtxBuffer.size();
        idx_off += cmd_list->IdxBuffer.size();
    }

    data.vertices->unmap();
    data.elements->unmap();

    for(auto n : stl_types::Range<int>(draw_data->CmdListsCount))
    {
        auto cmd_list = draw_data->CmdLists[n];
        auto scope_name
            = std::string(IM_API "Command list ") + cmd_list->_OwnerName;
        [[maybe_unused]] auto _
            = api.debug().scope(std::string_view(scope_name.c_str()));

        //        data.vertices->commit(to_bytes(cmd_list->VtxBuffer));
        //        data.elements->commit(to_bytes(cmd_list->IdxBuffer));

        for(auto const& cmd : cmd_list->CmdBuffer)
        {
            [[maybe_unused]] auto _
                = api.debug().scope(IM_API "Command buffer");
            union
            {
                ImTextureID ptr;
                u32         hnd;
            } tex_handle                 = {.ptr = cmd.TextureId};
            data.shell_texture->m_handle = tex_handle.hnd;
            if(cmd.UserCallback)
                cmd.UserCallback(cmd_list, &cmd);
            else
            {
                gleam::draw_command draw = {
                        .program  = data.pipeline,
                        .vertices = data.vao,
                        .call = { .indexed = true, },
                        .data = {{ .elements = {
                            .count  = cmd.ElemCount,
                            .offset = cmd.IdxOffset * sizeof(ImDrawIdx),
                            .vertex_offset = cmd.VtxOffset,
                            .type =
                                sizeof(ImDrawIdx) == 2
                                    ? semantic::TypeEnum::UShort
                                    : semantic::TypeEnum::UInt,
                        }}},
                    };

                auto ec = api.submit(
                    draw,
                    gfx::make_sampler_list(gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"Texture"sv},
                        data.shell_sampler,
                    }),
                    uniform_list,
                    gfx::view_state{
                        .scissor = Veci4(
                            cmd.ClipRect.x,
                            fb_height - cmd.ClipRect.w,
                            cmd.ClipRect.z - cmd.ClipRect.x,
                            cmd.ClipRect.w - cmd.ClipRect.y),
                    },
                    gfx::cull_state{
                        .front_face = true,
                    },
                    gfx::blend_state{});
                if(ec)
                {
                    auto [error, message] = ec.value();
                    Throw(undefined_behavior(message));
                }
            }
            data.shell_texture->m_handle.release();
        }
    }

    data.vertices->discard();
    data.elements->discard();
}

void ImGuiSystem::setup_graphics_data(Proxy& e)
{
    using namespace std::string_view_literals;

    DProfContext _(IM_API "Creating device data");

    constexpr std::string_view vertex_shader =
#if defined(GLEAM_USE_CORE) && GLEAM_MAX_VERSION >= 0x420
        "#version 420\n"
        "#extension GL_ARB_explicit_uniform_location : enable\n"
        "layout(location=1) uniform mat4 ProjMtx;\n"
        "layout(location=0) in vec2 Position;\n"
        "layout(location=1) in vec2 UV;\n"
        "layout(location=2) in vec4 Color;\n"
#elif defined(GLEAM_USE_CORE)
        "#version 330\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
#else
        "#version 300 es\n"
        "precision mediump float;\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
#endif
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    constexpr std::string_view vertex_shader_100
        = "#version 100\n"
          "precision mediump float;\n"
          "uniform mat4 ProjMtx;\n"
          "attribute vec2 Position;\n"
          "attribute vec2 UV;\n"
          "attribute vec4 Color;\n"
          "varying vec2 Frag_UV;\n"
          "varying vec4 Frag_Color;\n"
          "void main()\n"
          "{\n"
          "	Frag_UV = UV;\n"
          "	Frag_Color = Color;\n"
          "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
          "}\n";

    constexpr std::string_view fragment_shader =
#if defined(GLEAM_USE_CORE) && GLEAM_MAX_VERSION >= 0x420
        "#version 420\n"
        "#extension GL_ARB_explicit_uniform_location : enable\n"
        "layout(location=0) uniform sampler2D Texture;\n"
#elif defined(GLEAM_USE_CORE)
        "#version 330\n"
        "uniform sampler2D Texture;\n"
#else
        "#version 300 es\n"
        "precision mediump float;\n"
        "precision mediump sampler2D;\n"
        "uniform sampler2D Texture;\n"
#endif
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 OutColor;\n"
        "void main()\n"
        "{\n"
        "	OutColor = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    constexpr std::string_view fragment_shader_100
        = "#version 100\n"
          "precision mediump float;\n"
          "precision mediump sampler2D;\n"
          "uniform sampler2D Texture;\n"
          "varying vec2 Frag_UV;\n"
          "varying vec4 Frag_Color;\n"
          "void main()\n"
          "{\n"
          "	gl_FragColor = Frag_Color * texture2D( Texture, Frag_UV.st);\n"
          "}\n";

    m_gfx_data = std::unique_ptr<ImGuiGraphicsData, ImGuiGraphicsDataDeleter>(
        new ImGuiGraphicsData);

    auto& api  = e.subsystem<gleam::system>();
    auto& data = *m_gfx_data;

    [[maybe_unused]] auto a = api.debug().scope(IM_API "Creating device data");

    auto needs_v100 = api.api_version() == std::make_tuple(2, 0)
                      && api.api_type() == gleam::api_type_t::es;

    do
    {
        DProfContext _(IM_API "Compiling shaders");

        auto& pip = data.pipeline;

        pip = api.alloc_program();

        pip->add(
            gfx::program_t::stage_t::Vertex,
            api.alloc_shader(mem_chunk<byte_t const>::ofContainer(
                needs_v100 ? vertex_shader_100 : vertex_shader)));
        pip->add(
            gfx::program_t::stage_t::Fragment,
            api.alloc_shader(mem_chunk<byte_t const>::ofContainer(
                needs_v100 ? fragment_shader_100 : fragment_shader)));
        if(auto res = pip->compile(); res.has_error())
            cWarning("Shader compile error: {0}", res.error());
        else
            cDebug("Shader compile info: {0}", res.value());
    } while(false);

    do
    {
        using semantic::RSCA;
        struct ImVertexProxy
        {
            Vecf2                           position;
            Vecf2                           uv;
            typing::vectors::tvector<u8, 4> color;
        };
        DProfContext _(IM_API "Creating vertex array object");
        auto&        a = data.vao;

        a             = api.alloc_vertex_array();
        data.vertices = api.alloc_buffer(
            gleam::buffers::vertex,
            RSCA::WriteOnly | RSCA::Discard | RSCA::Streaming);
        data.elements = api.alloc_buffer(
            gleam::buffers::element,
            RSCA::WriteOnly | RSCA::Discard | RSCA::Streaming);

        data.vertices->alloc();
        data.elements->alloc();

        a->alloc();

        a->add(gleam::vertex_attribute::from_member(&ImVertexProxy::position));
        auto attr  = gleam::vertex_attribute::from_member(&ImVertexProxy::uv);
        attr.index = 1;
        a->add(attr);
        attr = gleam::vertex_attribute::from_member(
            &ImVertexProxy::color, gleam::vertex_float_type);
        attr.index = 2;
        a->add(attr);

        a->set_buffer(gfx::buffers::vertex, data.vertices, 0);
        a->set_buffer(gfx::buffers::element, data.elements);

        a->set_attribute_names({
            {"Position"sv, 0},
            {"UV"sv, 1},
            {"Color"sv, 2},
        });
    } while(false);

    {
        DProfContext          _(IM_API "Creating font atlas");
        [[maybe_unused]] auto __
            = api.debug().scope(IM_API "Create font atlas");

        // Build texture atlas
        ImGuiIO&       io = ImGui::GetIO();
        unsigned char* pixels;
        int            width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        using typing::PixCmp;
        using typing::pixels::BitFmt;
        auto pixelDataSize = Coffee::GetPixSize(
            BitFmt::UByte, PixCmp::RGBA, C_FCAST<u32>(width * height));

        auto surface_size = size_2d<i32>{width, height}.convert<u32>();

        data.font_atlas = api.alloc_texture(
            gleam::textures::d2,
            typing::pixels::PixDesc(typing::pixels::PixFmt::RGBA8),
            1);
        data.font_atlas->alloc(size_3d<u32>{surface_size.w, surface_size.h, 1});
        data.font_atlas->upload(
            mem_chunk<byte_t const>::ofBytes(pixels, pixelDataSize).view,
            Veci2(0, 0),
            size_2d<i32>{width, height});

        data.font_sampler = data.font_atlas->sampler();
        data.font_sampler->alloc();
        data.font_sampler->set_filtering(
            typing::Filtering::Linear, typing::Filtering::Linear);

        union
        {
            void* ptr;
            u32   handle;
        } tex_handle = {
            .handle = data.font_atlas->m_handle,
        };
        io.Fonts->SetTexID(tex_handle.ptr);

        data.shell_texture = api.alloc_texture(
            gleam::textures::d2,
            typing::pixels::PixDesc(typing::pixels::PixFmt::RGBA8),
            1);

        data.shell_sampler = data.shell_texture->sampler();
        data.shell_sampler->alloc();
        data.shell_sampler->set_filtering(
            typing::Filtering::Linear, typing::Filtering::Linear);
    }
}

void ImGuiSystem::free_graphics_data(Proxy& e)
{
    DProfContext _(IM_API "Invalidating device objects");
    if(!m_gfx_data)
        return;
    [[maybe_unused]] auto __ = e.subsystem<gleam::system>().debug().scope(
        IM_API "Invalidating device objects");
    m_gfx_data->vertices->dealloc();
    m_gfx_data->elements->dealloc();
    m_gfx_data->pipeline->dealloc();
    m_gfx_data->font_atlas->dealloc();
    m_gfx_data->font_sampler->dealloc();

    m_gfx_data.reset();
}

} // namespace imgui::detail
