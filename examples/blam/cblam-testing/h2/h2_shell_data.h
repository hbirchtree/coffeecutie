#pragma once

#include "h2_scene.h"

#include <blam/dimeter/ui.h>

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/* The main menu shell as the loader decodes it from mainmenu.map: screens
 * with their text resolved, bitmaps and fonts on the GPU, the camera path.
 * H2MapLoader writes it, H2Shell reads it */
namespace h2 {

namespace ui = blam::dimeter::ui;

struct ShellGlyph
{
    Vecf4 uv;      /*!< left, top, right, bottom in the atlas */
    Vecf2 size;    /*!< texels */
    Vecf2 origin;  /*!< x: left bearing, y: rows above the baseline */
    f32   advance{0.f};
};

struct ShellFont
{
    std::string                              name;
    i32                                      ascent{0}, descent{0}, leading{0};
    std::shared_ptr<gfx::texture_2d_t>       atlas;
    std::shared_ptr<gfx::sampler_t>          sampler;
    std::unordered_map<char32_t, ShellGlyph> glyphs;
    std::unordered_map<u32, i16>             kerning; /*!< first << 16 | second */

    inline ShellGlyph const* glyph(char32_t c) const
    {
        auto it = glyphs.find(c);
        return it == glyphs.end() ? nullptr : &it->second;
    }

    inline f32 kern(char32_t a, char32_t b) const
    {
        auto it = kerning.find((static_cast<u32>(a) << 16) | static_cast<u32>(b));
        return it == kerning.end() ? 0.f : static_cast<f32>(it->second);
    }
};

struct ShellBitmap
{
    std::string name;
    Texture     texture;
    u32         width{0}, height{0};
};

struct ShellKeyframe
{
    f32   alpha{1.f};
    Vecf3 position{};
};

struct ShellAnimation
{
    i32                        intro_ms{0}, outro_ms{0}, ambient_ms{0};
    i16                        ambient_loop{0}; /*!< 1 reverse loop, 2 loop */
    std::vector<ShellKeyframe> intro, outro, ambient;
};

struct ShellText
{
    std::string text; /*!< UTF-8; empty when the string id resolved to none */
    u32         string_index{0}; /*!< Into the screen's strings */
    u32         flags{0};
    u32         font{0};
    Vecf4       color{1.f, 1.f, 1.f, 1.f};
    ui::rect16_t bounds{};
    u16         animation{0};
    i16         depth{0};
};

struct ShellBitmapBlock
{
    i32          bitmap{-1}; /*!< Into H2ShellData::bitmaps */
    ui::point16_t top_left{};
    Vecf2        wraps_per_second{0.f, 0.f};
    u16          animation{0};
    u16          blend{0};
    u32          flags{0};
    i16          depth{0};
};

struct ShellList
{
    u32           skin{0};
    i32           visible_items{0};
    ui::point16_t bottom_left{};
    u32           flags{0};
    u16           animation{0};
};

struct ShellPane
{
    u16                           animation{0};
    std::vector<ShellText>        texts;
    std::vector<ShellText>        buttons;
    std::vector<ShellBitmapBlock> bitmaps;
    std::vector<ShellList>        lists;
};

struct ShellScreen
{
    std::string              name; /*!< Tag path */
    ui::screen_id_t          id{};
    ui::button_key_t         button_key{};
    u32                      flags{0};
    i16                      shape_group{0};
    std::string              header;
    Vecf4                    default_text_color{1.f, 1.f, 1.f, 1.f};
    std::vector<std::string> strings; /*!< The screen's string list, English */
    std::vector<ShellPane>   panes;
};

struct ShellSkin
{
    std::string                   name;
    std::vector<ShellText>        texts;
    std::vector<ShellBitmapBlock> bitmaps;
    std::vector<ShellAnimation>   item_animations; /*!< select, deselect, idle */
};

struct ShellCameraPoint
{
    std::string name;
    Vecf3       position;
    Vecf3       orientation; /*!< yaw, pitch, roll, radians */
};

struct H2ShellData : compo::SubsystemBase
{
    using type = H2ShellData;

    bool loaded{false};
    u32  generation{0};

    std::vector<ShellScreen>        screens;
    std::vector<ShellSkin>          skins;
    std::vector<ShellAnimation>     animations;
    std::vector<ShellBitmap>        bitmaps;
    std::array<std::shared_ptr<ShellFont>, 11> fonts; /*!< By ui::font_t slot */
    std::vector<std::string>        button_key_strings;
    std::vector<ShellCameraPoint>   camera_path; /*!< ui_path_* in order */
    Vecf4                           overlay_color{0.f, 0.f, 0.f, 0.f};
    Vecf4                           default_text_color{1.f, 1.f, 1.f, 1.f};
    u32                             header_font{2};
    ui::rect16_t                    header_bounds{};
    ui::rect16_t                    button_key_bounds{};
    ui::rect16_t                    half_header_bounds{};
    ui::rect16_t                    half_button_key_bounds{};

    inline ShellScreen const* screen(ui::screen_id_t id) const
    {
        for(auto const& s : screens)
            if(s.id == id)
                return &s;
        return nullptr;
    }

    inline ShellScreen const* screen(std::string_view name_suffix) const
    {
        for(auto const& s : screens)
            if(s.name.ends_with(name_suffix))
                return &s;
        return nullptr;
    }
};

void alloc_h2_shell(compo::EntityContainer& e);

} // namespace h2
