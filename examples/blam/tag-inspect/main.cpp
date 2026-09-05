/* Prints decoded tag data straight out of a .map, so shader questions can be
 * answered from the tag instead of from the rendered frame. */

#include "blam/volta/blam_antr.h"
#include "blam/volta/blam_tag_classes.h"
#include "peripherals/stl/enumerate.h"
#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_bsp_structures.h>
#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_scenario.h>
#include <blam/volta/blam_swizzle.h>
#include <blam/volta/blam_shaders.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_versions.h>
#include <coffee/application/application_start.h>
#include <coffee/core/coffee_args.h>
#include <coffee/core/debug/formatting.h>
#include <coffee/core/files/cfiles.h>
#include <cmath>
#include <map>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cxxopts.hpp>
#include <glm/gtc/quaternion.hpp>
#include <magic_enum/magic_enum.hpp>
#include <peripherals/libc/types.h>
#include <url/url.h>

using libc_types::f32;
using libc_types::i32;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf3;
using Coffee::Logging::cWarning;

namespace {

blam::map_ptr g_magic;
/* Pixel data is addressed by absolute file offset, unlike tag data */
blam::map_ptr g_raw_magic;
bool          g_dump_mirrors = false;
size_t        g_scan_window  = 0;
bool          g_dump_player  = false;
bool          g_dump_scenario = false;
bool          g_dump_bones    = false;
bool          g_channel_stats{false};
std::string   g_dump_prefix{};

/* Writes each byte position of a 32-bit image as its own greyscale PGM, so the
 * channel order can be settled by looking at which one carries the mask. */
void dump_planes(
    std::string const& name, semantic::Span<const libc_types::u8> px, u32 w, u32 h)
{
    for(u32 c = 0; c < 4; c++)
    {
        auto  path = fmt::format("{}_{}_byte{}.pgm", g_dump_prefix, name, c);
        auto* fp   = fopen(path.c_str(), "wb");
        if(!fp)
            continue;
        fprintf(fp, "P5\n%u %u\n255\n", w, h);
        for(size_t i = 0; i < static_cast<size_t>(w) * h; i++)
            fputc(px[i * 4 + c], fp);
        fclose(fp);
        printf("      wrote %s\n", path.c_str());
    }
}

std::string_view sv(std::string_view v)
{
    return v;
}

template<typename E>
std::string_view enum_name(E value)
{
    auto name = magic_enum::enum_name(value);
    return name.empty() ? sv("?") : name;
}

/* Enums are stored as flag words as often as they are stored as values;
 * print the number either way so an unnamed bit is still visible. */
template<typename E>
void print_enum(char const* label, E value)
{
    printf("%s=%.*s(%u) ",
           label,
           static_cast<int>(enum_name(value).size()),
           enum_name(value).data(),
           static_cast<u32>(value));
}

std::string name_of(blam::tagref_t const& ref)
{
    if(!ref.valid())
        return "<none>";
    return std::string(ref.to_name().to_string(g_magic));
}

void print_anim(char const* label, blam::shader::texture_property_anim const& a)
{
    printf("      %-4s src=%-6.*s fn=%-16.*s period=%g phase=%g scale=%g\n",
           label,
           static_cast<int>(enum_name(a.source).size()),
           enum_name(a.source).data(),
           static_cast<int>(enum_name(a.function).size()),
           enum_name(a.function).data(),
           a.period,
           a.phase,
           a.scale);
}

void print_anim(
    char const* label, blam::shader::simple_tex_property_anim const& a)
{
    printf("      %-4s src=%-6.*s fn=%-16.*s period=%g scale=%g\n",
           label,
           static_cast<int>(enum_name(a.source).size()),
           enum_name(a.source).data(),
           static_cast<int>(enum_name(a.function).size()),
           enum_name(a.function).data(),
           a.period,
           a.scale);
}

void print_bitm_ref(blam::shader::bitm_reference_t const& map)
{
    printf("      bitmap=%s uv_scale=(%g,%g) uv_offset=(%g,%g) rot=%g "
           "mip_bias=%g\n",
           name_of(map.map).c_str(),
           map.uv_scale.x,
           map.uv_scale.y,
           map.uv_offset.x,
           map.uv_offset.y,
           map.rotation,
           map.mip_bias);
}

void print_detail_map(char const* label, blam::shader::detail_map const& map)
{
    printf("    %-10s scale=%g bitmap=%s\n",
           label,
           map.scale,
           name_of(map.map).c_str());
}

/* ---- shader_transparent (sotr) ---- */

void dump_sotr(blam::shader::shader_transparent const* info)
{
    using namespace blam::shader;

    printf("  transparent: ");
    print_enum("blend", info->transparent.blend_function);
    print_enum("fade_mode", info->transparent.fade_mode);
    print_enum("fade_src", info->transparent.fade_src);
    print_enum("first_map_type", info->transparent.first_map_type);
    print_enum("flags", info->transparent.flags);
    printf("\n");
    printf("  lens_flare=%s layers=%u\n",
           name_of(info->lens_flares.lens_flare).c_str(),
           info->layers.size());

    if(auto layers = info->layers.data(g_magic); layers.has_value())
        for(auto const& layer : layers.value())
            printf("    layer: %s\n", name_of(layer).c_str());

    auto maps = info->maps.data(g_magic);
    printf("  maps=%u\n", maps.has_value() ? maps.value().size() : 0u);
    if(maps.has_value())
    {
        u32 i = 0;
        for(auto const& map : maps.value())
        {
            printf("    map %u: ", i++);
            print_enum("flags", map.flags);
            printf("\n");
            print_bitm_ref(map.map);
            print_anim("u", map.animation.u);
            print_anim("v", map.animation.v);
            print_anim("rot", map.animation.rot);
            printf("      rot_center=(%g,%g)\n",
                   map.animation.rotation_center.x,
                   map.animation.rotation_center.y);
        }
    }

    auto stages = info->stages.data(g_magic);
    printf("  stages=%u\n", stages.has_value() ? stages.value().size() : 0u);
    if(!stages.has_value())
        return;

    auto cmap = [](shader_transparent::input_t in,
                   shader_transparent::mapping_t m) {
        printf("%.*s/%.*s ",
               static_cast<int>(enum_name(in).size()),
               enum_name(in).data(),
               static_cast<int>(enum_name(m).size()),
               enum_name(m).data());
    };
    auto amap = [](color_input in, shader_transparent::mapping_t m) {
        printf("%.*s/%.*s ",
               static_cast<int>(enum_name(in).size()),
               enum_name(in).data(),
               static_cast<int>(enum_name(m).size()),
               enum_name(m).data());
    };

    u32 i = 0;
    for(auto const& s : stages.value())
    {
        printf("    stage %u: ", i++);
        print_enum("flags", s.flags);
        print_enum("color0_src", s.color0_source);
        print_enum("color0_fn", s.color0_func);
        printf("period=%g\n", s.color0_period);
        printf("      color0_lower=(%g,%g,%g,%g) color0_upper=(%g,%g,%g,%g)\n",
               s.color0_lower.x,
               s.color0_lower.y,
               s.color0_lower.z,
               s.color0_lower.w,
               s.color0_upper.x,
               s.color0_upper.y,
               s.color0_upper.z,
               s.color0_upper.w);
        printf("      color1=(%g,%g,%g,%g)\n",
               s.color1.x,
               s.color1.y,
               s.color1.z,
               s.color1.w);
        printf("      color in:  ");
        cmap(s.color.a_input, s.color.a_mapping);
        cmap(s.color.b_input, s.color.b_mapping);
        cmap(s.color.c_input, s.color.c_mapping);
        cmap(s.color.d_input, s.color.d_mapping);
        printf("\n      color out: ");
        print_enum("ab", s.color.ab_output);
        print_enum("ab_fn", s.color.ab_out_func);
        print_enum("cd", s.color.cd_output);
        print_enum("cd_fn", s.color.cd_out_func);
        print_enum("sum", s.color.ab_cd_mux_sum);
        print_enum("map", s.color.output_map);
        printf("\n      alpha in:  ");
        amap(s.alpha.a_input, s.alpha.a_mapping);
        amap(s.alpha.b_input, s.alpha.b_mapping);
        amap(s.alpha.c_input, s.alpha.c_mapping);
        amap(s.alpha.d_input, s.alpha.d_mapping);
        printf("\n      alpha out: ");
        print_enum("ab", s.alpha.ab_output);
        print_enum("cd", s.alpha.cd_output);
        print_enum("sum", s.alpha.ab_cd_mux_sum);
        print_enum("map", s.alpha.output_map);
        printf("\n");
    }
}

/* ---- chicago (schi / scex) ---- */

void dump_chicago_maps(blam::shader::chicago::map_t const& map, u32 idx)
{
    printf("    map %u: ", idx);
    print_enum("flags", map.flags);
    print_enum("color_func", map.color_func);
    print_enum("alpha_func", map.alpha_func);
    printf("\n");
    print_bitm_ref(map.map);
    print_anim("u", map.anim_2d.u);
    print_anim("v", map.anim_2d.v);
    print_anim("rot", map.anim_2d.rot);
}

template<typename T>
void dump_chicago_base(T const* info)
{
    printf("  transparent: ");
    print_enum("blend", info->transparent.blend_function);
    print_enum("fade_mode", info->transparent.fade_mode);
    print_enum("fade_src", info->transparent.fade_src);
    print_enum("first_map_type", info->transparent.first_map_type);
    print_enum("flags", info->transparent.flags);
    printf("\n");
}

/* ---- shader_env (senv) ---- */

void dump_senv(blam::shader::shader_env const* info)
{
    print_enum("  flags", info->flags);
    print_enum("type", info->shader_type);
    printf("\n");
    printf("    base       bitmap=%s ",
           name_of(info->diffuse.base).c_str());
    print_enum("diffuse_flags", info->diffuse.flags);
    printf("\n");
    print_enum("    detail_function", info->diffuse.detail_function);
    printf("\n");
    print_detail_map("primary", info->diffuse.primary);
    print_detail_map("secondary", info->diffuse.secondary);
    print_enum("    micro_function", info->diffuse.micro_function);
    printf("\n");
    print_detail_map("micro", info->diffuse.micro);
    printf("    material_color=(%g,%g,%g)\n",
           info->diffuse.material_color.x,
           info->diffuse.material_color.y,
           info->diffuse.material_color.z);
    print_detail_map("bump", info->bump);
    printf("    scrolling:\n");
    print_anim("u", info->scrolling.u);
    print_anim("v", info->scrolling.v);
    print_enum("    self_illum flags", info->self_illum.flags);
    printf("map=%s\n", name_of(info->self_illum.map.map).c_str());
    auto illum = [](char const* label,
                    blam::shader::shader_env::illumination_props const& p) {
        printf("      %-10s on=(%g,%g,%g) off=(%g,%g,%g)\n",
               label,
               p.color_on.x,
               p.color_on.y,
               p.color_on.z,
               p.color_off.x,
               p.color_off.y,
               p.color_off.z);
        print_anim("anim", p.anim);
    };
    illum("primary", info->self_illum.primary);
    illum("secondary", info->self_illum.secondary);
    illum("plasma", info->self_illum.plasma);
    print_enum("    reflection flags", info->reflection.flags);
    printf(
        "type=%.*s perp=%g parallel=%g lightmap=%g cube=%s\n",
        static_cast<int>(magic_enum::enum_name(info->reflection.type).size()),
        magic_enum::enum_name(info->reflection.type).data(),
        info->reflection.perpendicular_brightness,
        info->reflection.parallel_brightness,
        info->reflection.lightmap_brightness,
        name_of(info->reflection.reflection).c_str());
}

/* ---- shader_model (soso) ---- */

/* The multipurpose map's channels carry four separate masks. The field comment
 * on shader_model::maps_t::multipurpose is the authority on which is which:
 * R = base color, G = illumination, B = change color, A = reflection. */
void dump_soso(blam::shader::shader_model const* info)
{
    print_enum("  flags", info->flags);
    printf(" translucency=%g", info->translucency);
    print_enum(" change_color_src", info->change_color_src);
    printf("\n");
    printf("    maps scale=(%g,%g)\n",
           info->maps.scale.x,
           info->maps.scale.y);
    printf("      base         = %s\n", name_of(info->maps.base).c_str());
    printf("      multipurpose = %s\n",
           name_of(info->maps.multipurpose).c_str());
    printf("        (R=base_color G=illumination B=change_color A=reflection)\n");
    print_enum("      detail function", info->maps.detail.function);
    print_enum(" mask", info->maps.detail.mask);
    printf(" scale=%g v_scale=%g map=%s\n",
           info->maps.detail.scale,
           info->maps.detail.v_scale,
           name_of(info->maps.detail.map).c_str());
    printf("    reflection falloff=%g cutoff=%g\n",
           info->reflection.falloff_distance,
           info->reflection.cutoff_distance);
    printf("      perpendicular brightness=%g tint=(%g,%g,%g)\n",
           info->reflection.perpendicular_brightness,
           info->reflection.perpendicular_tint.x,
           info->reflection.perpendicular_tint.y,
           info->reflection.perpendicular_tint.z);
    printf("      parallel      brightness=%g tint=(%g,%g,%g)\n",
           info->reflection.parallel_brightness,
           info->reflection.parallel_tint.x,
           info->reflection.parallel_tint.y,
           info->reflection.parallel_tint.z);
    printf("      cube         = %s\n",
           name_of(info->reflection.reflection).c_str());
}

/* ---- obje / unit ---- */

/* Bind pose of a model's bone tree, in the convention caching.cpp builds
 * inv_bind with: world = parent_world * translate(t) * mat4_cast(conj(q)).
 * The stored quaternion is the conjugate of what GLM's forward kinematics
 * wants, so it is conjugated here too and both are printed.
 *
 * "axes" are the bone's local X/Y/Z expressed in model space. That is what a
 * retargeting mapping has to be written against: a source rotation in model
 * space becomes bone-local as conj(world) * q * world. */
template<typename Ver>
void dump_bones(blam::mod2::header<Ver> const* header)
{
    auto bones_opt = header->bones.data(g_magic);
    if(!bones_opt.has_value())
    {
        printf("  (no bone data)\n");
        return;
    }
    auto bones = bones_opt.value();
    u32  n     = static_cast<u32>(bones.size());
    printf("  bones=%u\n", n);

    std::vector<Quatf> world_rot(n, Quatf(1.f, 0.f, 0.f, 0.f));
    std::vector<Vecf3> world_pos(n, Vecf3(0.f));

    for(u32 i = 0; i < n; i++)
    {
        auto const& b   = bones[i];
        Quatf       br  = glm::conjugate(b.rotation);
        u16         par = b.parent;
        if(par != blam::mod2::bone::invalid_bone && par < i)
        {
            world_rot[i] = world_rot[par] * br;
            world_pos[i] = world_pos[par] + world_rot[par] * b.translation;
        } else
        {
            world_rot[i] = br;
            world_pos[i] = b.translation;
        }

        auto const& w    = world_rot[i];
        auto        axes = glm::mat3_cast(w);
        auto        name = b.name.str();
        auto        idx  = [](u16 v) {
            return v == blam::mod2::bone::invalid_bone ? -1 : static_cast<i32>(v);
        };

        printf("  [%3u] %-26.*s parent=%-4d next=%-4d child=%d\n",
               i,
               static_cast<int>(name.size()),
               name.data(),
               idx(par),
               idx(b.next_bone),
               idx(b.next_child));
        printf("        local t=(%8.4f,%8.4f,%8.4f) q=(%7.4f,%7.4f,%7.4f,%7.4f)\n",
               b.translation.x, b.translation.y, b.translation.z,
               b.rotation.x, b.rotation.y, b.rotation.z, b.rotation.w);
        printf("        world t=(%8.4f,%8.4f,%8.4f) q=(%7.4f,%7.4f,%7.4f,%7.4f)\n",
               world_pos[i].x, world_pos[i].y, world_pos[i].z,
               w.x, w.y, w.z, w.w);
        printf("        axes  X=(%6.3f,%6.3f,%6.3f) "
               "Y=(%6.3f,%6.3f,%6.3f) Z=(%6.3f,%6.3f,%6.3f)\n",
               axes[0].x, axes[0].y, axes[0].z,
               axes[1].x, axes[1].y, axes[1].z,
               axes[2].x, axes[2].y, axes[2].z);
    }
}

void dump_object(blam::scn::object const* obj)
{
    print_enum("  type", obj->type);
    print_enum("flags", obj->flags);
    printf(
        "bound_radius=%g render_bound=%g accel_scale=%g\n",
        obj->bound_radius,
        obj->render_bound_radius,
        obj->acceleration_scale);
    printf(
        "    bound_offset=(%g,%g,%g) origin_offset=(%g,%g,%g)\n",
        obj->bound_offset.x,
        obj->bound_offset.y,
        obj->bound_offset.z,
        obj->origin_offset.x,
        obj->origin_offset.y,
        obj->origin_offset.z);
    printf("    model      =%s\n", name_of(obj->model).c_str());
    printf("    anim_graph =%s\n", name_of(obj->anim_graph).c_str());
    printf("    collider   =%s\n", name_of(obj->collider).c_str());
    printf("    physics    =%s\n", name_of(obj->physics).c_str());
    printf("    shader     =%s\n", name_of(obj->shader).c_str());
    printf("    effect     =%s\n", name_of(obj->effect).c_str());
    printf(
        "    hud_msg=%d shader_perm=%d\n",
        obj->export_.hud_msg,
        obj->export_.shader_perm);
}

void dump_unit(blam::scn::unit const* unit)
{
    dump_object(unit);

    auto colors = unit->change_colors.data(g_magic);
    if(colors.has_error())
    {
        printf("    change_colors: <unreadable>\n");
        return;
    }
    printf("    change_colors: %zu\n", colors.value().size());

    u32 idx = 0;
    for(auto const& c : colors.value())
    {
        printf("      [%u] ", idx++);
        print_enum("darken_by", c.darken_by);
        print_enum("scale_by", c.scale_by);
        print_enum("flags", c.scale_flags);
        printf("\n");
        printf(
            "          lower=(%g,%g,%g) upper=(%g,%g,%g)\n",
            c.lower_bound.x,
            c.lower_bound.y,
            c.lower_bound.z,
            c.upper_bound.x,
            c.upper_bound.y,
            c.upper_bound.z);

        auto perms = c.permutations.data(g_magic);
        if(perms.has_error())
        {
            printf("          permutations: <unreadable>\n");
            continue;
        }
        for(auto const& p : perms.value())
            printf(
                "          perm weight=%g lower=(%g,%g,%g) upper=(%g,%g,%g)\n",
                p.weight,
                p.lower_bound.x,
                p.lower_bound.y,
                p.lower_bound.z,
                p.upper_bound.x,
                p.upper_bound.y,
                p.upper_bound.z);
    }
}

/* ---- bitm ---- */

void dump_bitm(blam::bitm::header_t const* header, std::string_view name)
{
    print_enum("  type", header->type);
    print_enum("format", header->format);
    print_enum("usage", header->usage);
    print_enum("sprite_usage", header->sprite_usage);
    print_enum("budget", header->budget);
    printf("sprite_spacing=%u mipmaps=%u import=%ux%u\n",
           header->sprite_spacing,
           header->mipmap_count,
           header->import_width,
           header->import_height);

    auto seqs = header->sequences.data(g_magic);
    printf("  sequences=%u\n", seqs.has_value() ? seqs.value().size() : 0u);
    if(seqs.has_value())
    {
        u32 i = 0;
        for(auto const& seq : seqs.value())
        {
            auto sprites = seq.sprites.data(g_magic);
            auto name    = seq.name.str();
            printf("    seq %u: \"%.*s\" first=%u count=%u sprites=%u\n",
                   i++,
                   static_cast<int>(name.size()),
                   name.data(),
                   seq.first_bitmap,
                   seq.bitmap_count,
                   sprites.has_value() ? sprites.value().size() : 0u);
            if(!sprites.has_value())
                continue;
            for(auto const& spr : sprites.value())
                printf("      sprite: bitmap=%u l=%g r=%g t=%g b=%g reg=(%g,%g)\n",
                       spr.bitmap_index,
                       spr.left,
                       spr.right,
                       spr.top,
                       spr.bottom,
                       spr.registration_point.x,
                       spr.registration_point.y);
        }
    }

    auto images = header->images.data(g_magic);
    printf("  images=%u\n", images.has_value() ? images.value().size() : 0u);
    if(!images.has_value())
        return;
    if(g_channel_stats)
        for(auto const& img : images.value())
        {
            if(img.format != blam::bitm::format_t::ARGB8 &&
               img.format != blam::bitm::format_t::XRGB8)
                continue;
            /* image_t::data() is what the engine itself uses; a hand-rolled
             * read of img.offset does not land in the same place. */
            auto data = img.data(g_raw_magic, 0);
            printf("    data(): %zu bytes (expected %u)\n",
                   data.size(),
                   img.isize.x * img.isize.y * 4u);
            /* Byte order is what is in question, so report per byte position;
             * Morton swizzling permutes pixels, never channels, so these are
             * exact whether or not the image is swizzled. */
            u32 lo[4] = {255, 255, 255, 255}, hi[4] = {0, 0, 0, 0};
            libc_types::u64 sum[4] = {0, 0, 0, 0};
            size_t          px     = data.size() / 4;
            for(size_t i = 0; i < px; i++)
                for(u32 c = 0; c < 4; c++)
                {
                    u32 v  = data[i * 4 + c];
                    lo[c]  = std::min(lo[c], v);
                    hi[c]  = std::max(hi[c], v);
                    sum[c] += v;
                }
            if(!g_dump_prefix.empty())
            {
                /* Xbox stores these Morton-swizzled; linearise first or the
                 * plane images are unreadable. */
                std::vector<libc_types::u8> linear(data.size());
                auto view = data;
                if(static_cast<u16>(img.flags) &
                   static_cast<u16>(blam::bitm::flags_t::swizzled))
                    if(blam::swizzle::deswizzle_bytes(
                           data,
                           semantic::Span<libc_types::u8>(
                               linear.data(), linear.size()),
                           static_cast<u32>(img.isize.x),
                           static_cast<u32>(img.isize.y),
                           4u))
                        view = semantic::Span<const libc_types::u8>(
                            linear.data(), linear.size());
                auto short_name = std::string(
                    name.substr(name.find_last_of('\\') + 1));
                for(auto& ch : short_name)
                    if(ch == ' ')
                        ch = '_';
                dump_planes(
                    short_name,
                    view,
                    static_cast<u32>(img.isize.x),
                    static_cast<u32>(img.isize.y));
            }
            printf("    byte stats over %zu px:\n", px);
            for(u32 c = 0; c < 4; c++)
                printf("      byte %u: min=%3u max=%3u mean=%6.1f\n",
                       c,
                       lo[c],
                       hi[c],
                       px ? static_cast<double>(sum[c]) / px : 0.0);
        }
    u32 i = 0;
    for(auto const& img : images.value())
    {
        printf("    image %u: %ix%ix%i ", i++, img.isize.x, img.isize.y, img.depth);
        print_enum("type", img.type);
        print_enum("format", img.format);
        print_enum("flags", img.flags);
        printf("mips=%u offset=0x%x\n", img.mipmaps, img.offset);
    }
}

void dump_antr(blam::antr::header const* animation)
{
    printf("  objects=%u\n", animation->objects.count);
    printf("  units=%u\n", animation->units.count);
    printf("  weapons=%u\n", animation->weapons.count);
    printf("  vehicles=%u\n", animation->vehicles.count);
    printf("  devices=%u\n", animation->devices.count);
    printf("  nodes=%u\n", animation->nodes.count);
    for(auto [i, node] : stl_types::enumerate(animation->nodes.data(g_magic).value()))
    {
        auto joint = magic_enum::enum_name(node.joint_flags);
        printf("    node %zu: name=%.*s parent=%hi joint=%.*s\n",
            i,
            static_cast<int>(node.name.size), node.name.data.data(),
            node.parent,
            static_cast<int>(joint.size()), joint.data());
    }
    printf("  animations=%u\n", animation->animations.count);
    for(auto [i, anim] : stl_types::enumerate(animation->animations.data(g_magic).value()))
    {
        auto anim_type = magic_enum::enum_name(anim.type);
        printf("    animation %zu: name=%.*s type=%.*s\n",
            i,
            static_cast<int>(anim.name.size), anim.name.data.data(),
            static_cast<int>(anim_type.size()), anim_type.data());
    }
}

template<typename Ver>
void dump_mode(blam::mod2::header<Ver> const* info)
{
    if(auto markers = info->markers.data(g_magic); markers.has_value())
    {
        printf("  markers=%zu\n", markers.value().size());
        for(auto [i, marker] : stl_types::enumerate(markers.value()))
        {
            printf("    marker %zu: name=%.*s\n",
                i,
                static_cast<int>(marker.name.size), marker.name.data.data());
        }
    } 
    if(auto regions = info->regions.data(g_magic); regions.has_value())
    {
        printf("  regions=%zu\n", regions.value().size());
        for(auto [i, region] : stl_types::enumerate(regions.value()))
        {
            printf("    region %zu: name=%.*s\n",
                i,
                static_cast<int>(region.name.size), region.name.data.data());
        }
    }
    /* Per-part vertex_type: model_at() silently drops any part that is not
     * mod2_uncompressed(4) or mod2_compressed(5), so a part listed here with
     * another type is geometry the renderer never sees. */
    if(auto geoms = info->geometries.data(g_magic); geoms.has_value())
    {
        printf("  geometries=%zu\n", geoms.value().size());
        for(auto [gi, geom] : stl_types::enumerate(geoms.value()))
        {
            auto parts = geom.meshes(g_magic);
            printf("    geometry %zu: parts=%zu\n", gi, parts.size());
            for(auto [pi, part] : stl_types::enumerate(parts))
            {
                auto vt = blam::from_le(part.data.vertex_type);
                printf(
                    "      part %zu: vertex_type=%u%s shader_idx=%u\n",
                    pi,
                    static_cast<unsigned>(vt),
                    (vt == blam::vert::vertex_type_t::mod2_compressed_vertex
                     || vt == blam::vert::vertex_type_t::mod2_uncompressed_vertex)
                        ? ""
                        : "  <-- DROPPED by model_at",
                    static_cast<unsigned>(blam::from_le(part.data.shader_idx)));
            }
        }
    }
}

/* ---- dispatch ---- */

/* The scenario never names the globals tag; it supplies where a player spawns
 * and with what, while globals supplies the unit itself. Printed together so
 * the split is visible. */
template<typename Ver>
void dump_scenario(blam::map_container<Ver> const& map)
{
    auto scn = map.scenario();
    if(!scn.has_value())
    {
        printf("no scenario tag\n");
        return;
    }
    auto const* s = scn.value();

    print_enum("scenario type", s->info.type);
    print_enum("flags", s->info.flags);
    printf("\n");

    if(auto p = s->player_start.profiles.data(g_magic); p.has_value())
    {
        printf("player_starting_profiles: %zu\n", p.value().size());
        for(auto const& prof : p.value())
            printf(
                "  %-16.*s health=%g shield=%g  %s (%u) / %s (%u)  nades f=%u p=%u\n",
                static_cast<int>(prof.name.str().size()),
                prof.name.str().data(),
                prof.health_modifier,
                prof.shield_modifier,
                name_of(prof.primary_weapon).c_str(),
                prof.rounds1_total,
                name_of(prof.secondary_weapon).c_str(),
                prof.rounds2_total,
                prof.frag_grenades,
                prof.plasma_nades);
    }

    if(auto l = s->player_start.locations.data(g_magic); l.has_value())
    {
        printf("player_starting_locations: %zu\n", l.value().size());
        u32 shown = 0;
        for(auto const& loc : l.value())
        {
            if(shown++ >= 4)
            {
                printf("  ...\n");
                break;
            }
            printf(
                "  pos=(%.2f,%.2f,%.2f) rot=%.2f team=%u bsp=%u modes=",
                loc.pos.x,
                loc.pos.y,
                loc.pos.z,
                loc.rot,
                loc.team_index,
                loc.bsp_index);
            for(auto const& t : loc.types)
                printf(
                    "%.*s ",
                    static_cast<int>(enum_name(t).size()),
                    enum_name(t).data());
            printf("\n");
        }
    }

    if(auto f = s->netgame.flags.data(g_magic); f.has_value())
        printf("netgame flags: %zu\n", f.value().size());
    if(auto e = s->netgame.equipment.data(g_magic); e.has_value())
        printf("netgame equipment: %zu\n", e.value().size());
    if(auto q = s->starting_equipment.data(g_magic); q.has_value())
        printf("starting_equipment: %zu\n", q.value().size());
}

/* Resolves the player's spawn unit the way the engine would: globals -> the
 * multiplayer or singleplayer information block -> a bipd tag -> its model. */
template<typename Ver>
void dump_player_biped(
    blam::map_container<Ver> const& map, blam::tag_index_view<Ver> const& index)
{
    for(blam::tag_t const& tag : index)
    {
        if(!tag.valid() || !tag.matches(blam::tag_class_t::matg))
            continue;

        auto glob = tag.template data<blam::globals::globals>(g_magic);
        if(!glob.has_value())
        {
            printf("globals tag has no data\n");
            return;
        }

        auto report = [&](char const* label, blam::tagref_t const& unit) {
            auto uname = unit.to_name().to_string(g_magic);
            printf(
                "%s unit: %.*s [%.*s]\n",
                label,
                static_cast<int>(uname.size()),
                uname.data(),
                4,
                reinterpret_cast<const char*>(&unit.tag_class));

            auto it = index.find(unit);
            if(it == index.end())
            {
                printf("  (biped tag not in index)\n");
                return;
            }
            auto biped = it->template data<blam::scn::biped>(g_magic);
            if(!biped.has_value())
            {
                printf("  (biped tag has no data)\n");
                return;
            }
            dump_unit(biped.value());
        };

        if(auto mp = glob.value()->multiplayer.data(g_magic);
           mp.has_value() && !mp.value().empty())
            report("multiplayer", mp.value()[0].unit);
        else
            printf("multiplayer unit: <none>\n");

        if(auto sp = glob.value()->player.data(g_magic);
           sp.has_value() && !sp.value().empty())
            report("singleplayer", sp.value()[0].unit);
        else
            printf("singleplayer unit: <none>\n");
        return;
    }
    printf("no globals tag\n");
}

/* Walks a tag's bytes looking for embedded tagrefs, for tags whose layout is
 * not described anywhere. A hit must carry a tag id present in the index whose
 * class agrees with the reference -- strong enough that false positives are
 * rare. Looked up against a prebuilt map because find() throws on a malformed
 * tagref, which is exactly what scanning produces. */
template<typename Ver>
void scan_tagrefs(blam::tag_index_view<Ver> const& index, blam::tag_t const& tag)
{
    static std::map<u32, blam::tag_t const*> by_id;
    if(by_id.empty())
        for(blam::tag_t const& t : index)
            if(t.valid())
                by_id.emplace(t.tag_id, &t);

    auto data = tag.template data<u8>(g_magic);
    if(!data.has_value())
    {
        printf("  (no data)\n");
        return;
    }
    auto const* base = data.value();

    /* Reflexives first: a tagref found by the scan usually lives inside one of
     * these blocks, and the block's position in the tag header is what names
     * the field it came from. */
    for(size_t off = 0; off + 12 <= g_scan_window; off += 4)
    {
        const u32 count = blam::from_le(
            *reinterpret_cast<const u32*>(base + off));
        const u32 ptr = blam::from_le(
            *reinterpret_cast<const u32*>(base + off + 4));
        if(count == 0 || count > 256 || ptr <= g_magic.file_offset)
            continue;
        const size_t rel = ptr - g_magic.file_offset;
        if(rel >= g_magic.max_size)
            continue;
        auto const* target =
            reinterpret_cast<const u8*>(g_magic.base_ptr) + rel;
        const ptrdiff_t delta = target - base;
        if(delta < 0 || delta > 0x20000)
            continue;
        printf(
            "  REFLEX +0x%03zx  count=%-3u -> +0x%04tx\n", off, count, delta);
    }

    for(size_t off = 0; off + 16 <= g_scan_window; off += 4)
    {
        auto const* r = reinterpret_cast<blam::tagref_t const*>(base + off);
        if(!r->valid())
            continue;
        auto it = by_id.find(r->tag_id);
        if(it == by_id.end() || it->second->tag_class() != r->tag_class)
            continue;
        auto n = it->second->to_name().to_string(g_magic);
        if(n.empty())
            continue;
        printf(
            "  +0x%03zx  %.*s  %.*s\n",
            off,
            4,
            reinterpret_cast<const char*>(&r->tag_class),
            static_cast<int>(n.size()),
            n.data());
    }
}

template<typename Ver>
void dump_tag(blam::tag_index_view<Ver> const& index, blam::tag_t const& tag)
{
    using namespace blam::shader;

    auto name  = tag.to_name().to_string(g_magic);
    auto klass = tag.tagclass[0].str();
    printf("=== %.*s [%s] id=0x%x\n",
           static_cast<int>(name.size()),
           name.data(),
           klass.c_str(),
           tag.tag_id);

    auto header_of = [&](auto* type) {
        using T   = std::remove_const_t<std::remove_pointer_t<decltype(type)>>;
        auto data = tag.template data<T>(g_magic);
        return data.has_value() ? data.value() : nullptr;
    };

    if(g_scan_window > 0)
    {
        scan_tagrefs(index, tag);
        return;
    }

    switch(tag.tag_class())
    {
    case blam::tag_class_t::bipd:
        if(auto* info = header_of((blam::scn::biped*)nullptr))
            dump_unit(info);
        break;
    case blam::tag_class_t::vehi:
        if(auto* info = header_of((blam::scn::vehicle*)nullptr))
            dump_unit(info);
        break;
    /* Everything else derived from obje shares the object header; only bipeds
     * and vehicles are units, so the rest stop there. */
    case blam::tag_class_t::scen:
    case blam::tag_class_t::mach:
    case blam::tag_class_t::ctrl:
    case blam::tag_class_t::lifi:
    case blam::tag_class_t::ssce:
    case blam::tag_class_t::garb:
    case blam::tag_class_t::proj:
    case blam::tag_class_t::weap:
    case blam::tag_class_t::eqip:
        if(auto* info = header_of((blam::scn::object*)nullptr))
            dump_object(info);
        break;
    case blam::tag_class_t::sotr:
        if(auto* info = header_of((shader_transparent*)nullptr))
            dump_sotr(info);
        break;
    case blam::tag_class_t::senv:
        if(auto* info = header_of((shader_env*)nullptr))
            dump_senv(info);
        break;
    case blam::tag_class_t::soso:
        if(auto* info = header_of((shader_model*)nullptr))
            dump_soso(info);
        break;
    case blam::tag_class_t::schi:
        if(auto* info = header_of((shader_chicago<Ver>*)nullptr))
        {
            dump_chicago_base(info);
            auto maps = info->maps.data(g_magic);
            printf("  maps=%zu\n", maps.has_value() ? maps.value().size() : 0u);
            if(maps.has_value())
            {
                u32 i = 0;
                for(auto const& map : maps.value())
                    dump_chicago_maps(map, i++);
            }
        }
        break;
    case blam::tag_class_t::scex:
        if(auto* info = header_of((shader_chicago_extended<Ver>*)nullptr))
        {
            dump_chicago_base(info);
            auto maps = info->maps_4stage.data(g_magic);
            printf("  maps_4stage=%zu\n",
                   maps.has_value() ? maps.value().size() : 0u);
            if(maps.has_value())
            {
                u32 i = 0;
                for(auto const& map : maps.value())
                    dump_chicago_maps(map, i++);
            }
            auto maps2 = info->maps_2stage.data(g_magic);
            printf("  maps_2stage=%zu\n",
                   maps2.has_value() ? maps2.value().size() : 0u);
            if(maps2.has_value())
            {
                u32 i = 0;
                for(auto const& map : maps2.value())
                    dump_chicago_maps(map, i++);
            }
        }
        break;
    case blam::tag_class_t::mod2:
    case blam::tag_class_t::mode:
        if(auto* info = header_of((blam::mod2::header<Ver>*)nullptr))
        {
            dump_mode(info);
            if(g_dump_bones)
                dump_bones(info);
            else
                printf("  (pass --dump-bones for the bone tree)\n");
        }
        break;
    case blam::tag_class_t::bitm:
        if(auto res = index.template resource<blam::bitm::header_t>(tag.tag_id);
           res.has_value())
        {
            auto [_, header, magic] = *res;
            auto saved              = g_magic;
            g_magic                 = magic;
            dump_bitm(header, name);
            g_magic = saved;
        }
        break;
    case blam::tag_class_t::antr:
        if(auto* info = header_of((blam::antr::header*)nullptr))
        {
            dump_antr(info);
        }
        break;
    default:
        printf("  (no decoder for this class)\n");
        break;
    }
    printf("\n");
}

/* A mirror's shader is one of the shader tag classes; anything else at that
 * offset means the layout guess is wrong. */
inline bool is_shader_class(blam::tag_class_t cls)
{
    using c = blam::tag_class_t;
    switch(cls)
    {
    case c::senv:
    case c::soso:
    case c::schi:
    case c::scex:
    case c::sotr:
    case c::swat:
    case c::sgla:
    case c::smet:
    case c::spla:
    case c::shdr:
        return true;
    default:
        return false;
    }
}

/* The cluster mirror block's internal layout is unverified -- it may carry
 * padding between the plane and the shader reference. Score candidate
 * paddings on whether the shader resolves to a real tag, the vertex count is
 * sane, and the vertices actually lie on the mirror's own plane. */
template<typename Ver>
void dump_mirrors(
    blam::map_container<Ver> const& map, blam::tag_index_view<Ver> const& index)
{
    auto scn = map.scenario();
    if(!scn.has_value())
    {
        printf("no scenario tag\n");
        return;
    }

    printf(
        "sizeof(cluster)=%zu sizeof(mirror)=%zu (as declared)\n",
        sizeof(blam::bsp::cluster),
        sizeof(blam::bsp::mirror));

    auto bsps = scn.value()->bsp_info.data(map.magic);
    if(bsps.has_error())
    {
        printf("no bsp_info\n");
        return;
    }

    static const int kPads[] = {0, 4, 8, 12, 16, 20, 24, 28, 32};
    const size_t     kNPads  = sizeof(kPads) / sizeof(kPads[0]);
    int              pass[16] = {};
    int              total    = 0;

    u32 bsp_idx = 0;
    for(blam::bsp::info const& bsp : bsps.value())
    {
        auto bsp_magic = bsp.bsp_magic(map.magic);
        auto sec       = bsp.to_bsp(bsp_magic).to_header().data(
            bsp_magic, blam::single_value);
        if(sec.has_error())
        {
            bsp_idx++;
            continue;
        }
        auto const& section  = *sec.value();
        auto        clusters = section.clusters.data(bsp_magic);
        if(clusters.has_error())
        {
            bsp_idx++;
            continue;
        }

        u32 cluster_idx = 0;
        for(blam::bsp::cluster const& cluster : clusters.value())
        {
            u32 const count = blam::from_le(cluster.mirrors.count);
            if(count == 0 || count > 64)
            {
                if(count > 64)
                    printf(
                        "bsp %u cluster %u: implausible mirror count %u\n",
                        bsp_idx,
                        cluster_idx,
                        count);
                cluster_idx++;
                continue;
            }
            u32 const   off = blam::from_le(cluster.mirrors.offset);
            auto const* raw = reinterpret_cast<const u8*>(
                bsp_magic.base_ptr + off - bsp_magic.file_offset);

            printf(
                "bsp %u cluster %u: %u mirror(s) @ 0x%x\n",
                bsp_idx,
                cluster_idx,
                count,
                off);
            total += static_cast<int>(count);

            for(size_t p = 0; p < kNPads; p++)
            {
                const int    pad    = kPads[p];
                const size_t stride = 16 + pad + 16 + 12;
                for(u32 m = 0; m < count; m++)
                {
                    auto const* e = raw + m * stride;
                    f32         n[4];
                    memcpy(n, e, sizeof(n));
                    const f32 len =
                        std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
                    if(std::fabs(len - 1.f) > 1e-3f)
                        continue;

                    auto const* sh =
                        reinterpret_cast<blam::tagref_t const*>(e + 16 + pad);
                    if(!sh->valid() || !is_shader_class(sh->tag_class))
                        continue;

                    auto const* vr =
                        reinterpret_cast<blam::reference<typing::vector_types::Vecf3> const*>(
                            e + 16 + pad + 16);
                    const u32 vcount = blam::from_le(vr->count);
                    if(vcount < 3 || vcount > 256)
                        continue;
                    auto verts = vr->data(bsp_magic);
                    if(verts.has_error())
                        continue;

                    f32 resid = 0.f;
                    for(auto const& v : verts.value())
                        resid = std::max(
                            resid,
                            std::fabs(
                                n[0] * v[0] + n[1] * v[1] + n[2] * v[2] - n[3]));
                    if(resid >= 0.05f)
                        continue;
                    pass[p]++;

                    auto name = sh->to_name().to_string(map.magic);
                    printf(
                        "    [pad=%d] plane=(%.4f %.4f %.4f) d=%.4f "
                        "verts=%u resid=%.5f\n      shader=%.*s (%.*s)\n",
                        pad,
                        n[0],
                        n[1],
                        n[2],
                        n[3],
                        vcount,
                        resid,
                        static_cast<int>(name.size()),
                        name.data(),
                        4,
                        reinterpret_cast<const char*>(&sh->tag_class));
                }
            }
            cluster_idx++;
        }
        bsp_idx++;
    }

    printf("\ntotal mirrors found: %d\n", total);
    if(total == 0)
        return;
    printf("layout scan (mirrors passing all checks):\n");
    for(size_t p = 0; p < kNPads; p++)
        printf(
            "  pad=%2d stride=%2zu  passing=%d/%d\n",
            kPads[p],
            static_cast<size_t>(44 + kPads[p]),
            pass[p],
            total);
}

template<typename Ver>
void open_map(
    std::string const& path,
    std::string const& class_filter,
    std::string const& name_filter,
    bool               list_only)
{
    Coffee::Resource map_file(platform::url::constructors::MkUrl(path));
    if(!Coffee::FileMap(map_file))
    {
        cWarning("Could not open map file: {}", path);
        std::exit(1);
    }
    auto map_ = blam::map_container<Ver>::from_bytes(map_file, Ver());
    if(map_.has_error())
    {
        cWarning("Failed to open map: {}", magic_enum::enum_name(map_.error()));
        std::exit(1);
    }
    blam::map_container<Ver> map = std::move(map_.value());
    blam::tag_index_view<Ver> index(map);
    g_magic     = map.magic;
    /* Exactly how BitmapCache builds bitm_magic for Xbox: the map's own magic
     * with the file offset zeroed. Building one from the file start instead
     * lands on unrelated bytes. */
    g_raw_magic             = map.magic;
    g_raw_magic.file_offset = 0;

    if(g_dump_mirrors)
    {
        dump_mirrors<Ver>(map, index);
        return;
    }

    if(g_dump_player)
    {
        dump_player_biped<Ver>(map, index);
        return;
    }

    if(g_dump_scenario)
    {
        dump_scenario<Ver>(map);
        return;
    }

    u32 matched = 0;
    for(blam::tag_t const& tag : index)
    {
        if(!tag.valid())
            continue;
        auto klass = tag.tagclass[0].str();
        if(!class_filter.empty() && klass != class_filter)
            continue;
        auto name = tag.to_name().to_string(g_magic);
        if(!name_filter.empty() &&
           name.find(name_filter) == std::string_view::npos)
            continue;
        matched++;
        if(list_only)
        {
            printf("%s  0x%-8x  %.*s\n",
                   klass.c_str(),
                   tag.tag_id,
                   static_cast<int>(name.size()),
                   name.data());
            continue;
        }
        dump_tag(index, tag);
    }
    if(matched == 0)
        printf("no tags matched\n");
}

} // namespace

int inspect_main()
{
    cxxopts::Options options(
        "Blam Tag Inspect", "Dump decoded tag data from a Halo .map file");
    options.add_options("Help")("h,help", "Show this help info");
    options.positional_help("[map file]");

    options.add_options("Tag selection")
        //
        ("halo-version",
         "Version of Halo: pc xbox custom trial mcc",
         cxxopts::value<std::string>())
        //
        ("c,class", "Only tags of this class, e.g. sotr", cxxopts::value<std::string>())
        //
        ("n,name", "Only tags whose name contains this", cxxopts::value<std::string>())
        //
        ("l,list", "List matching tags instead of dumping them")
        //
        ("channel-stats", "For 32-bit bitmaps, report min/max/mean per byte position")
        //
        ("dump-planes", "Write each byte position of 32-bit bitmaps as a PGM with this path prefix", cxxopts::value<std::string>())
        //
        ("dump-mirrors", "Walk BSP clusters and report their mirror blocks")
        //
        ("scan-tagrefs", "Scan this many bytes of each tag for embedded tag references", cxxopts::value<int>())
        //
        ("dump-player-biped", "Resolve the player's spawn unit and its model from globals")
        //
        ("dump-scenario", "Print scenario type, starting profiles and spawn locations")
        //
        ("dump-bones", "For model tags, print the bone tree with bind-pose axes in model space")
        //
        ;

    auto& args      = Coffee::GetInitArgs();
    auto  arguments = options.parse(args.size(), args.data());
    if(Coffee::BaseArgParser::PerformDefaults(options, args) >= 0)
        return 0;

    if(arguments.unmatched().empty())
    {
        cWarning("Needs a map file, check --help");
        std::quick_exit(1);
    }

    std::string version =
        arguments.as_optional<std::string>("halo-version").value_or("pc");
    std::string class_filter =
        arguments.as_optional<std::string>("class").value_or("");
    std::string name_filter =
        arguments.as_optional<std::string>("name").value_or("");
    bool list_only  = arguments.count("list") > 0;
    g_dump_mirrors  = arguments.count("dump-mirrors") > 0;
    g_dump_player   = arguments.count("dump-player-biped") > 0;
    g_dump_scenario = arguments.count("dump-scenario") > 0;
    g_dump_bones    = arguments.count("dump-bones") > 0;
    g_scan_window   = static_cast<size_t>(
        arguments.as_optional<int>("scan-tagrefs").value_or(0));
    g_channel_stats = arguments.count("channel-stats") > 0;
    g_dump_prefix =
        arguments.as_optional<std::string>("dump-planes").value_or("");
    if(!g_dump_prefix.empty())
        g_channel_stats = true;

    auto const& path = arguments.unmatched().at(0);

    if(version == "pc")
        open_map<blam::pc_version_t>(path, class_filter, name_filter, list_only);
    else if(version == "xbox")
        open_map<blam::xbox_version_t>(
            path, class_filter, name_filter, list_only);
    else if(version == "custom")
        open_map<blam::custom_version_t>(
            path, class_filter, name_filter, list_only);
    else if(version == "trial")
        open_map<blam::trial_version_t>(
            path, class_filter, name_filter, list_only);
    else if(version == "mcc")
        open_map<blam::mcc_version_t>(
            path, class_filter, name_filter, list_only);
    else
    {
        cWarning("Unknown halo version: {}", version);
        std::quick_exit(1);
    }

    /* quick_exit skips stdio teardown, which would drop a short dump */
    fflush(stdout);
    std::quick_exit(0);
}

COFFEE_APPLICATION_MAIN_CUSTOM(inspect_main, 0x1 | 0x2)
