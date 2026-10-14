#pragma once

#include "geometry.h"
#include "map.h"
#include "shader.h"
#include "shader_passes.h"

#include <array>
#include <optional>
#include <string_view>
#include <vector>

namespace blam::dimeter::material {

/*!
 * What a shad's postprocess slots mean. Compiled caches keep no parameter
 * names, so the template's pass remappings are matched against the pass
 * table: usages[i] names postprocess bitmap slot i, arguments[j] names
 * tiling slot j (Reclaimer's approach).
 */
struct resolved_t
{
    tag_t const*     bitmap{nullptr}; /*!< Slot standing in for the base map */
    std::string_view usage;           /*!< Which usage `bitmap` is */
    f32              scale_u{1.f};
    f32              scale_v{1.f};
    bool             resolved{false}; /*!< Named through the template */
    bool             alpha_test{false};
    bool water{false}; /*!< water_pool: bump + reflection + fresnel */
    /*! Template under shader_templates\transparent: never opaque, even
     * when its passes are unknown */
    bool             transparent{false};
    std::string_view blend_mode;

    std::vector<std::string_view>   usages;
    std::vector<std::string_view>   arguments;
    std::vector<tag_t const*>       slot_bitmaps; /*!< Per postprocess slot */
    std::vector<std::array<f32, 4>> tilings;      /*!< Per tiling slot */
    std::vector<std::string_view>   pass_names;
    std::vector<std::string_view>   unknown_passes;
    std::vector<std::array<u8, 4>>  pixel_constants; /*!< r, g, b, a */
    /*! The self-illumination pass's colour, white when there is none */
    std::array<u8, 4> illum_color{255, 255, 255, 255};
    /*! Per pass: the pixel constant slots its first implementation reads */
    std::vector<std::vector<u8>> pass_pixel_slots;

    struct layer_t
    {
        tag_t const* bitmap{nullptr};
        f32          scale_u{1.f};
        f32          scale_v{1.f};
    };

    /*! The bitmap and tiling of a usage, e.g. "detail_map" */
    inline std::optional<layer_t> layer(std::string_view usage) const
    {
        layer_t out;
        for(size_t i = 0; i < usages.size(); i++)
            if(usages[i] == usage)
            {
                out.bitmap = slot_bitmaps[i];
                break;
            }
        if(!out.bitmap)
            return std::nullopt;
        /* Either one (u, v) tiling, or two texture-transform rows named
         * <usage>_u / <usage>_v whose diagonal is the scale */
        bool found = false;
        for(size_t i = 0; i < arguments.size() && !found; i++)
            if(arguments[i] == usage)
            {
                /* Overlay details keep their scale in zw */
                auto const& t  = tilings[i];
                bool        zw = t[0] == 0.f && t[1] == 0.f;
                out.scale_u    = zw ? t[2] : t[0];
                out.scale_v    = zw ? t[3] : t[1];
                found          = true;
            }
        if(!found)
            for(size_t i = 0; i < arguments.size(); i++)
            {
                auto const& a = arguments[i];
                if(a.size() == usage.size() + 2 && a.starts_with(usage))
                {
                    if(a.ends_with("_u"))
                        out.scale_u = tilings[i][0];
                    else if(a.ends_with("_v"))
                        out.scale_v = tilings[i][1];
                }
            }
        if(out.scale_u == 0.f)
            out.scale_u = 1.f;
        if(out.scale_v == 0.f)
            out.scale_v = 1.f;
        return out;
    }
};

/*! Sky and effect shaders have no base map; their glow/cloud layer stands
 * in, and failing that the first slot */
inline constexpr auto base_map_candidates = std::to_array<std::string_view>(
    {"base_map", "self_illum_map", "cloud_map", "alpha_blend_map"});

template<typename V>
inline resolved_t resolve(
    map_container<V> const& map, geo::shad_header_t const& shad)
{
    resolved_t out;
    auto       props = shad.properties.data(map.magic());
    if(props.has_error() || props.value().empty())
        return out;
    auto bitmaps = props.value()[0].bitmaps.data(map.magic());
    auto tiling  = props.value()[0].tiling.data(map.magic());
    if(bitmaps.has_error() || tiling.has_error())
        return out;
    out.usages.resize(bitmaps.value().size());
    out.arguments.resize(tiling.value().size());
    for(auto const& b : bitmaps.value())
        out.slot_bitmaps.push_back(map.tag_at(b.bitmap.id));
    for(auto const& t : tiling.value())
        out.tilings.push_back(
            {from_le(t.x), from_le(t.y), from_le(t.z), from_le(t.w)});
    if(auto pc = props.value()[0].pixel_constants.data(map.magic());
       !pc.has_error())
        for(auto const& c : pc.value())
            out.pixel_constants.push_back({c.r, c.g, c.b, c.a});

    auto const* stem_tag = map.tag_at(shad.shader_template.tag_id);
    auto const* stem =
        stem_tag ? stem_tag->template data<stem::header_t>(map.magic())
                 : nullptr;
    if(stem_tag &&
       map.name_of(*stem_tag).find("\\transparent\\") != std::string_view::npos)
        out.transparent = true;
    auto stem_props = stem ? stem->properties.data(map.magic())
                           : result<Span<stem::properties_t const>, error_msg>(
                                 stl_types::failure("no template"sv));
    if(!stem_props.has_error() && !stem_props.value().empty())
    {
        auto const& tp     = stem_props.value()[0];
        auto        passes = tp.passes.data(map.magic());
        auto        impls  = tp.implementations.data(map.magic());
        auto        remaps = tp.remappings.data(map.magic());
        auto        apply  = [&](stem::block_range_t                     range,
                         std::array<std::string_view, 16> const& names,
                         std::vector<std::string_view>&          slots) {
            for(u32 i = 0; i < range.count() && i < 16; i++)
            {
                if(names[i].empty() || names[i].front() == '?')
                    continue;
                size_t ri = static_cast<size_t>(range.index()) + i;
                if(ri >= remaps.value().size())
                    break;
                u8 slot = remaps.value()[ri].block_index;
                if(slot < slots.size() && slots[slot].empty())
                    slots[slot] = names[i];
            }
        };
        if(!passes.has_error() && !impls.has_error() && !remaps.has_error())
            for(auto const& pass : passes.value())
            {
                auto const* spas = map.tag_at(pass.pass_id());
                if(!spas)
                    continue;
                auto name = map.name_of(*spas);
                if(auto slash = name.find_last_of('\\'); slash != name.npos)
                    name = name.substr(slash + 1);
                out.pass_names.push_back(name);
                out.pass_pixel_slots.emplace_back();
                if(auto range = pass.implementations();
                   range.count() && range.index() < impls.value().size() &&
                   !remaps.has_error())
                {
                    auto pr = impls.value()[range.index()].pixel_constants;
                    for(u32 i = 0; i < pr.count(); i++)
                    {
                        size_t ri = static_cast<size_t>(pr.index()) + i;
                        if(ri < remaps.value().size())
                            out.pass_pixel_slots.back().push_back(
                                remaps.value()[ri].block_index);
                    }
                }
                /* Cutouts: only passes built for it, the base map's alpha
                 * is a blend mask otherwise */
                if(name.find("alpha_test") != name.npos)
                    out.alpha_test = true;
                if(name == "water_pool")
                    out.water = true;
                auto const* def = passes::find(name);
                if(!def)
                {
                    out.unknown_passes.push_back(name);
                    continue;
                }
                if(!def->blend_mode.empty())
                {
                    out.blend_mode = def->blend_mode;
                    if(def->blend_mode == "alpha_blend")
                        out.alpha_test = true;
                }
                auto range = pass.implementations();
                if(range.count() == 0 || range.index() >= impls.value().size())
                    continue;
                auto const& impl = impls.value()[range.index()];
                apply(impl.bitmaps, def->bitmaps, out.usages);
                apply(impl.vertex_constants, def->constants, out.arguments);
            }
    }

    for(auto const& name : base_map_candidates)
    {
        for(size_t i = 0; i < out.usages.size() && out.usage.empty(); i++)
            if(out.usages[i] == name)
            {
                out.bitmap   = map.tag_at(bitmaps.value()[i].bitmap.id);
                out.usage    = name;
                out.resolved = true;
            }
        if(!out.usage.empty())
            break;
    }
    if(out.usage.empty() && !bitmaps.value().empty())
        out.bitmap = map.tag_at(bitmaps.value()[0].bitmap.id);
    for(size_t i = 0; i < out.pass_names.size(); i++)
        if(out.pass_names[i].find("self_illum") != out.pass_names[i].npos &&
           !out.pass_pixel_slots[i].empty() &&
           out.pass_pixel_slots[i][0] < out.pixel_constants.size())
        {
            out.illum_color = out.pixel_constants[out.pass_pixel_slots[i][0]];
            break;
        }
    for(size_t i = 0; i < out.arguments.size() && !out.usage.empty(); i++)
        if(out.arguments[i] == out.usage)
        {
            f32 su      = from_le(tiling.value()[i].x);
            f32 sv      = from_le(tiling.value()[i].y);
            out.scale_u = su != 0.f ? su : 1.f;
            out.scale_v = sv != 0.f ? sv : 1.f;
            break;
        }
    return out;
}

} // namespace blam::dimeter::material
