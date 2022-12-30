#pragma once

#include <algorithm>
#include <peripherals/concepts/graphics_api.h>
#include <peripherals/concepts/span.h>
#include <peripherals/stl/all_of.h>
#include <peripherals/stl/any_of.h>
#include <peripherals/stl/for_each.h>
#include <peripherals/stl/types.h>

#include "rhi_features.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

#include "rhi_buffer.h"
#include "rhi_context.h"
#include "rhi_debug.h"
#include "rhi_draw_command.h"
#include "rhi_program.h"
#include "rhi_query.h"
#include "rhi_rendertarget.h"
#include "rhi_texture.h"
#include "rhi_uniforms.h"
#include "rhi_vertex.h"

#include <coffee/comp_app/services.h>

#include <glw/extensions/EXT_multi_draw_arrays.h>
#include <glw/extensions/KHR_debug.h>

namespace gleam {
using stl_types::Optional;
using stl_types::String;
using stl_types::Tup;

struct api
{
    api() noexcept;
    api(api&&);
    api(api const&) = delete;

    using texture_type      = texture_t;
    using buffer_type       = buffer_t;
    using rendertarget_type = rendertarget_t;
    using program_type      = program_t;
    using vertex_type =
#if GLEAM_MAX_VERSION >= 0x150 || GLEAM_MAX_VERSION_ES >= 0x300 || \
    defined(GL_OES_vertex_array_object)
        vertex_array_t;
#else
        vertex_array_legacy_t;
#endif

    template<class T>
        requires Buffer<buffer_t, buffer_slice_t>
    inline auto alloc_buffer(T, semantic::RSCA access)
    {
        return MkShared<buffer_t>(m_features.buffer, T::value, access);
    }

    inline auto alloc_program()
    {
        return MkShared<program_t>(m_features.program, std::ref(*m_debug));
    }

    inline auto alloc_rendertarget()
    {
        return MkShared<rendertarget_t>(
            m_features.rendertarget, std::ref(m_rendertargetCurrency));
    }

    template<typename T>
    inline auto alloc_shader(
        T const& data, shader_t::constants_t const& constants = {})
    {
        return MkShared<shader_t>(std::ref(data), constants);
    }
    template<typename T>
    inline auto alloc_shader(
        T&& data, shader_t::constants_t const& constants = {})
    {
        return MkShared<shader_t>(std::move(data), constants);
    }

    template<class T>
        requires Texture<texture_2d_t>
    inline auto alloc_texture(
        T,
        typing::pixels::PixDesc data_type,
        u32                     mipmaps,
        textures::property      properties = textures::property::none)
    {
        using namespace semantic::concepts::graphics::textures;
        static_assert(
            stl_types::is_any_of_v<
                T,
                decltype(d2),
                decltype(d2_array),
                decltype(d3),
                decltype(cube_array)>,
            "invalid texture type");

        if constexpr(T::value == textures::type::d2)
        {
            return MkShared<texture_2d_t>(
                m_features.texture, T::value, data_type, mipmaps, properties);
        }
#if GLEAM_MAX_VERSION >= 0x120 || GLEAM_MAX_VERSION_ES >= 0x300
        else if constexpr(T::value == textures::type::d2_array)
        {
            return MkShared<texture_2da_t>(
                m_features.texture, T::value, data_type, mipmaps, properties);
        } else if constexpr(T::value == textures::type::d3)
        {
            return MkShared<texture_3d_t>(
                m_features.texture, T::value, data_type, mipmaps, properties);
        }
#if GLEAM_MAX_VERSION >= 0x410 || GLEAM_MAX_VERSION_ES >= 0x320
        else if constexpr(T::value == textures::type::cube_array)
        {
            return MkShared<texture_cube_array_t>(
                m_features.texture, T::value, data_type, mipmaps, properties);
        }
#endif
#endif
    }

    template<typename Dummy = void>
        requires VertexArray<vertex_type, buffer_type>
    inline auto alloc_vertex_array()
    {
        return MkShared<vertex_type>(
            std::ref(m_features.vertex), std::ref(*m_debug));
    }

    template<class T>
        requires
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        Query<query_t> &&
#endif
#if GLEAM_MAX_VERSION_ES >= 0x200 && GL_EXT_disjoint_timer_query
        Query<ext_disjoint_timer::query_t> &&
#endif
        Query<null_query_t>
    inline auto alloc_query(T type)
    {
        if constexpr(T::value == queries::type::fragments)
        {
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            return MkShared<query_t>(m_features.query, T::value);
#else
            return MkShared<null_query_t>(m_features.query, T::value);
#endif
        }
#if 0
        if constexpr(T::value == queries::type::time)
            return MkShared<query_t>(m_features.query, T::value);
        if constexpr(T::value == queries::type::timestamp)
            return MkShared<timestamp_query>(m_features.query, T::value);
#endif
        return nullptr;
    }

    template<class Dummy = void>
        requires RenderTarget<rendertarget_type, texture_type>
    inline auto default_rendertarget()
    {
        if(!m_framebuffer)
        {
            m_framebuffer = stl_types::MkShared<rendertarget_type>(
                m_features.rendertarget, std::ref(m_rendertargetCurrency));

            if constexpr(compile_info::debug_mode)
            {
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
                auto current_read = m_rendertargetCurrency.read;
                auto current_draw = m_rendertargetCurrency.draw;
#else
                auto current = m_rendertargetCurrency.draw;
#endif

                m_framebuffer->internal_bind(
                    group::framebuffer_target::framebuffer);
                auto status = static_cast<group::framebuffer_status>(
                    cmd::check_framebuffer_status(
                        group::framebuffer_target::framebuffer));
                if(status != group::framebuffer_status::framebuffer_complete)
                    Throw(undefined_behavior("invalid framebuffer"));

#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
                m_rendertargetCurrency.update(
                    current_read, group::framebuffer_target::read_framebuffer);
                m_rendertargetCurrency.update(
                    current_draw, group::framebuffer_target::draw_framebuffer);
                cmd::bind_framebuffer(
                    group::framebuffer_target::read_framebuffer, current_read);
                cmd::bind_framebuffer(
                    group::framebuffer_target::draw_framebuffer, current_draw);
#else
                m_rendertargetCurrency.update(
                    current, group::framebuffer_target::framebuffer);
                cmd::bind_framebuffer(
                    group::framebuffer_target::framebuffer, current);
#endif
            }
        }
        return m_framebuffer;
    }

    inline auto& debug()
    {
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
        if(!m_debug)
            m_debug = stl_types::MkUq<debug::api>();
        return *m_debug;
#else
        static debug::null_api nothing;
        return nothing;
#endif
    }

    inline auto& context_robustness()
    {
        return m_context_state;
    }

    inline const auto& feature_info() const
    {
        return m_features;
    }

    template<typename... UList>
    Optional<Tup<error, std::string_view>> submit(
        draw_command const& command, UList&&... uniforms);

    using extensions_set = stl_types::Set<String>;
    struct load_options_t
    {
        Optional<u32>            api_version;
        Optional<api_type_t>     api_type;
        Optional<extensions_set> api_extensions;
        Optional<features>       api_features;
        Optional<workarounds>    api_workarounds;
    };

    static Tup<features, api_type_t, u32> query_native_api_features(
        extensions_set const& extensions, load_options_t options = {});
    static api_type_t     query_native_api();
    static Tup<u32, u32>  query_native_version();
    static extensions_set query_native_extensions();

    STATICINLINE u32 version_tuple_to_u32(Tup<u32, u32> const& ver)
    {
        auto [major, minor] = ver;
        return ((major << 8) & 0xF00) | ((minor << 4) & 0x0F0);
    }

    std::string_view    api_name();
    api_type_t          api_type();
    Tup<u32, u32>       api_version();
    String              shaderlang_name();
    api_type_t          shaderlang_type();
    Tup<u32, u32>       shaderlang_version();
    Tup<String, String> device();
    Optional<String>    device_driver();

    void collect_info(comp_app::interfaces::AppInfo& appInfo);

    Optional<error> load(load_options_t options = {});
    void            unload();

  private:
    static bool supports_extension(
        stl_types::Set<String> const& extensions, String const& ext);
    bool supports_extension(String const& ext);

    stl_types::ShPtr<rendertarget_type> m_framebuffer;
    stl_types::UqPtr<
        std::conditional<debug::api_available, debug::api, debug::null_api>::
            type>
                 m_debug;
    context::api m_context_state;

    rendertarget_currency m_rendertargetCurrency;
    features              m_features;
    workarounds           m_workarounds;
    extensions_set        m_extensions;
    api_type_t            m_api_type{api_type_t::none};
    u32                   m_api_version{0};
};

namespace detail {

inline Optional<error> evaluate_draw_state(draw_command const& command)
{
    auto const& call = command.call;
    auto const& data = command.data;
    if(call.indexed)
    {
        i32 current_binding{0};
        cmd::get_integerv(
            group::get_prop::element_array_buffer_binding,
            SpanOne(current_binding));
        if(current_binding == 0)
            return error::draw_no_element_buffer;
        if(stl_types::any_of(
               data, [](auto const& d) { return d.elements.count == 0; }))
            return error::draw_no_elements;
    } else
    {
        if(stl_types::any_of(
               data, [](auto const& d) { return d.arrays.count == 0; }))
            return error::draw_no_arrays;
    }

    return std::nullopt;
}

inline constexpr auto unsupported_drawcall()
{
    using namespace std::string_view_literals;
    return std::make_tuple(
        error::no_implementation_for_draw_call,
        "draw call not implemented on this api"sv);
}

inline std::optional<std::tuple<error, std::string_view>> direct_draw(
    draw_command::call_spec_t const& call,
    draw_command::data_t const&      data,
    shader_bookkeeping_t&            bookkeeping)
{
    const auto base_instance = data.instances.offset != 0;
    const auto instanced     = call.instanced || data.instances.offset > 0 ||
                           (call.indexed && data.elements.vertex_offset > 0);

    if(call.indexed)
    {
        const auto base_vertex = data.elements.vertex_offset != 0;
        if(instanced)
        {
            bookkeeping.baseInstance = data.instances.offset;
#if GLEAM_MAX_VERSION_ES >= 0x300
            if(base_instance && base_vertex)
#if GLEAM_MAX_VERSION >= 0x420
                cmd::draw_elements_instanced_base_vertex_base_instance(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset,
                    data.instances.count,
                    data.elements.vertex_offset,
                    data.instances.offset);
#else
                return detail::unsupported_drawcall();
#endif
            else if(base_instance)
#if GLEAM_MAX_VERSION >= 0x420
                cmd::draw_elements_instanced_base_instance(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset,
                    data.instances.count,
                    data.instances.offset);
#else
                return detail::unsupported_drawcall();
#endif
            else if(base_vertex)
#if GLEAM_MAX_VERSION >= 0x320 || GLEAM_MAX_VERSION_ES >= 0x320
                cmd::draw_elements_instanced_base_vertex(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset,
                    data.instances.count,
                    data.elements.vertex_offset);
#else
                return detail::unsupported_drawcall();
#endif
            else
                cmd::draw_elements_instanced(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset,
                    data.instances.count);
            return detail::unsupported_drawcall();
#else
            for(auto i : stl_types::Range<>(data.instances.count))
            {
                cmd::draw_elements(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset);
                bookkeeping.instanceId++;
            }
#endif
        } else
            cmd::draw_elements(
                convert::to(call.mode),
                data.elements.count,
                convert::to<group::draw_elements_type>(data.elements.type),
                data.elements.offset);
    } else
    {
        if(instanced)
        {
            bookkeeping.baseInstance = data.instances.offset;
#if GLEAM_MAX_VERSION_ES != 0x200
            if(base_instance)
#if GLEAM_MAX_VERSION >= 0x420
                cmd::draw_arrays_instanced_base_instance(
                    convert::to(call.mode),
                    data.arrays.offset,
                    data.arrays.count,
                    data.instances.count,
                    data.instances.offset);
#else
                return detail::unsupported_drawcall();
#endif
            else
                cmd::draw_arrays_instanced(
                    convert::to(call.mode),
                    data.arrays.offset,
                    data.arrays.count,
                    data.instances.count);
#else
            for(auto i : stl_types::Range<>(data.instances.count))
            {
                cmd::draw_arrays(
                    convert::to(call.mode),
                    data.arrays.offset,
                    data.arrays.count);
                bookkeeping.instanceId++;
            }
#endif
        } else
            cmd::draw_arrays(
                convert::to(call.mode), data.arrays.offset, data.arrays.count);
    }
    return std::nullopt;
}

union alignas(4) indirect_command_buffer
{
    struct elements_indirect_t
    {
        u32 count;
        u32 instanceCount;
        u32 first;
        i32 baseVertex;
        u32 baseInstance;
    } elements;
    struct arrays_indirect_t
    {
        u32 count;
        u32 instanceCount;
        u32 first;
        u32 baseInstance;
    } arrays;
};

inline void create_draw(
    draw_command::call_spec_t const& call,
    draw_command::data_t const&      data,
    indirect_command_buffer*         buffer)
{
    if(call.indexed)
        buffer->elements = {
            .count         = data.elements.count,
            .instanceCount = data.instances.count,
            .first         = static_cast<u32>(data.elements.offset),
            .baseVertex    = static_cast<i32>(data.elements.vertex_offset),
            .baseInstance  = data.instances.offset,
        };
    else
        buffer->arrays = {
            .count         = data.arrays.count,
            .instanceCount = data.instances.offset,
            .first         = data.arrays.offset,
            .baseInstance  = data.instances.offset,
        };
}

inline void indirect_draw(
    draw_command::call_spec_t const& call, draw_command::data_t const& data)
{
#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION_ES >= 0x310
    indirect_command_buffer cmd{};
    create_draw(call, data, &cmd);
    if(call.indexed)
        cmd::draw_elements_indirect(
            convert::to(call.mode),
            convert::to<group::draw_elements_type>(data.elements.type),
            reinterpret_cast<uintptr_t>(&cmd.elements));
    else
        cmd::draw_arrays_indirect(
            convert::to(call.mode), reinterpret_cast<uintptr_t>(&cmd.arrays));
#endif
}

inline void multi_indirect_draw(
    draw_command::call_spec_t const&    call,
    decltype(draw_command::data) const& data)
{
#if GLEAM_MAX_VERSION >= 0x430
    std::vector<indirect_command_buffer> cmds;
    cmds.resize(data.size());
    size_t i = 0;
    stl_types::for_each(data, [&call, &cmds, &i](auto const& d) {
        create_draw(call, d, &cmds.at(i++));
    });

    // TODO: Push commands into circular GPU buffer

    if(call.indexed)
    {
        auto element_type = data.at(0).elements.type;
        cmd::multi_draw_elements_indirect(
            convert::to(call.mode),
            convert::to<group::draw_elements_type>(element_type),
            reinterpret_cast<uintptr_t>(cmds.data()),
            cmds.size(),
            sizeof(decltype(cmds)::value_type));
    } else
    {
        cmd::multi_draw_arrays_indirect(
            convert::to(call.mode),
            reinterpret_cast<uintptr_t>(cmds.data()),
            cmds.size(),
            sizeof(decltype(cmds)::value_type));
    }
#endif
}

} // namespace detail

template<typename... MList>
inline Optional<Tup<error, std::string_view>> api::submit(
    draw_command const& command, MList&&... modifiers)
{
    using namespace std::string_view_literals;

    auto const& call          = command.call;
    auto const& data          = command.data;
    auto        program       = command.program.lock();
    auto        render_target = command.render_target.expired()
                                    ? default_rendertarget()
                                    : command.render_target.lock();

    if(data.empty())
        return std::nullopt;

    auto _ = debug().scope(__PRETTY_FUNCTION__);

    render_target->internal_bind(
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        group::framebuffer_target::draw_framebuffer);
#else
        group::framebuffer_target::framebuffer);
#endif

    const bool uses_baseinstance = stl_types::any_of(
        data, [](auto const& d) { return d.instances.offset > 0; });
    const bool uses_vertex_offset =
        call.indexed && stl_types::any_of(data, [](auto const& d) {
            return d.elements.vertex_offset > 0;
        });
    if(m_api_type == api_type_t::es && uses_baseinstance &&
       !m_workarounds.draw.emulated_base_instance)
        return std::make_tuple(
            error::no_implementation_for_draw_call,
            "baseinstance not enabled in GL ES"sv);
    if(m_api_type == api_type_t::es && m_api_version == 0x200 &&
       call.instanced && !m_workarounds.draw.emulated_instance_id)
        return std::make_tuple(
            error::no_implementation_for_draw_call,
            "instancing not enabled in GL ES 2.0"sv);
    if(m_api_type == api_type_t::es && m_api_version <= 0x300 &&
       uses_vertex_offset && !m_workarounds.draw.emulated_vertex_offset)
        return std::make_tuple(
            error::no_implementation_for_draw_call,
            "base vertex not enabled in GL ES 3.0 and below"sv);

    if(program->m_handle == 0)
        return std::make_tuple(error::no_program, "no program provided");

    auto                      vao = command.vertices.lock();
    std::shared_ptr<buffer_t> element_buf;
#if GLEAM_MAX_VERSION_ES != 0x200
    if(m_features.vertex.vertex_arrays && m_features.vertex.attribute_binding)
    {
        if(m_features.vertex.layout_binding)
        {
            for(auto const& attrib : vao->m_attribute_names)
                cmd::bind_attrib_location(
                    program->m_handle, attrib.second, attrib.first);
            cmd::link_program(program->m_handle);
        }
        cmd::bind_vertex_array(vao->m_handle);
    } else
#endif
    {
        using buffer_target = group::buffer_target_arb;

        for(auto const& attrib : vao->m_attribute_names)
            cmd::bind_attrib_location(
                program->m_handle, attrib.second, attrib.first);
        for(auto const& attrib : vao->m_attributes)
        {
            cmd::enable_vertex_attrib_array(attrib.index);
            cmd::bind_buffer(
                buffer_target::array_buffer,
                vao->m_buffers.at(attrib.buffer.id).lock()->m_handle);
            detail::vertex_setup_attribute(attrib);
        }

        if(!vao->m_element_buffer.expired())
        {
            element_buf = vao->m_element_buffer.lock();
            cmd::bind_buffer(
                buffer_target::element_array_buffer, element_buf->m_handle);
        }
    }

    cmd::use_program(program->m_handle);

    {
        u32 vertex_program =
            m_features.program.separable_programs
                ? program->m_stages.at(program_t::stage_t::Vertex)->m_handle
                : program->m_handle;
        for(auto const& attrib : vao->m_attribute_names)
            cmd::bind_attrib_location(
                vertex_program, attrib.second, attrib.first);
    }

    detail::shader_bookkeeping_t bookkeeping{};
    (detail::apply_command_modifier(*program, bookkeeping, modifiers), ...);

    if(m_workarounds.draw.emulated_instance_id)
    {
        if constexpr(compile_info::debug_mode)
        {
            auto loc = detail::get_program_uniform_location(
                *program, program_t::stage_t::Vertex, {"g_BaseInstance"sv});
            if(loc == invalid_uniform)
                debug().message(
                    "uniform 'g_InstanceID' not located with emulated InstanceID enabled"sv);
        }
        auto instanceUniform = make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            uniform_pair{
                {"g_InstanceID"sv},
                SpanOne<const i32>(bookkeeping.instanceId),
            });
        detail::apply_command_modifier(*program, bookkeeping, instanceUniform);
    }
    if(m_workarounds.draw.emulated_base_instance)
    {
        if constexpr(compile_info::debug_mode)
        {
            auto loc = detail::get_program_uniform_location(
                *program, program_t::stage_t::Vertex, {"g_BaseInstance"sv});
            if(loc == invalid_uniform)
                debug().message(
                    "uniform 'g_BaseInstance' not located with emulated BaseInstance enabled"sv);
        }
        auto baseUniform = make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            uniform_pair{
                {"g_BaseInstance"sv},
                SpanOne<const i32>(bookkeeping.baseInstance),
            });
        detail::apply_command_modifier(*program, bookkeeping, baseUniform);
    }

    const bool indirect_supported = false;
    //        = m_api_type == api_type_t::core
    //              ? (uses_baseinstance ? m_api_version >= 0x420
    //                                   : m_api_version >= 0x400)
    //              : m_api_version >= 0x310;
    const bool has_uniform_element_type =
        call.indexed && stl_types::all_of(data, [&data](auto const& d) {
            return d.elements.type == data.at(0).elements.type;
        });
    const bool multi_indirect_supported = false;
    //        = m_api_type == api_type_t::core ? m_api_version >= 0x430 : false;

    if constexpr(compile_info::debug_mode)
    {
        if(multi_indirect_supported && call.indexed &&
           !has_uniform_element_type)
            debug().message(
                "varying element types prevents use of MultiDrawIndirect"sv);
    }

    if constexpr(compile_info::debug_mode)
    {
        auto log = detail::program_log(program->m_handle);
        if(auto error = detail::evaluate_draw_state(command); error.has_value())
            return std::make_tuple(*error, String());
    }

    if(multi_indirect_supported && (!call.indexed || has_uniform_element_type))
    {
        detail::multi_indirect_draw(call, data);
    } else if(indirect_supported)
    {
        for(auto const& d : data)
            detail::indirect_draw(call, d);
    } else
        for(auto const& d : data)
            detail::direct_draw(call, d, bookkeeping);

    if(!m_features.vertex.vertex_arrays || !m_features.vertex.attribute_binding)
        for(auto const& attrib : vao->m_attributes)
            cmd::disable_vertex_attrib_array(attrib.index);

    if(element_buf)
    {
        cmd::bind_buffer(
            group::buffer_target_arb::element_array_buffer,
            element_buf->m_handle);
    }

    (detail::undo_command_modifier(*program, bookkeeping, std::move(modifiers)),
     ...);

    return std::nullopt;
}

inline void test_t2d()
{
    api  ap;
    auto a = ap.alloc_texture(
        textures::d2,
        typing::pixels::PixDesc(typing::pixels::PixFmt::RGBA8),
        3);
    a->upload(null_span{}, Veci2{}, Veci2{}, 0);
    a->view(textures::d2, {}, Vecui3{}, size_2d<u32>());
}

inline void test_buffer()
{
    using semantic::RSCA;
    api  ap;
    auto a =
        ap.alloc_buffer(buffers::vertex, RSCA::ReadWrite | RSCA::Persistent);
}

inline void test_query()
{
    api  ap;
    auto a = ap.alloc_query(queries::time);
}

} // namespace gleam
