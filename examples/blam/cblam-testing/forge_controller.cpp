#include "forge_controller.h"

#include "components.h"
#include "crunched/loading_screen.h"
#include "render/rendering.h"

#include <coffee/components/restricted_subsystem.h>

using ForgeControllerManifest = compo::SubsystemManifest<
    type_list_t<
        const PhysicsData,
        const PlayerCamera,
        const PlayerInfo,
        PlayerInput
    >,
    type_list_t<
        gfx::system,
        PostProcessParameters,
        ScreenClear
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
        for(auto const& player : p.select<PlayerInfo, PlayerInput, PlayerCamera>())
        {
            auto [info, input, camera] = player.components();
            if(info.mode.physics ||
                    !camera.is_active() ||
                    !postproc->forge_overlay)
                continue;

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
            // Object picker box
            constexpr u32 box_size = 480;
            constexpr u32 box_width = 300;
            constexpr u32 element_size = 40;
            constexpr u32 padding = 4;
            constexpr u32 element_spacing = element_size + padding;
            constexpr u32 box_margin = 10;
            constexpr u32 screen_margin = 10;
            extra_quads.push_back({
                .position = Vecf2{
                    bounds.x + screen_margin,
                    bounds.y + screen_margin},
                .size = Vecf2{box_width, box_size},
                .atlas_offset = Vecf2{0.22f, 0.22f},
                .atlas_scale = Vecf2{0.02f, 0.02f},
                .tint = Vecf4{0, 0.75f, 1.f, 0.35f},
                .sampler = forge_cursor_smp,
            });
            // Object picker select
            Vecf2 select_box = Vecf2{
                bounds.x + screen_margin + box_margin,
                bounds.y + screen_margin + (
                    box_size - box_margin -
                    input.forge.nav_y * element_spacing
                ),
            };
            Vecf2 select_box_scale = Vecf2{
                box_width - box_margin * 2,
                element_size,
            };
            extra_quads.push_back({
                .position = select_box,
                .size = select_box_scale,
                .atlas_offset = Vecf2{0.22f, 0.22f},
                .atlas_scale = Vecf2{0.02f, 0.02f},
                .tint = Vecf4{0.9f, 0.2f, 0.f, 0.35f},
                .sampler = forge_cursor_smp,
            });
            extra_quads.push_back({
                .position = select_box + Vecf2{
                    box_width - box_margin * 2 - 36 - 2,
                    2 + 36,
                },
                .size = Vecf2{36, -36},
                .atlas_offset = Vecf2{0.5f, 0},
                .atlas_scale = Vecf2{0.5f, 0.5f},
                .tint = input.forge.accept ? Vecf4{1} : Vecf4{Vecf3{0.2f}, 1},
                .sampler = forge_cursor_smp,
            });
            extra_quads.push_back({
                .position = select_box + Vecf2{2, 2},
                .size = Vecf2{36, 36},
                .atlas_offset = Vecf2{0, 0},
                .atlas_scale = Vecf2{0.5f, 0.5f},
                .tint = Vecf4{0.7f, 0.7f, 0.7f, 0.8f},
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
};

void alloc_forge_controller(compo::EntityContainer &e)
{
    e.register_subsystem_inplace<ForgeController>();
}
