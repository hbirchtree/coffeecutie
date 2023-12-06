#pragma once

#include <coffee/core/debug/formatting.h>
#include <coffee/core/files/cfiles.h>

#include "graphics_api.h"

struct shader_pair_t
{
    std::string_view                 vertex_file;
    std::string_view                 fragment_file;
    std::shared_ptr<gfx::program_t>& shader;
};

template<size_t N>
static void create_shaders(
    gfx::api& api, std::array<shader_pair_t, N>&& shaders)
{
    using namespace std::string_view_literals;
    using platform::url::constructors::MkUrl;
    using semantic::RSCA;

    auto version = api.api_version();
    if(api.api_type() == gfx::api_type_t::core)
    {
        /* Lower version to known versions */
        if(version == std::make_tuple(4u, 4u)
           || version == std::make_tuple(4u, 5u))
            version = std::make_tuple(4u, 3u);
        else if(version == std::make_tuple(4u, 2u))
            version = std::make_tuple(4u, 1u);
        else if(version == std::make_tuple(4u, 0u))
            version = std::make_tuple(3u, 3u);
    }

    std::string variant = fmt::format(
        "{}{}{}0",
        api.api_type() == gfx::api_type_t::core ? "core" : "es",
        std::get<0>(version),
        std::get<1>(version));

    if(variant.empty())
        Throw(std::runtime_error("no shader variant selected, good night"));
    else
        Coffee::cDebug("Selected shader variant {}", variant);

    for(shader_pair_t& shader : shaders)
    {
        using Coffee::Resource;
        if(shader.shader)
        {
            shader.shader->dealloc();
        }

        auto vertex_url = MkUrl(
            fmt::format("{0}.{1}.vert", shader.vertex_file, variant),
            RSCA::AssetFile);
        auto fragment_url = MkUrl(
            fmt::format("{0}.{1}.frag", shader.fragment_file, variant),
            RSCA::AssetFile);

        auto pipeline = shader.shader ? shader.shader : api.alloc_program();
        pipeline->add(
            gfx::program_t::stage_t::Vertex,
            api.alloc_shader(Resource(vertex_url).data()));
        pipeline->add(
            gfx::program_t::stage_t::Fragment,
            api.alloc_shader(Resource(fragment_url).data()));
        if(auto res = pipeline->compile(); res.has_error())
        {
            auto [msg] = res.error();
            Coffee::cFatal("Failed to compile shader: {0}", msg);
        }
        shader.shader = pipeline;
    }
}
