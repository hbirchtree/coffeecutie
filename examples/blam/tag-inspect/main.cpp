/* Prints decoded tag data straight out of a .map, so shader questions can be
 * answered from the tag instead of from the rendered frame. */

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_shaders.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_versions.h>
#include <coffee/application/application_start.h>
#include <coffee/core/coffee_args.h>
#include <coffee/core/debug/formatting.h>
#include <coffee/core/files/cfiles.h>
#include <cstdlib>
#include <cxxopts.hpp>
#include <magic_enum/magic_enum.hpp>
#include <peripherals/libc/types.h>
#include <url/url.h>

using libc_types::f32;
using libc_types::i32;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;
using Coffee::Logging::cWarning;

namespace {

blam::map_ptr g_magic;

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
}

/* ---- bitm ---- */

void dump_bitm(blam::bitm::header_t const* header)
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

/* ---- dispatch ---- */

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

    switch(tag.tag_class())
    {
    case blam::tag_class_t::sotr:
        if(auto* info = header_of((shader_transparent*)nullptr))
            dump_sotr(info);
        break;
    case blam::tag_class_t::senv:
        if(auto* info = header_of((shader_env*)nullptr))
            dump_senv(info);
        break;
    case blam::tag_class_t::schi:
        if(auto* info = header_of((shader_chicago<Ver>*)nullptr))
        {
            dump_chicago_base(info);
            auto maps = info->maps.data(g_magic);
            printf("  maps=%u\n", maps.has_value() ? maps.value().size() : 0u);
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
            printf("  maps_4stage=%u\n",
                   maps.has_value() ? maps.value().size() : 0u);
            if(maps.has_value())
            {
                u32 i = 0;
                for(auto const& map : maps.value())
                    dump_chicago_maps(map, i++);
            }
            auto maps2 = info->maps_2stage.data(g_magic);
            printf("  maps_2stage=%u\n",
                   maps2.has_value() ? maps2.value().size() : 0u);
            if(maps2.has_value())
            {
                u32 i = 0;
                for(auto const& map : maps2.value())
                    dump_chicago_maps(map, i++);
            }
        }
        break;
    case blam::tag_class_t::bitm:
        if(auto res = index.template resource<blam::bitm::header_t>(tag.tag_id);
           res.has_value())
        {
            auto [_, header, magic] = *res;
            auto saved              = g_magic;
            g_magic                 = magic;
            dump_bitm(header);
            g_magic = saved;
        }
        break;
    default:
        printf("  (no decoder for this class)\n");
        break;
    }
    printf("\n");
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
    g_magic = map.magic;

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
    bool list_only = arguments.count("list") > 0;

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
