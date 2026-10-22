#pragma once

#include "h2_bitmap.h"
#include "h2_shell_data.h"
#include "h2_sound.h"

#include <blam/dimeter/font.h>
#include <blam/dimeter/scenario.h>
#include <blam/dimeter/sound.h>
#include <blam/dimeter/ui.h>
#include <coffee/core/CDebug>

#include <algorithm>
#include <map>
#include <string_view>

/* Decodes mainmenu.map's shell tags into H2ShellData */
namespace h2 {

struct ShellFontFile
{
    std::string        name;
    Span<const u8> bytes;
};

namespace detail {

template<typename V>
struct ShellLoader
{
    gfx::api&                    api;
    dim::map_container<V> const& map;
    dim::raw_pool const&         pool;
    H2ShellData&                 out;
    H2SoundBank&                 bank;
    std::map<u16, i32>           bitmap_index; /*!< tag -> out.bitmaps */

    static Vecf4 color_of(ui::color_argb_t const& c)
    {
        return {from_le(c.r), from_le(c.g), from_le(c.b), from_le(c.a)};
    }

    i32 bitmap(dim::tag_reference<blam::tag_class_t::bitm> const& ref)
    {
        auto const* tag = map.tag_at(ref.tag_id);
        if(!tag || !tag->matches(blam::tag_class_t::bitm))
            return -1;
        auto key = static_cast<u16>(tag - map.tags().data());
        if(auto it = bitmap_index.find(key); it != bitmap_index.end())
            return it->second;
        i32 idx = -1;
        if(auto tex = upload_bitmap(api, map, pool, *tag))
        {
            ShellBitmap bm{.name = std::string(map.name_of(*tag))};
            bm.texture.texture = *tex;
            bm.texture.sampler = (*tex)->sampler();
            bm.texture.sampler->alloc();
            bm.texture.sampler->set_filtering(
                typing::Filtering::Linear,
                typing::Filtering::Linear,
                typing::Filtering::Linear);
            if(auto const* image = dim::image_of(map, *tag))
            {
                bm.width  = static_cast<u32>(from_le(image->width));
                bm.height = static_cast<u32>(from_le(image->height));
            }
            idx = static_cast<i32>(out.bitmaps.size());
            out.bitmaps.push_back(std::move(bm));
        }
        bitmap_index.emplace(key, idx);
        return idx;
    }

    ui::string_list_t const* string_list(
        dim::tag_reference<blam::tag_class_t::unic> const& ref)
    {
        auto const* tag = map.tag_at(ref.tag_id);
        return tag && tag->matches(blam::tag_class_t::unic)
                   ? tag->template data<ui::string_list_t>(map.magic())
                   : nullptr;
    }

    std::string text_of(ui::string_list_t const* list, dim::string_id id)
    {
        if(!list)
            return {};
        return std::string(
            ui::list_string_by_id(map, *list, ui::language_t::english, id));
    }

    ShellText text_block(ui::text_block_t const& t, ui::string_list_t const* list)
    {
        return ShellText{
            .text      = text_of(list, t.text),
            .flags     = from_le(t.text_flags),
            .font      = static_cast<u32>(t.font),
            .color     = color_of(t.text_color),
            .bounds    = t.bounds,
            .animation = static_cast<u16>(t.animation),
            .depth     = from_le(t.render_depth_bias),
        };
    }

    ShellBitmapBlock bitmap_block(ui::bitmap_block_t const& b)
    {
        return ShellBitmapBlock{
            .bitmap   = bitmap(b.bitmap),
            .top_left = b.top_left,
            .wraps_per_second =
                {from_le(b.horizontal_wraps_per_second),
                 from_le(b.vertical_wraps_per_second)},
            .animation = static_cast<u16>(b.animation),
            .blend     = static_cast<u16>(b.blend),
            .flags     = from_le(b.flags),
            .depth     = from_le(b.render_depth_bias),
        };
    }

    template<typename Block>
    void each(Block const& block, auto&& fn)
    {
        auto items = block.data(map.magic());
        if(items.has_error())
            return;
        for(auto const& item : items.value())
            fn(item);
    }

    ShellAnimation animation(ui::screen_animation_t const& a)
    {
        ShellAnimation out{
            .intro_ms     = from_le(a.intro_period_ms),
            .outro_ms     = from_le(a.outro_period_ms),
            .ambient_ms   = from_le(a.ambient_period_ms),
            .ambient_loop = from_le(a.ambient_looping_style),
        };
        auto frames = [&](auto const& block, std::vector<ShellKeyframe>& dst) {
            each(block, [&](ui::keyframe_t const& k) {
                dst.push_back(
                    {.alpha    = from_le(k.alpha),
                     .position = {
                         from_le(k.position.x),
                         from_le(k.position.y),
                         from_le(k.position.z)}});
            });
        };
        frames(a.intro, out.intro);
        frames(a.outro, out.outro);
        frames(a.ambient, out.ambient);
        return out;
    }

    void load_screen(dim::tag_t const& tag)
    {
        auto const* s = tag.template data<ui::screen_t>(map.magic());
        if(!s)
            return;
        auto const* list = string_list(s->strings);
        ShellScreen screen{
            .name               = std::string(map.name_of(tag)),
            .id                 = s->screen_id,
            .button_key         = s->button_key,
            .flags              = from_le(s->flags),
            .shape_group        = from_le(s->shape_group),
            .header             = text_of(list, s->header),
            .default_text_color = color_of(s->default_text_color),
        };
        if(list)
        {
            auto const& range =
                list->languages[static_cast<size_t>(ui::language_t::english)];
            u32 count = static_cast<u32>(std::max<i16>(0, from_le(range.count)));
            for(u32 i = 0; i < count; i++)
                screen.strings.emplace_back(
                    ui::list_string(map, *list, ui::language_t::english, i));
        }
        each(s->panes, [&](ui::pane_t const& p) {
            ShellPane pane{.animation = static_cast<u16>(p.animation)};
            each(p.text_blocks, [&](ui::text_block_t const& t) {
                pane.texts.push_back(text_block(t, list));
            });
            each(p.buttons, [&](ui::button_t const& b) {
                pane.buttons.push_back(ShellText{
                    .text      = text_of(list, b.text),
                    .flags     = from_le(b.text_flags),
                    .font      = static_cast<u32>(b.font),
                    .color     = color_of(b.text_color),
                    .bounds    = b.bounds,
                    .animation = static_cast<u16>(b.animation),
                    .depth     = from_le(b.render_depth_bias),
                });
            });
            each(p.bitmap_blocks, [&](ui::bitmap_block_t const& b) {
                pane.bitmaps.push_back(bitmap_block(b));
            });
            each(p.lists, [&](ui::list_definition_t const& l) {
                pane.lists.push_back(ShellList{
                    .skin          = static_cast<u32>(l.skin),
                    .visible_items = from_le(l.visible_items),
                    .bottom_left   = l.bottom_left,
                    .flags         = from_le(l.flags),
                    .animation     = static_cast<u16>(l.animation),
                });
            });
            screen.panes.push_back(std::move(pane));
        });
        out.screens.push_back(std::move(screen));
    }

    void load_skin(dim::tag_t const& tag)
    {
        ShellSkin skin{.name = std::string(map.name_of(tag))};
        if(auto const* s = tag.template data<ui::list_skin_t>(map.magic()))
        {
            each(s->text_blocks, [&](ui::text_block_t const& t) {
                skin.texts.push_back(text_block(t, nullptr));
            });
            each(s->bitmap_blocks, [&](ui::bitmap_block_t const& b) {
                skin.bitmaps.push_back(bitmap_block(b));
            });
            each(s->item_animations, [&](ui::item_animation_t const& a) {
                ShellAnimation anim{.intro_ms = from_le(a.period_ms)};
                each(a.keyframes, [&](ui::keyframe_t const& k) {
                    anim.intro.push_back(
                        {.alpha    = from_le(k.alpha),
                         .position = {
                             from_le(k.position.x),
                             from_le(k.position.y),
                             from_le(k.position.z)}});
                });
                skin.item_animations.push_back(std::move(anim));
            });
        }
        out.skins.push_back(std::move(skin));
    }

    void load_globals(ui::shared_globals_t const& g)
    {
        out.overlay_color      = color_of(g.overlayed_interface_color);
        out.default_text_color = color_of(g.default_text_color);
        out.header_font        = static_cast<u32>(g.full_screen_header_font);
        out.header_bounds      = g.full_screen_header_bounds;
        out.button_key_bounds  = g.full_screen_button_key_bounds;
        out.half_header_bounds = g.half_dialog_header_bounds;
        out.half_button_key_bounds = g.half_dialog_button_key_bounds;
        if(auto const* keys = string_list(g.button_key_strings))
        {
            auto const& range =
                keys->languages[static_cast<size_t>(ui::language_t::english)];
            u32 count = static_cast<u32>(std::max<i16>(0, from_le(range.count)));
            for(u32 i = 0; i < count; i++)
                out.button_key_strings.emplace_back(
                    ui::list_string(map, *keys, ui::language_t::english, i));
        }
        each(g.screen_animations, [&](ui::screen_animation_t const& a) {
            out.animations.push_back(animation(a));
        });
        load_music(g);
        each(g.list_skins, [&](ui::list_skin_ref_t const& r) {
            if(auto const* tag = map.tag_at(r.skin.tag_id);
               tag && tag->matches(blam::tag_class_t::skin))
                load_skin(*tag);
            else
                out.skins.push_back({});
        });
    }

    /* Glyphs 0x20..0xFF plus the button icons at U+E100 into one RGBA8
     * atlas, white with the coverage in alpha */
    std::shared_ptr<ShellFont> load_font(ShellFontFile const& file)
    {
        auto font = blam::dimeter::font::font_t::from_bytes(file.bytes);
        if(!font)
        {
            cWarning("H2: {}: not a font file", file.name);
            return nullptr;
        }
        auto out = std::make_shared<ShellFont>();
        out->name    = file.name;
        out->ascent  = font->ascent();
        out->descent = font->descent();
        out->leading = from_le(font->header->leading);
        for(auto const& k : font->kerning)
            out->kerning[(static_cast<u32>(k.first) << 16) | k.second] =
                from_le(k.adjust);

        struct placed_t
        {
            char32_t                     codepoint;
            blam::dimeter::font::image_t image;
            u32                          x, y;
            blam::dimeter::font::glyph_t const* glyph;
        };
        std::vector<placed_t> placed;
        constexpr u32 atlas_width = 512;
        u32           shelf_x = 0, shelf_y = 0, shelf_h = 0;
        auto          add = [&](char32_t c) {
            auto const* g = font->glyph(c);
            if(!g)
                return;
            blam::dimeter::font::image_t image;
            if(!font->decode(*g, image))
                return;
            u32 w = image.width + 1, h = image.height + 1;
            if(shelf_x + w > atlas_width)
            {
                shelf_y += shelf_h;
                shelf_x = 0;
                shelf_h = 0;
            }
            placed.push_back(
                {.codepoint = c, .image = std::move(image), .x = shelf_x, .y = shelf_y, .glyph = g});
            shelf_x += w;
            shelf_h = std::max(shelf_h, h);
        };
        for(char32_t c = 0x20; c < 0x100; c++)
            add(c);
        for(char32_t c = 0xE100; c < 0xE110; c++)
            add(c);
        u32 atlas_height = 1;
        while(atlas_height < shelf_y + shelf_h)
            atlas_height *= 2;
        std::vector<u8> pixels(static_cast<size_t>(atlas_width) * atlas_height * 4, 0);
        for(auto const& p : placed)
        {
            for(u32 y = 0; y < p.image.height; y++)
                std::memcpy(
                    pixels.data() + ((p.y + y) * atlas_width + p.x) * 4,
                    p.image.rgba.data() + y * p.image.width * 4,
                    p.image.width * 4);
            out->glyphs[p.codepoint] = ShellGlyph{
                .uv =
                    {static_cast<f32>(p.x) / atlas_width,
                     static_cast<f32>(p.y) / atlas_height,
                     static_cast<f32>(p.x + p.image.width) / atlas_width,
                     static_cast<f32>(p.y + p.image.height) / atlas_height},
                .size   = {static_cast<f32>(p.image.width), static_cast<f32>(p.image.height)},
                .origin = {static_cast<f32>(from_le(p.glyph->origin_x)), static_cast<f32>(from_le(p.glyph->origin_y))},
                .advance = static_cast<f32>(from_le(p.glyph->advance)),
            };
        }
        out->atlas = api.alloc_texture(
            gfx::textures::d2,
            typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8),
            1);
        out->atlas->alloc(size_3d<u32>{atlas_width, atlas_height, 1});
        out->atlas->upload(
            std::move(pixels),
            Veci2{0, 0},
            Veci2{static_cast<i32>(atlas_width), static_cast<i32>(atlas_height)},
            0);
        out->sampler = out->atlas->sampler();
        out->sampler->alloc();
        out->sampler->set_filtering(
            typing::Filtering::Linear, typing::Filtering::Linear);
        cDebug(
            "H2: font {}: {} glyphs in {}x{}, ascent {} descent {}",
            file.name,
            out->glyphs.size(),
            atlas_width,
            atlas_height,
            out->ascent,
            out->descent);
        return out;
    }

    void load_fonts(
        std::vector<std::string> const&   table,
        std::vector<ShellFontFile> const& files)
    {
        std::map<std::string, std::shared_ptr<ShellFont>> by_name;
        for(size_t slot = 0; slot < out.fonts.size() && slot < table.size(); slot++)
        {
            auto const& name = table[slot];
            if(auto it = by_name.find(name); it != by_name.end())
            {
                out.fonts[slot] = it->second;
                continue;
            }
            auto file = std::find_if(files.begin(), files.end(), [&](auto const& f) {
                return f.name == name;
            });
            auto font = file == files.end() ? nullptr : load_font(*file);
            by_name[name]   = font;
            out.fonts[slot] = font;
        }
    }

    /* Permutation 0 of a snd!, its chunks joined: Xbox ADPCM blocks, or
     * a WMA file for the "codec" encoding (whatever the compression byte
     * says) */
    bool sound_blocks(
        dim::tag_reference<blam::tag_class_t::snd> const& ref,
        dim::snd::gestalt const*                          gestalt,
        H2Sound&                                          sound_out)
    {
        auto const* tag = map.tag_at(ref.tag_id);
        auto const* sound =
            tag && tag->matches(blam::tag_class_t::snd) ? tag->template data<dim::snd::sound>(map.magic())
                                        : nullptr;
        bool const wma = sound && sound->encoding == dim::snd::encoding_t::codec;
        if(!sound || !gestalt ||
           (!wma && sound->compression != dim::snd::compression_t::xbox_adpcm))
        {
            cDebug(
                "H2: sound {}: {} (compression {}, encoding {})",
                tag ? map.name_of(*tag) : std::string_view("<none>"),
                !sound ? "no sound body" : !gestalt ? "no gestalt" : "unsupported",
                sound ? static_cast<int>(sound->compression) : -1,
                sound ? static_cast<int>(sound->encoding) : -1);
            return false;
        }
        auto ranges = gestalt->pitch_ranges.data(map.magic());
        auto perms  = gestalt->permutations.data(map.magic());
        auto chunks = gestalt->chunks.data(map.magic());
        if(ranges.has_error() || perms.has_error() || chunks.has_error())
            return false;
        auto ri = from_le(sound->pitch_range_index);
        if(ri < 0 || static_cast<size_t>(ri) >= ranges.value().size())
            return false;
        auto const& range = ranges.value()[ri];
        auto        pi    = from_le(range.first_permutation);
        if(pi < 0 || static_cast<size_t>(pi) >= perms.value().size() ||
           from_le(range.permutation_count) < 1)
            return false;
        auto const& perm   = perms.value()[pi];
        auto&       blocks = sound_out.blocks;
        blocks.clear();
        blocks.reserve(perm.sample_size());
        i16 first = from_le(perm.first_chunk), count = from_le(perm.chunk_count);
        for(i16 c = first; c < first + count; c++)
        {
            if(c < 0 || static_cast<size_t>(c) >= chunks.value().size())
                return false;
            auto const& chunk = chunks.value()[c];
            auto        raw   = pool.resolve(chunk.offset, chunk.size());
            if(!raw)
                return false;
            blocks.insert(
                blocks.end(),
                reinterpret_cast<char const*>(raw->data()),
                reinterpret_cast<char const*>(raw->data()) + raw->size());
        }
        sound_out.name     = std::string(map.name_of(*tag));
        sound_out.wma      = wma;
        sound_out.channels = sound->encoding == dim::snd::encoding_t::stereo ? 2 : 1;
        sound_out.rate     = dim::snd::to_hertz(sound->sample_rate);
        cDebug(
            "H2: sound {}: {} Hz {}ch, {} bytes{}",
            sound_out.name,
            sound_out.rate,
            sound_out.channels,
            blocks.size(),
            wma ? " (wma)" : "");
        return !blocks.empty();
    }

    std::string tag_label_or(dim::tag_t const* tag) const
    {
        return tag ? std::string(map.name_of(*tag)) : std::string("<none>");
    }

    /* wigl's main menu music: the first track's intro and loop */
    void load_music(ui::shared_globals_t const& g)
    {
        auto const* lsnd_tag = map.tag_at(g.main_menu_music.tag_id);
        auto const* lsnd =
            lsnd_tag && lsnd_tag->matches(blam::tag_class_t::lsnd)
                ? lsnd_tag->template data<dim::snd::looping_sound>(map.magic())
                : nullptr;
        if(!lsnd)
        {
            cDebug("H2: sound: no menu music lsnd ({})", tag_label_or(lsnd_tag));
            return;
        }
        dim::snd::gestalt const* gestalt = nullptr;
        for(auto const& tag : map.tags())
            if(tag.valid() && tag.matches(blam::tag_class_t::ugh))
            {
                gestalt = tag.template data<dim::snd::gestalt>(map.magic());
                break;
            }
        auto tracks = lsnd->tracks.data(map.magic());
        if(tracks.has_error() || tracks.value().empty())
        {
            cDebug("H2: sound: lsnd without tracks");
            return;
        }
        auto const& track = tracks.value()[0];
        bank.music.name   = std::string(map.name_of(*lsnd_tag));
        bank.music.gain   = from_le(track.gain) > 0.f ? from_le(track.gain) : 1.f;
        sound_blocks(track.in, gestalt, bank.music.in);
        sound_blocks(track.loop, gestalt, bank.music.loop);
        bank.loaded = !bank.music.loop.blocks.empty();
    }

    void load_camera_path()
    {
        auto const* scnr_tag = map.scenario();
        auto const* scnr =
            scnr_tag ? scnr_tag->template data<dim::scn::scenario_t>(map.magic())
                     : nullptr;
        if(!scnr)
            return;
        each(scnr->cutscene_camera_points, [&](dim::scn::cutscene_camera_point_t const& c) {
            auto name = c.name.str();
            if(!name.starts_with("ui_path_"))
                return;
            out.camera_path.push_back(ShellCameraPoint{
                .name     = std::string(name),
                .position = {
                    from_le(c.position.x), from_le(c.position.y), from_le(c.position.z)},
                .orientation = {
                    from_le(c.orientation.x),
                    from_le(c.orientation.y),
                    from_le(c.orientation.z)},
            });
        });
        std::sort(
            out.camera_path.begin(), out.camera_path.end(), [](auto const& a, auto const& b) {
                return a.name < b.name;
            });
    }

    bool load(
        std::vector<std::string> const&   font_table,
        std::vector<ShellFontFile> const& font_files)
    {
        dim::tag_t const* wgtz = nullptr;
        for(auto const& tag : map.tags())
            if(tag.valid() && tag.matches(blam::tag_class_t::wgtz))
            {
                wgtz = &tag;
                break;
            }
        auto const* globals =
            wgtz ? wgtz->template data<ui::globals_t>(map.magic()) : nullptr;
        if(!globals)
            return false;
        if(auto const* wigl_tag = map.tag_at(globals->shared_globals.tag_id))
            if(auto const* wigl =
                   wigl_tag->template data<ui::shared_globals_t>(map.magic()))
                load_globals(*wigl);
        each(globals->screens, [&](ui::screen_widget_ref_t const& r) {
            if(auto const* tag = map.tag_at(r.widget.tag_id);
               tag && tag->matches(blam::tag_class_t::wgit))
                load_screen(*tag);
        });
        load_fonts(font_table, font_files);
        load_camera_path();
        cDebug(
            "H2: shell: {} screens, {} skins, {} bitmaps, {} animations, {} "
            "camera points, {} fonts",
            out.screens.size(),
            out.skins.size(),
            out.bitmaps.size(),
            out.animations.size(),
            out.camera_path.size(),
            std::count_if(out.fonts.begin(), out.fonts.end(), [](auto const& f) {
                return f != nullptr;
            }));
        return !out.screens.empty();
    }
};

} // namespace detail

template<typename V>
inline bool load_shell(
    gfx::api&                         api,
    dim::map_container<V> const&      map,
    dim::raw_pool const&              pool,
    std::vector<std::string> const&   font_table,
    std::vector<ShellFontFile> const& font_files,
    H2ShellData&                      out,
    H2SoundBank&                      bank)
{
    out        = {};
    out.loaded = false;
    bank       = {};
    detail::ShellLoader<V> loader{api, map, pool, out, bank};
    out.loaded = loader.load(font_table, font_files);
    return out.loaded;
}

} // namespace h2
