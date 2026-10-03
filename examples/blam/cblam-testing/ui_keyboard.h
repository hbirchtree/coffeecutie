#pragma once

#include "data_cache.h"

#include <blam/volta/blam_ui.h>

#include <cmath>
#include <optional>
#include <string>
#include <vector>

/* Everything the renderer needs from the map's vcky tag */
struct VirtualKeyboardData
{
    struct key_t
    {
        blam::virtual_keyboard::virtual_key_t const* key{nullptr};
        generation_idx_t                             selected; /*!< highlight */
        generation_idx_t                             active;   /*!< pressed */
        generation_idx_t                             sticky;   /*!< mode on */
    };

    generation_idx_t            font_id;
    generation_idx_t            background;
    std::vector<std::u16string> labels; /*!< 0-7 special keys, 8+ prompts */
    std::vector<key_t>          keys;   /*!< indexed by virtual_key_t::key */

    bool valid() const
    {
        return font_id.valid() && !keys.empty();
    }
};

/* The engine draws the keys over the background's empty key panel; the tag
 * has no positions. Centres in UI space, read off the keyboard preview on the
 * profile settings screen. */
struct VirtualKeyboardLayout
{
    struct slot_t
    {
        libc_types::u16 key;
        libc_types::f32 x, y;
    };

    static constexpr libc_types::f32 panel_x  = 106.f;
    static constexpr libc_types::f32 panel_y  = 160.f;
    static constexpr libc_types::f32 column_0 = panel_x + 120.f;
    static constexpr libc_types::f32 row_0    = panel_y + 21.f;
    static constexpr libc_types::f32 pitch    = 31.f;

    /* Text field inside the background */
    static constexpr libc_types::f32 field_x = 118.f;
    static constexpr libc_types::f32 field_y = 112.f;
    static constexpr libc_types::f32 field_h = 36.f;
    /* Content of the 1024x512 background image */
    static constexpr libc_types::f32 background_w = 640.f;
    static constexpr libc_types::f32 background_h = 444.f;

    static std::vector<slot_t> const& slots()
    {
        static std::vector<slot_t> const out = [] {
            std::vector<slot_t> s;
            auto row = [](libc_types::u32 r) { return row_0 + pitch * r; };
            auto col = [](libc_types::u32 c) { return column_0 + pitch * c; };
            constexpr libc_types::f32 special_x = panel_x + 56.f;

            s.push_back({36, special_x, row(0)}); /* Done */
            s.push_back({37, special_x, row(1)}); /* Shift */
            s.push_back({38, special_x, row(2)}); /* Caps Lock */
            for(libc_types::u16 i = 0; i < 10; ++i)
            {
                s.push_back({i, col(i), row(0)});                         /* 1-0 */
                s.push_back({static_cast<libc_types::u16>(10 + i), col(i), row(1)}); /* a-j */
                s.push_back({static_cast<libc_types::u16>(20 + i), col(i), row(2)}); /* k-t */
                if(i < 6)
                    s.push_back(
                        {static_cast<libc_types::u16>(30 + i), col(i), row(3)}); /* u-z */
            }
            s.push_back({40, panel_x + 353.f, row(3)}); /* Backspace */
            s.push_back({43, panel_x + 198.f, row(4)}); /* Space */
            s.push_back({41, panel_x + 321.f, row(4)}); /* <- */
            s.push_back({42, panel_x + 383.f, row(4)}); /* -> */
            return s;
        }();
        return out;
    }
};

/* One text entry in progress */
struct VirtualKeyboard
{
    using key_t  = blam::virtual_keyboard::virtual_key_t;
    using mode_t = key_t::input_mode_t;

    static constexpr libc_types::u16 first_special = 36;
    static constexpr size_t          max_length    = 11;

    blam::ui_element::function_t function{}; /*!< run_function that opened it, gets the text */
    libc_types::u16 prompt{8};   /*!< index into the special key labels */
    std::u16string  text;
    size_t          cursor{0};
    mode_t          mode{};
    size_t          selected{0}; /*!< index into VirtualKeyboardLayout::slots() */

    libc_types::u16 selected_key() const
    {
        return VirtualKeyboardLayout::slots()[selected].key;
    }

    /* Nearest key in the direction, preferring the same row or column */
    void move(libc_types::i32 dx, libc_types::i32 dy)
    {
        auto const& slots = VirtualKeyboardLayout::slots();
        auto const& from  = slots[selected];

        std::optional<size_t> best;
        libc_types::f32       best_cost = 0.f;
        for(size_t i = 0; i < slots.size(); ++i)
        {
            libc_types::f32 const along =
                dx != 0 ? (slots[i].x - from.x) * static_cast<libc_types::f32>(dx)
                        : (slots[i].y - from.y) * static_cast<libc_types::f32>(dy);
            libc_types::f32 const across =
                dx != 0 ? std::abs(slots[i].y - from.y)
                        : std::abs(slots[i].x - from.x);
            if(along < 4.f)
                continue;
            libc_types::f32 const cost = along + across * 3.f;
            if(!best || cost < best_cost)
            {
                best      = i;
                best_cost = cost;
            }
        }
        if(best)
            selected = *best;
    }

    /* Returns true when Done was pressed */
    bool press(VirtualKeyboardData const& data)
    {
        auto const index = selected_key();
        if(index >= data.keys.size() || !data.keys[index].key)
            return false;
        auto token = data.keys[index].key->tokenize(mode);
        if(!token)
            return false;

        auto const [character, action, next_mode] = *token;
        using action_t = key_t::action_t;
        switch(action)
        {
        case action_t::done:
            return true;
        case action_t::backspace:
            if(cursor > 0)
                text.erase(--cursor, 1);
            break;
        case action_t::left:
            if(cursor > 0)
                --cursor;
            break;
        case action_t::right:
            if(cursor < text.size())
                ++cursor;
            break;
        case action_t::none:
            if(character != 0)
            {
                if(text.size() < max_length)
                    text.insert(cursor++, 1, character);
                /* Shift applies to one character */
                mode       = next_mode;
                mode.shift = false;
                return false;
            }
            break;
        }
        mode = next_mode;
        return false;
    }

    /* What a key shows in the current mode */
    std::u16string_view label(
        VirtualKeyboardData const& data, libc_types::u16 index) const
    {
        if(index >= first_special)
        {
            auto const label_idx = index - first_special;
            return label_idx < data.labels.size() ? data.labels[label_idx]
                                                  : std::u16string_view{};
        }
        if(index >= data.keys.size() || !data.keys[index].key)
            return {};
        auto token = data.keys[index].key->tokenize(mode);
        if(!token || std::get<0>(*token) == 0)
            return {};
        m_label = std::get<0>(*token);
        return m_label;
    }

    /* Sticky keys show their engaged look while the mode is on */
    bool engaged(libc_types::u16 index) const
    {
        return (index == 37 && mode.shift) || (index == 38 && mode.caps) ||
               (index == 39 && mode.symbols);
    }

  private:
    mutable std::u16string m_label;
};
