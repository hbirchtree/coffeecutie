#include "forge_controller.h"

#include "blam_files.h"
#include "components.h"
#include "crunched/loading_screen.h"
#include "data.h"
#include "network/networking.h"
#include "render/rendering.h"
#include "selected_version.h"
#include "ui.h"

#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/types/input/keymap_latin1.h>

#include <algorithm>
#include <array>
#include <map>

using ForgeControllerManifest = compo::SubsystemManifest<
    type_list_t<
        const PhysicsData,
        const PlayerCamera,
        const PlayerInfo,
        const PlayerInput
    >,
    type_list_t<
        gfx::system,
        PostProcessParameters,
        ScreenClear,
        ScreenText,
        GameEventBus,
        const BlamFiles<halo_version>,
        const NetworkState
    >,
    type_list_t<
        comp_app::ControllerInput,
        comp_app::EventBus<SoundEvent>,
        comp_app::GraphicsFramebuffer
    >
>;

struct ForgeController : compo::RestrictedSubsystem<ForgeController, ForgeControllerManifest>
{
    using type = ForgeController;
    using Proxy = compo::proxy_of<ForgeControllerManifest>;

    void start_restricted(Proxy& p, compo::time_point const& t)
    {
        if(!forge_cursor)
        {
            load_resources(p.subsystem<gfx::system>());
        }

        PostProcessParameters* postproc;
        p.subsystem(postproc);

        auto* framebuffer = p.service<comp_app::GraphicsFramebuffer>();
        comp_app::EventBus<SoundEvent>* sound_bus = p.service<comp_app::EventBus<SoundEvent>>();

        auto fb_size = framebuffer->size();

        auto& extra_quads = p.subsystem<ScreenClear>().extra_quads;

        // Set up forge cursors
        u32 num_players{0};
        for(auto const& player : p.select<PlayerInfo, PlayerCamera>())
        {
            auto [info, camera] = player.components();
            if(!camera.is_active())
                continue;
            num_players++;
        }
        auto screen_bounds = [&](PlayerInfo const& info) -> Vecf4 {
            switch(num_players)
            {
            case 2:
                return Vecf4(
                    0,
                    (fb_size.h / 2) * info.seat_idx,
                    fb_size.w,
                    fb_size.h / 2);
            case 3:
            case 4:
            {
                auto x = info.seat_idx % 2;
                auto y = 1 - info.seat_idx / 2;
                return Vecf4(
                    (fb_size.w / 2) * x,
                    (fb_size.h / 2) * y,
                    fb_size.w / 2,
                    fb_size.h / 2);
            }
            default:
                return Vecf4(0, 0, fb_size.w, fb_size.h);
            }
        };
        auto* controllers = p.service<comp_app::ControllerInput>();
        for(auto const& player : p.select<PlayerInfo, PlayerInput, PlayerCamera>())
        {
            auto [info, input, camera] = player.components();
            /* Tracked while hidden too, so a button still held as a menu
             * closes does not count as pressed */
            auto& buttons = m_buttons[player.id()];
            sample_buttons(buttons, input, camera, controllers);
            /* Only in game: this player's open menu owns the buttons */
            if(info.mode.physics ||
                    !camera.is_active() ||
                    !postproc->forge_overlay ||
                    input.input_mode != PlayerInput::input_mode_t::game)
            {
                buttons.frame_end();
                continue;
            }

            f32 aspect = fb_size.aspect();
            auto bounds = screen_bounds(info);
            auto size = Vecf2{bounds.z * 0.1f / aspect, bounds.w * 0.1f};
            /* Green while carrying something, cyan when idle */
            auto const* physics = p.get<PhysicsData>(player.id());
            bool const  holding = physics && physics->grabbed != 0;
            // Forge cursor
            extra_quads.push_back({
                .position = Vecf2{bounds.x, bounds.y} +
                            Vecf2{bounds.z / 2 - size.x / 2, bounds.w / 2 + size.y / 2},
                .size     = Vecf2{size.x, -size.y},
                .atlas_offset = Vecf2{0.f, 0.5f},
                .atlas_scale = Vecf2{0.5f, 0.5f},
                .tint     = holding ? Vecf4{0.1f, 1.5f, 0.15f, 0.9f}
                                    : Vecf4{0.15f, 1.1f, 1.5f, 0.8f},
                .sampler  = forge_cursor_smp,
            });
            draw_menu(p, player.id(), buttons, camera, bounds, fb_size.h);
            buttons.frame_end();
        }
    }

    // Object picker
    static constexpr u32 box_size = 480;
    static constexpr u32 box_width = 300;
    static constexpr u32 element_size = 40;
    static constexpr u32 padding = 4;
    static constexpr u32 element_spacing = element_size + padding;
    static constexpr u32 box_margin = 10;
    static constexpr u32 screen_margin = 10;
    static constexpr u32 icon_size = 36;
    /* Rows below the header */
    static constexpr u32 visible_rows =
        (box_size - box_margin * 2) / element_spacing - 1;
    static constexpr f32 spawn_distance = 3.f;

    struct entry_t
    {
        blam::tag_t const* tag;
        std::u16string     label;
    };

    struct group_t
    {
        blam::tag_class_t    tag_class;
        std::u16string       title;
        std::vector<entry_t> entries;
    };

    struct buttons_t
    {
        debounced_button_t up{};
        debounced_button_t down{};
        debounced_button_t left{};
        debounced_button_t right{};
        debounced_button_t accept{};
        debounced_button_t cancel{};

        void frame_end()
        {
            up.frame_end();
            down.frame_end();
            left.frame_end();
            right.frame_end();
            accept.frame_end();
            cancel.frame_end();
        }
    };

    /* D-pad, A and B, or arrows, enter and backspace */
    static void sample_buttons(
        buttons_t&                             buttons,
        PlayerInput const&                     input,
        PlayerCamera const&                    camera,
        comp_app::interfaces::ControllerInput* controllers)
    {
        using namespace Coffee::Input;
        if(controllers && camera.controller.index)
        {
            auto const& b = controllers->state(*camera.controller.index).buttons.e;
            buttons.up     |= b.p_up;
            buttons.down   |= b.p_down;
            buttons.left   |= b.p_left;
            buttons.right  |= b.p_right;
            buttons.accept |= b.a;
            buttons.cancel |= b.b;
        }
        if(camera.keyboard.enabled)
        {
            auto key = [&input](u16 k) {
                return StandardCamera::has_key(input.keys, k);
            };
            buttons.up     |= key(CK_Up);
            buttons.down   |= key(CK_Down);
            buttons.left   |= key(CK_Left);
            buttons.right  |= key(CK_Right);
            buttons.accept |= key(CK_EnterCR);
            buttons.cancel |= key(CK_BackSpace);
        }
    }

    /* group < 0 lists the groups */
    struct menu_t
    {
        i32 group{-1};
        u32 row{0};
        u32 scroll{0};
    };

    static std::u16string widen(std::string_view text)
    {
        return {text.begin(), text.end()};
    }

    /* Object tag classes in menu order, the rest by their class code */
    static std::pair<u32, std::u16string> class_title(blam::tag_class_t cls)
    {
        using blam::tag_class_t;
        constexpr std::array<std::pair<tag_class_t, std::string_view>, 11>
            titles = {{
                {tag_class_t::vehi, "Vehicles"},
                {tag_class_t::weap, "Weapons"},
                {tag_class_t::eqip, "Equipment"},
                {tag_class_t::scen, "Scenery"},
                {tag_class_t::bipd, "Bipeds"},
                {tag_class_t::mach, "Machines"},
                {tag_class_t::ctrl, "Controls"},
                {tag_class_t::lifi, "Light fixtures"},
                {tag_class_t::garb, "Garbage"},
                {tag_class_t::proj, "Projectiles"},
                {tag_class_t::ssce, "Sound scenery"},
            }};
        for(u32 i = 0; i < titles.size(); i++)
            if(titles[i].first == cls)
                return {i, widen(titles[i].second)};
        auto const     v = static_cast<u32>(cls);
        std::u16string code;
        for(auto shift : {24, 16, 8, 0})
            if(char c = static_cast<char>((v >> shift) & 0xFF))
                code.push_back(static_cast<char16_t>(c));
        return {static_cast<u32>(titles.size()), code};
    }

    /* Spawnable object tags of the loaded map, grouped by class */
    void refresh_catalog(BlamFiles<halo_version> const& files)
    {
        if(files.load_generation == m_catalog_generation)
            return;
        m_catalog_generation = files.load_generation;
        m_groups.clear();
        m_menus.clear();

        blam::tag_index_view<halo_version> index(files.container);
        for(auto const& tag : index)
        {
            if(!tag.valid() || !tag.matches(blam::tag_class_t::obje))
                continue;
            auto group = std::find_if(
                m_groups.begin(), m_groups.end(), [&tag](group_t const& g) {
                    return g.tag_class == tag.tag_class();
                });
            if(group == m_groups.end())
            {
                m_groups.push_back({
                    .tag_class = tag.tag_class(),
                    .title     = class_title(tag.tag_class()).second,
                });
                group = std::prev(m_groups.end());
            }
            /* The last path part names it; the folder tells twins apart */
            std::string_view path   = index.name_of(tag);
            auto const       cut    = path.rfind('\\');
            std::string_view name   = path.substr(cut + 1);
            std::string_view folder = cut == std::string_view::npos
                                          ? std::string_view{}
                                          : path.substr(0, cut);
            folder = folder.substr(folder.rfind('\\') + 1);
            group->entries.push_back({&tag, widen(name)});
            m_folders[&tag] = widen(folder);
        }
        for(auto& group : m_groups)
        {
            std::sort(
                group.entries.begin(),
                group.entries.end(),
                [](entry_t const& a, entry_t const& b) {
                    return a.label < b.label;
                });
            for(size_t i = 0; i < group.entries.size(); i++)
            {
                bool const twin =
                    (i > 0 &&
                     group.entries[i - 1].label == group.entries[i].label) ||
                    (i + 1 < group.entries.size() &&
                     group.entries[i + 1].label == group.entries[i].label);
                if(twin)
                    group.entries[i].label +=
                        u" (" + m_folders[group.entries[i].tag] + u")";
            }
        }
        std::sort(
            m_groups.begin(), m_groups.end(), [](group_t const& a, group_t const& b) {
                auto const [ia, ta] = class_title(a.tag_class);
                auto const [ib, tb] = class_title(b.tag_class);
                return ia != ib ? ia < ib : ta < tb;
            });
        m_folders.clear();
    }

    u32 rows_of(menu_t const& menu) const
    {
        if(menu.group < 0)
            return static_cast<u32>(m_groups.size());
        return static_cast<u32>(m_groups[menu.group].entries.size());
    }

    void navigate(
        Proxy& p, menu_t& menu, buttons_t const& pressed, PlayerCamera const& camera)
    {
        u32 const rows = rows_of(menu);
        if(pressed.up && menu.row > 0)
            menu.row--;
        if(pressed.down && menu.row + 1 < rows)
            menu.row++;
        if(menu.group < 0)
        {
            if((pressed.right || pressed.accept) && rows)
                menu = {.group = static_cast<i32>(menu.row)};
        } else if(pressed.left || pressed.cancel)
            menu = {.row = static_cast<u32>(menu.group)};
        else if(pressed.accept && rows)
            spawn(p, m_groups[menu.group].entries[menu.row], camera);
        if(menu.row < menu.scroll)
            menu.scroll = menu.row;
        if(menu.row >= menu.scroll + visible_rows)
            menu.scroll = menu.row + 1 - visible_rows;
    }

    /* In front of the camera, facing its way; a client asks the server */
    void spawn(Proxy& p, entry_t const& entry, PlayerCamera const& camera)
    {
        auto const& net    = p.subsystem<NetworkState>();
        bool const  client = !net.clock_authority &&
                            net.client_state != NetworkState::ClientState::None;
        Vecf3 const forward =
            glm::transpose(Matf3(camera.rotation)) * Vecf3{0.f, 0.f, -1.f};
        SpawnObjectEvent spawn{
            .object   = entry.tag->as_ref(),
            .position = camera.camera.position + forward * spawn_distance,
            .rotation = glm::angleAxis(
                std::atan2(forward.y, forward.x), Vecf3{0.f, 0.f, 1.f}),
            .server_owned = client,
        };
        GameEvent ev{.type = GameEvent::SpawnObject};
        p.subsystem<GameEventBus>().inject(ev, &spawn);
    }

    void draw_menu(
        Proxy&              p,
        u64                 player,
        buttons_t const&    pressed,
        PlayerCamera const& camera,
        Vecf4 const&        bounds,
        f32                 fb_height)
    {
        refresh_catalog(p.subsystem<BlamFiles<halo_version>>());
        auto& menu = m_menus[player];
        navigate(p, menu, pressed, camera);

        auto& extra_quads = p.subsystem<ScreenClear>().extra_quads;
        auto& text        = p.subsystem<ScreenText>().lines;
        Vecf2 const origin{
            bounds.x + screen_margin, bounds.y + screen_margin};
        /* Bottom of row slot k, slot 0 being the header */
        auto row_y = [&](u32 slot) {
            return origin.y + box_size - box_margin -
                   static_cast<f32>(slot + 1) * element_spacing;
        };
        /* Text is laid out top-down in window pixels */
        auto put_text = [&](std::u16string line, f32 x, u32 slot, Vecf4 color) {
            text.push_back({
                .text     = std::move(line),
                .baseline = {x, fb_height - (row_y(slot) + element_size * .3f)},
                .color    = color,
                .max_width = box_width - box_margin * 2 - icon_size * 2 - 12,
            });
        };

        extra_quads.push_back({
            .position = origin,
            .size = Vecf2{box_width, box_size},
            .atlas_offset = Vecf2{0.22f, 0.22f},
            .atlas_scale = Vecf2{0.02f, 0.02f},
            .tint = Vecf4{0, 0.75f, 1.f, 0.35f},
            .sampler = forge_cursor_smp,
        });

        u32 const rows = rows_of(menu);
        std::u16string header =
            menu.group < 0 ? u"Spawn" : u"< " + m_groups[menu.group].title;
        if(rows)
            header += u"  " + widen(std::to_string(menu.row + 1)) + u"/" +
                      widen(std::to_string(rows));
        put_text(header, origin.x + box_margin + 4, 0, Vecf4{0.6f, 0.9f, 1.f, 1.f});
        if(!rows)
            put_text(u"No map loaded", origin.x + box_margin + 4, 1, Vecf4{0.8f});

        for(u32 slot = 1; slot <= visible_rows; slot++)
        {
            u32 const idx = menu.scroll + slot - 1;
            if(idx >= rows)
                break;
            bool const selected = idx == menu.row;
            std::u16string label =
                menu.group < 0
                    ? m_groups[idx].title + u" (" +
                          widen(std::to_string(m_groups[idx].entries.size())) +
                          u")"
                    : m_groups[menu.group].entries[idx].label;
            put_text(
                std::move(label),
                origin.x + box_margin + icon_size + 8,
                slot,
                selected ? Vecf4{1.f} : Vecf4{0.75f, 0.9f, 1.f, 0.9f});
            if(!selected)
                continue;
            // Object picker select
            Vecf2 const select_box{origin.x + box_margin, row_y(slot)};
            extra_quads.push_back({
                .position = select_box,
                .size = Vecf2{box_width - box_margin * 2, element_size},
                .atlas_offset = Vecf2{0.22f, 0.22f},
                .atlas_scale = Vecf2{0.02f, 0.02f},
                .tint = Vecf4{0.9f, 0.2f, 0.f, 0.35f},
                .sampler = forge_cursor_smp,
            });
            extra_quads.push_back({
                .position = select_box + Vecf2{
                    box_width - box_margin * 2 - icon_size - 2,
                    2 + icon_size,
                },
                .size = Vecf2{icon_size, -static_cast<f32>(icon_size)},
                .atlas_offset = Vecf2{0.5f, 0},
                .atlas_scale = Vecf2{0.5f, 0.5f},
                .tint = pressed.accept ? Vecf4{1}
                                                       : Vecf4{Vecf3{0.2f}, 1},
                .sampler = forge_cursor_smp,
            });
            extra_quads.push_back({
                .position = select_box + Vecf2{2, 2},
                .size = Vecf2{icon_size, icon_size},
                .atlas_offset = Vecf2{0, 0},
                .atlas_scale = Vecf2{0.5f, 0.5f},
                .tint = Vecf4{0.7f, 0.7f, 0.7f, 0.8f},
                .sampler = forge_cursor_smp,
            });
        }

        /* Scrollbar when the list runs past the box */
        if(rows > visible_rows)
        {
            f32 const track_top    = row_y(0);
            f32 const track_bottom = row_y(visible_rows);
            f32 const track        = track_top - track_bottom;
            f32 const thumb =
                track * static_cast<f32>(visible_rows) / static_cast<f32>(rows);
            f32 const top =
                track_top - (track - thumb) * static_cast<f32>(menu.scroll) /
                                static_cast<f32>(rows - visible_rows);
            f32 const x = origin.x + box_width - box_margin / 2 - 2;
            extra_quads.push_back({
                .position = Vecf2{x, top - thumb},
                .size = Vecf2{4, thumb},
                .atlas_offset = Vecf2{0.22f, 0.22f},
                .atlas_scale = Vecf2{0.02f, 0.02f},
                .tint = Vecf4{0.7f, 0.9f, 1.f, 0.7f},
                .sampler = forge_cursor_smp,
            });
        }
    }

    void load_resources(gfx::api& api)
    {
        std::vector<u8> backing_store;
        auto const& desc = blam::loading::forge_ui_desc;
        auto data = gsl::span(
            blam::loading::forge_ui_data.data(),
            blam::loading::forge_ui_data.size());
        auto fmt = pix_fmt::R8;
        if(!api.feature_info().texture.swizzle)
        {
            // WebGL in particular does not have swizzle
            // So downgrade to RGBA8 here instead
            fmt = pix_fmt::RGBA8;
            for(auto px : data)
                for(auto _ : stl_types::range<>(4))
                    backing_store.push_back(px);
            data = gsl::span(backing_store.data(), backing_store.size());
        }
        forge_cursor = api.alloc_texture(gfx::textures::d2, fmt, 1);
        forge_cursor->alloc(size_3d<u32>(desc.width, desc.height, 1));
        forge_cursor->upload(data, Veci2{0, 0}, size_2d<i32>(desc.width, desc.height));
        if(api.feature_info().texture.swizzle)
            forge_cursor->set_swizzle(
                gfx::textures::swizzle_t::red,
                gfx::textures::swizzle_t::red,
                gfx::textures::swizzle_t::red,
                gfx::textures::swizzle_t::red);
        forge_cursor_smp = forge_cursor->sampler();
        forge_cursor_smp->alloc();
    }

    std::shared_ptr<gfx::texture_2d_t> forge_cursor;
    std::shared_ptr<gfx::sampler_t>    forge_cursor_smp;

    u32                                                m_catalog_generation{0};
    std::vector<group_t>                               m_groups;
    std::map<blam::tag_t const*, std::u16string>       m_folders;
    std::map<u64, menu_t>                              m_menus;
    std::map<u64, buttons_t>                           m_buttons;
};

void alloc_forge_controller(compo::EntityContainer &e)
{
    e.register_subsystem_inplace<ForgeController>();
}
