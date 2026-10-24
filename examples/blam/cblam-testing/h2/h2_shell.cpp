#include "h2_shell_data.h"
#include "../world/player_spawn.h"

#include "components.h"
#include "data.h"
#include "resource_creation.h"

#include <coffee/comp_app/services.h>
#include "shader_compiler.h"
#include "ui.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/CDebug>
#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_buffer.h>
#include <coffee/graphics/apis/gleam/rhi_draw_command.h>
#include <coffee/graphics/apis/gleam/rhi_program.h>
#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/graphics/apis/gleam/rhi_vertex.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>

namespace h2 {

using blam::from_le;

using H2ShellManifest = compo::SubsystemManifest<
    compo::type_list_t<PlayerCamera, PlayerInput, const PlayerInfo>,
    compo::type_list_t<const H2ShellData, gfx::system, UIEventBus, RenderingParameters>,
    compo::type_list_t<comp_app::GraphicsFramebuffer>>;

/* The main menu: a stack of screens over the scripted camera flythrough,
 * drawn in Halo's 640x480 space after the world */
struct H2Shell : compo::RestrictedSubsystem<H2Shell, H2ShellManifest>
{
    using Proxy = compo::proxy_of<H2ShellManifest>;
    using type  = H2Shell;

    struct Vertex
    {
        Vecf2 position;
        Vecf2 texcoord;
        Vecf4 color;
    };

    struct Quad
    {
        i32                                depth;
        bool                               additive;
        std::shared_ptr<gfx::texture_2d_t> texture;
        std::shared_ptr<gfx::sampler_t>    sampler;
        std::array<Vertex, 6>              vertices;
    };

    struct ScreenState
    {
        ShellScreen const* screen;
        f32                opened_at;
        i32                selected{0};
        std::vector<u32>   items; /*!< Indices into the screen's strings */
    };

    H2Shell(compo::EntityContainer& e)
        : m_container(e)
    {
        /* Drawn in end_frame after the postprocess, like the Halo 1 shell:
         * overlays need a priority under ScreenClear's 899 there */
        priority = 850;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
        auto const& data = p.template subsystem<H2ShellData>();
        f32 now = std::chrono::duration<f32>(t.time_since_epoch()).count();
        if(m_start < 0.f)
            m_start = now;
        m_time = now - m_start;
        if(data.generation != m_generation)
        {
            m_generation = data.generation;
            m_stack.clear();
            m_opened = m_time;
            if(data.loaded)
            {
                open(data, data.screen(ui::screen_id_t::press_start_intro));
                p.template subsystem<RenderingParameters>().render_ui = true;
            }
        }
        if(!data.loaded || m_stack.empty())
            return;
        subscribe(p);
        ensure_gpu();

        for(auto e : p.select<PlayerCamera, PlayerInput, PlayerInfo>())
        {
            auto [cam, input, info] = e.components();
            if(!world::is_local_seat(info))
                continue;
            input.input_mode = PlayerInput::input_mode_t::menu;
            fly_camera(data, cam);
            break;
        }
        m_nav_queue->poll();
        for(auto const& nav : m_nav_events)
            navigate(data, nav);
        m_nav_events.clear();
    }

    void end_restricted(Proxy& p, time_point const&)
    {
        auto const& data = p.template subsystem<H2ShellData>();
        /* Hidden, the camera still flies */
        if(!data.loaded || m_stack.empty() || !m_api ||
           !p.template subsystem<RenderingParameters>().render_ui)
            return;
        auto* fb = p.template service<comp_app::GraphicsFramebuffer>();
        if(!fb)
            return;
        Veci2 size{static_cast<i32>(fb->size().w), static_cast<i32>(fb->size().h)};
        m_quads.clear();
        f32 aspect = static_cast<f32>(size.x) / static_cast<f32>(std::max(1, size.y));
        Vecf2 screen_scale{(4.f / 3.f) / aspect / 320.f, 1.f / 240.f};

        /* The world under the shell is tinted, then every screen in the
         * stack over the background */
        f32 half_width = 320.f * aspect / (4.f / 3.f);
        push_quad(
            -1000,
            m_white,
            m_white_sampler,
            {-half_width, 240.f},
            {2.f * half_width, 480.f},
            {0.f, 0.f, 1.f, 1.f},
            data.overlay_color);
        /* The background's header framing goes with the header: screens
         * flagged "no header text" (start, main menu) show only its strips */
        m_frame_bars = !(m_stack.back().screen->flags & 0x4);
        if(auto const* bg = data.screen(ui::screen_id_t::game_shell_background))
            draw_screen(data, *bg, m_opened, -1, {}, false);
        m_frame_bars = true;
        for(size_t i = 0; i < m_stack.size(); i++)
        {
            auto const& state = m_stack[i];
            bool const  top   = i + 1 == m_stack.size();
            draw_screen(
                data, *state.screen, state.opened_at, state.selected, top ? state.items : std::vector<u32>{}, top);
        }
        submit(size, screen_scale);
    }

  private:
    void subscribe(Proxy& p)
    {
        if(m_subscribed)
            return;
        m_subscribed = true;
        m_nav_queue =
            p.template subsystem<UIEventBus>().template addQueuedEventFunction<UINavigation>(
                0, [this](UIEvent&, UINavigation* nav) {
                    m_nav_events.push_back(*nav);
                });
    }

    void open(H2ShellData const& data, ShellScreen const* screen)
    {
        if(!screen)
            return;
        ScreenState state{.screen = screen, .opened_at = m_time};
        switch(screen->id)
        {
        case ui::screen_id_t::main_menu:
            /* Campaign .. Settings; the sign-out entry needs a profile */
            for(u32 i = 0; i + 1 < screen->strings.size() && i < 5; i++)
                state.items.push_back(i);
            break;
        case ui::screen_id_t::player_profile_select:
            if(screen->strings.size() > 17)
                state.items.push_back(17); /* Create New Profile */
            break;
        default:
            break;
        }
        m_stack.push_back(std::move(state));
        cDebug("H2: shell: {} opened", screen->name);
        (void)data;
    }

    void navigate(H2ShellData const& data, UINavigation const& nav)
    {
        if(m_stack.empty())
            return;
        auto& top = m_stack.back();
        switch(nav.action)
        {
        case UINavigation::up:
            if(!top.items.empty())
                top.selected = (top.selected + static_cast<i32>(top.items.size()) - 1) %
                               static_cast<i32>(top.items.size());
            break;
        case UINavigation::down:
            if(!top.items.empty())
                top.selected = (top.selected + 1) % static_cast<i32>(top.items.size());
            break;
        case UINavigation::back:
            if(m_stack.size() > 1)
            {
                cDebug("H2: shell: {} closed", top.screen->name);
                m_stack.pop_back();
            }
            break;
        case UINavigation::open:
        case UINavigation::close:
        case UINavigation::accept:
            if(top.screen->id == ui::screen_id_t::press_start_intro)
            {
                m_stack.pop_back();
                open(data, data.screen(ui::screen_id_t::main_menu));
            } else if(top.screen->id == ui::screen_id_t::main_menu)
                open(data, data.screen("profile_picker\\profile_picker"));
            break;
        default:
            break;
        }
    }

    /* The scripted mainmenu_flythrough: a leg per camera point, the
     * script's sleeps as the leg times, looping back to the first */
    void fly_camera(H2ShellData const& data, PlayerCamera& cam)
    {
        auto const& path = data.camera_path;
        if(path.size() < 2)
            return;
        static constexpr f32 ticks = 30.f;
        std::vector<f32>     legs;
        for(size_t i = 0; i < path.size(); i++)
            legs.push_back((i < 9 ? 250.f : i < 12 ? 200.f : 300.f) / ticks);
        f32 total = 0.f;
        for(f32 l : legs)
            total += l;
        f32 t = std::fmod(m_time, total);
        size_t leg = 0;
        while(leg + 1 < legs.size() && t >= legs[leg])
            t -= legs[leg++];
        f32 a = std::clamp(t / legs[leg], 0.f, 1.f);
        a     = a * a * (3.f - 2.f * a);
        auto const& from = path[leg];
        auto const& to   = path[(leg + 1) % path.size()];
        Vecf3 position = glm::mix(from.position, to.position, a);
        auto  lerp_angle = [](f32 x, f32 y, f32 a) {
            f32 d = std::remainder(y - x, 2.f * glm::pi<f32>());
            return x + d * a;
        };
        f32 yaw   = lerp_angle(from.orientation.x, to.orientation.x, a);
        f32 pitch = lerp_angle(from.orientation.y, to.orientation.y, a);
        Vecf3 forward{
            std::cos(yaw) * std::cos(pitch),
            std::sin(yaw) * std::cos(pitch),
            std::sin(pitch)};
        /* PlayerController folds the vertex->BSP basis into the view:
         * view = R * basis * T(-eye), so R = lookAt's rotation * basis^T */
        static const glm::mat3 basis{{0, 0, 1}, {1, 0, 0}, {0, 1, 0}};
        glm::mat3 look = glm::mat3(
            glm::lookAt(position, position + forward, Vecf3{0.f, 0.f, 1.f}));
        cam.camera.position = position;
        cam.camera.rotation = glm::quat_cast(look * glm::transpose(basis));
    }

    /* Alpha and offset of an animation at `age` seconds */
    std::pair<f32, Vecf2> animate(H2ShellData const& data, u16 index, f32 age) const
    {
        if(index == 0 || index > data.animations.size())
            return {1.f, {}};
        auto const& anim = data.animations[index - 1];
        auto        at   = [](std::vector<ShellKeyframe> const& frames, f32 u) {
            if(frames.empty())
                return std::pair<f32, Vecf2>{1.f, {}};
            if(frames.size() == 1)
                return std::pair<f32, Vecf2>{frames[0].alpha, Vecf2(frames[0].position)};
            f32    s = std::clamp(u, 0.f, 1.f) * static_cast<f32>(frames.size() - 1);
            size_t i = std::min(static_cast<size_t>(s), frames.size() - 2);
            f32    f = s - static_cast<f32>(i);
            return std::pair<f32, Vecf2>{
                glm::mix(frames[i].alpha, frames[i + 1].alpha, f),
                glm::mix(Vecf2(frames[i].position), Vecf2(frames[i + 1].position), f)};
        };
        f32 intro = static_cast<f32>(anim.intro_ms) / 1000.f;
        if(age < intro && !anim.intro.empty())
            return at(anim.intro, age / intro);
        if(anim.ambient_ms > 1 && anim.ambient.size() > 1)
        {
            f32 period = static_cast<f32>(anim.ambient_ms) / 1000.f;
            f32 u      = std::fmod(age - intro, period) / period;
            if(anim.ambient_loop == 1)
                u = u < 0.5f ? u * 2.f : 2.f - u * 2.f;
            return at(anim.ambient, u);
        }
        return anim.intro.empty() ? std::pair<f32, Vecf2>{1.f, {}}
                                  : std::pair<f32, Vecf2>{anim.intro.back().alpha, {}};
    }

    void draw_screen(
        H2ShellData const&      data,
        ShellScreen const&      screen,
        f32                     opened_at,
        i32                     selected,
        std::vector<u32> const& items,
        bool                    top = true)
    {
        f32 age = m_time - opened_at;
        for(auto const& pane : screen.panes)
        {
            auto [pane_alpha, pane_offset] = animate(data, pane.animation, age);
            for(auto const& b : pane.bitmaps)
            {
                if(b.bitmap < 0)
                    continue;
                if(!m_frame_bars && b.wraps_per_second == Vecf2{0.f, 0.f})
                    continue;
                auto [alpha, offset] = animate(data, b.animation, age);
                draw_bitmap(data.bitmaps[b.bitmap], b, pane_offset + offset, alpha * pane_alpha);
            }
            for(auto const& t : pane.texts)
            {
                auto [alpha, offset] = animate(data, t.animation, age);
                draw_text(data, t, t.text, pane_offset + offset, alpha * pane_alpha);
            }
            for(auto const& t : pane.buttons)
            {
                auto [alpha, offset] = animate(data, t.animation, age);
                draw_text(data, t, t.text, pane_offset + offset, alpha * pane_alpha);
            }
            for(auto const& list : pane.lists)
            {
                if(!top || list.skin >= data.skins.size())
                    continue;
                auto const& skin = data.skins[list.skin];
                auto [alpha, offset] = animate(data, list.animation, age);
                i32 row = skin.texts.empty()
                              ? 20
                              : from_le(skin.texts[0].bounds.top) -
                                    from_le(skin.texts[0].bounds.bottom);
                for(size_t i = 0; i < items.size(); i++)
                {
                    Vecf2 at = pane_offset + offset +
                               Vecf2(
                                   static_cast<f32>(from_le(list.bottom_left.x)),
                                   static_cast<f32>(from_le(list.bottom_left.y)) -
                                       static_cast<f32>(row * static_cast<i32>(i)));
                    bool const is_selected = static_cast<i32>(i) == selected;
                    f32        item_alpha  = is_selected ? 1.f : 0.5f;
                    if(is_selected)
                        for(auto const& b : skin.bitmaps)
                            if(b.bitmap >= 0)
                                draw_bitmap(data.bitmaps[b.bitmap], b, at, alpha * pane_alpha);
                    auto const& text = items[i] < screen.strings.size()
                                           ? screen.strings[items[i]]
                                           : std::string();
                    for(auto const& t : skin.texts)
                        draw_text(data, t, text, at, alpha * pane_alpha * item_alpha);
                }
            }
        }
        /* Header and button legend from the shared globals */
        bool const dialog = (screen.flags & 0x9) != 0;
        if(!screen.header.empty() && !(screen.flags & 0x4))
            draw_text(
                data,
                ShellText{
                    .font   = data.header_font,
                    .color  = data.default_text_color,
                    .bounds = dialog ? data.half_header_bounds : data.header_bounds,
                    .depth  = 20},
                screen.header,
                {},
                1.f);
        auto key = static_cast<u32>(screen.button_key);
        if(key > 0 && key <= data.button_key_strings.size())
            draw_text(
                data,
                ShellText{
                    .font   = 1,
                    .color  = data.default_text_color,
                    .bounds = dialog ? data.half_button_key_bounds : data.button_key_bounds,
                    .depth  = 20},
                data.button_key_strings[key - 1],
                {},
                1.f);
    }

    void draw_bitmap(
        ShellBitmap const& bitmap, ShellBitmapBlock const& block, Vecf2 const& offset, f32 alpha)
    {
        if(!bitmap.texture.texture || alpha <= 0.f)
            return;
        Vecf2 at(
            static_cast<f32>(from_le(block.top_left.x)) + offset.x,
            static_cast<f32>(from_le(block.top_left.y)) + offset.y);
        Vecf2 size{static_cast<f32>(bitmap.width), static_cast<f32>(bitmap.height)};
        /* Scrolling strips wrap around the screen */
        f32   u0 = m_time * block.wraps_per_second.x;
        f32   v0 = m_time * block.wraps_per_second.y;
        Vecf4 uv{u0, v0, u0 + 1.f, v0 + 1.f};
        push_quad(
            block.depth,
            bitmap.texture.texture,
            bitmap.texture.sampler,
            at,
            size,
            uv,
            Vecf4{alpha, alpha, alpha, alpha},
            block.blend == 0);
    }

    static std::u32string decode_utf8(std::string_view s)
    {
        std::u32string out;
        for(size_t i = 0; i < s.size();)
        {
            u8  c = static_cast<u8>(s[i]);
            u32 cp;
            int n;
            if(c < 0x80)
                cp = c, n = 1;
            else if((c & 0xE0) == 0xC0)
                cp = c & 0x1F, n = 2;
            else if((c & 0xF0) == 0xE0)
                cp = c & 0x0F, n = 3;
            else
                cp = c & 0x07, n = 4;
            for(int k = 1; k < n && i + k < s.size(); k++)
                cp = (cp << 6) | (static_cast<u8>(s[i + k]) & 0x3F);
            out.push_back(cp);
            i += n;
        }
        return out;
    }

    void draw_text(
        H2ShellData const& data,
        ShellText const&   t,
        std::string const& text,
        Vecf2 const&       offset,
        f32                alpha)
    {
        if(text.empty() || t.font >= data.fonts.size() || !data.fonts[t.font])
            return;
        auto const& font = *data.fonts[t.font];
        auto        cps  = decode_utf8(text);
        f32         width = 0.f;
        for(size_t i = 0; i < cps.size(); i++)
            if(auto const* g = font.glyph(cps[i]))
            {
                width += g->advance;
                if(i + 1 < cps.size())
                    width += font.kern(cps[i], cps[i + 1]);
            }
        f32 left   = from_le(t.bounds.left);
        f32 right  = from_le(t.bounds.right);
        f32 top    = from_le(t.bounds.top);
        f32 bottom = from_le(t.bounds.bottom);
        f32 x = (t.flags & 1)   ? left
                : (t.flags & 2) ? right - width
                                : (left + right - width) / 2.f;
        f32 text_h   = static_cast<f32>(font.ascent + font.descent);
        f32 baseline = bottom + ((top - bottom) - text_h) / 2.f +
                       static_cast<f32>(font.descent);
        /* Clear text blocks are captions the game fills in; nothing to
         * show until then */
        if(t.color.a <= 0.f)
            return;
        Vecf4 color = t.color;
        if(t.flags & 4)
            alpha *= 0.7f + 0.3f * std::sin(m_time * 4.f);
        color.a *= alpha;
        for(size_t i = 0; i < cps.size(); i++)
        {
            auto const* g = font.glyph(cps[i]);
            if(!g)
                continue;
            Vecf2 at{
                x + g->origin.x + offset.x,
                baseline + g->origin.y + offset.y};
            push_quad(t.depth, font.atlas, font.sampler, at, g->size, g->uv, color);
            x += g->advance;
            if(i + 1 < cps.size())
                x += font.kern(cps[i], cps[i + 1]);
        }
    }

    /* A quad with its top-left at `at`, extending right and down */
    void push_quad(
        i32                                       depth,
        std::shared_ptr<gfx::texture_2d_t> const& texture,
        std::shared_ptr<gfx::sampler_t> const&    sampler,
        Vecf2 const&                              at,
        Vecf2 const&                              size,
        Vecf4 const&                              uv,
        Vecf4 const&                              color,
        bool                                      additive = false)
    {
        Vertex tl{{at.x, at.y}, {uv.x, uv.y}, color};
        Vertex tr{{at.x + size.x, at.y}, {uv.z, uv.y}, color};
        Vertex bl{{at.x, at.y - size.y}, {uv.x, uv.w}, color};
        Vertex br{{at.x + size.x, at.y - size.y}, {uv.z, uv.w}, color};
        m_quads.push_back(Quad{depth, additive, texture, sampler, {tl, bl, br, tl, br, tr}});
    }

    void submit(Veci2 const& size, Vecf2 const& screen_scale)
    {
        if(m_quads.empty())
            return;
        std::stable_sort(m_quads.begin(), m_quads.end(), [](Quad const& a, Quad const& b) {
            return a.depth < b.depth;
        });
        m_vertices.clear();
        for(auto const& q : m_quads)
            m_vertices.insert(m_vertices.end(), q.vertices.begin(), q.vertices.end());
        m_vertex_buf->commit(Span<const Vertex>(m_vertices.data(), m_vertices.size()));
        for(size_t i = 0; i < m_quads.size();)
        {
            size_t j = i + 1;
            while(j < m_quads.size() && m_quads[j].texture == m_quads[i].texture &&
                  m_quads[j].additive == m_quads[i].additive)
                j++;
            auto res_ = m_api->submit(
                gfx::draw_command{
                    .program  = m_program,
                    .vertices = m_vao,
                    .call =
                        {
                            .instanced = false,
                            .mode      = gfx::drawing::primitive::triangle,
                        },
                    .data = {{.arrays = {
                                  .count  = static_cast<u32>((j - i) * 6),
                                  .offset = static_cast<u32>(i * 6)}}},
                },
                gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair<Vecf2 const>{
                        {"screen_scale"}, semantic::SpanOne<const Vecf2>(screen_scale)}),
                gfx::make_sampler_list(gfx::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"image", 0},
                    m_quads[i].sampler,
                }),
                gfx::view_state{
                    .view = typing::vector_types::Veci4{0, 0, size.x, size.y},
                },
                gfx::blend_state{.additive = m_quads[i].additive});
            if(res_.has_value() && !m_draw_error_logged)
            {
                cWarning("H2: shell: failed to draw: {}", std::get<1>(*res_));
                m_draw_error_logged = true;
            }
            i = j;
        }
    }

    void ensure_gpu()
    {
        if(m_api)
            return;
        m_api     = &m_container.subsystem_cast<gfx::system>();
        auto& api = *m_api;
        create_program(
            api,
            shader_pair_t{
                .vertex_file   = "h2_ui",
                .fragment_file = "h2_ui",
                .shader        = m_program,
            });
        m_vao = api.alloc_vertex_array();
        m_vao->alloc();
        m_vao->add(gfx::vertex_attribute{
            .index = 0,
            .value = {.offset = 0, .stride = sizeof(Vertex), .count = 2},
        });
        m_vao->add(gfx::vertex_attribute{
            .index = 1,
            .value = {.offset = offsetof(Vertex, texcoord), .stride = sizeof(Vertex), .count = 2},
        });
        m_vao->add(gfx::vertex_attribute{
            .index = 2,
            .value = {.offset = offsetof(Vertex, color), .stride = sizeof(Vertex), .count = 4},
        });
        m_vao->set_attribute_names({{"position", 0}, {"texcoord", 1}, {"color", 2}});
        m_vertex_buf = api.alloc_buffer(
            gfx::buffers::vertex, semantic::RSCA::WriteOnly | semantic::RSCA::Streaming);
        m_vertex_buf->alloc();
        m_vao->set_buffer(gfx::buffers::vertex, m_vertex_buf, 0);

        m_white = api.alloc_texture(
            gfx::textures::d2, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8), 1);
        m_white->alloc(size_3d<u32>{2, 2, 1});
        m_white->upload(std::vector<u8>(16, 0xFF), Veci2{0, 0}, Veci2{2, 2}, 0);
        m_white_sampler = m_white->sampler();
        m_white_sampler->alloc();
    }

    compo::EntityContainer& m_container;
    gfx::api*               m_api{nullptr};
    u32                     m_generation{0};
    f32                     m_start{-1.f}, m_time{0.f}, m_opened{0.f};
    bool                    m_subscribed{false};
    bool                    m_draw_error_logged{false};
    bool                    m_frame_bars{true};

    std::vector<ScreenState>  m_stack;
    std::vector<UINavigation> m_nav_events;
    std::shared_ptr<UIEventBus::queue_type<UINavigation>> m_nav_queue;

    std::shared_ptr<gfx::program_t>      m_program;
    std::shared_ptr<gfx::vertex_array_t> m_vao;
    std::shared_ptr<gfx::buffer_t>       m_vertex_buf;
    std::shared_ptr<gfx::texture_2d_t>   m_white;
    std::shared_ptr<gfx::sampler_t>      m_white_sampler;
    std::vector<Quad>                    m_quads;
    std::vector<Vertex>                  m_vertices;
};

void alloc_h2_shell(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<H2Shell>(std::ref(e));
}

} // namespace h2
