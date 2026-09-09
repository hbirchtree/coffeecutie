#pragma once

#include "coffee/graphics/apis/gleam/rhi_draw_command.h"
#include "coffee/graphics/apis/gleam/rhi_program.h"
#include "coffee/graphics/apis/gleam/rhi_vertex.h"
#include "crunched/crunch_types.h"
#include "crunched/loading_screen.h"
#include "peripherals/concepts/graphics_api.h"
#include "peripherals/stl/accumulate.h"
#include "peripherals/stl/enumerate.h"
#include "peripherals/stl/map_values.h"
#include "peripherals/typing/enum/graphics/shader_stage.h"
#include "peripherals/typing/geometry/rect.h"
#include "peripherals/typing/vectors/glm_vector_types.h"
#include "rendering.h"

#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <magic_enum/magic_enum.hpp>

#include <algorithm>

using semantic::RSCA;
using namespace std::string_view_literals;

void LoadingScreen::end_restricted(Proxy& e, const time_point& time)
{
    using namespace std::chrono_literals;

    gfx::system*   api;
    ScreenClear*   screen_clear;
    LoadingStatus* status;
    e.subsystem(api);
    e.subsystem(screen_clear);
    e.subsystem(status);

    if(status->progress < 0)
    {
        if(!was_loading)
            return;
        if(loading_screen_gone_time.has_value() &&
           time > *loading_screen_gone_time)
        {
            was_loading = false;
            loading_screen_gone_time.reset();
            return;
        }
        if(!loading_screen_gone_time.has_value())
            loading_screen_gone_time = time + 2s;
    }

    if(!loading_program)
        load_resources(*api);

    auto _ = api->debug().scope("LoadingScreen::end_restricted");

    auto screen_aspect =
        e.service<comp_app::GraphicsFramebuffer>()->size().aspect();
    f32 timef = std::chrono::duration_cast<stl_types::chrono::seconds_f32>(
                    time.time_since_epoch())
                    .count();

    // Matf4 transform = glm::rotate(
    //     glm::scale(
    //         glm::identity<Matf4>(),
    //         Vecf3{
    //             2 / (screen_aspect < 1 ? screen_aspect : 1),
    //             2 * (screen_aspect > 1 ? screen_aspect : 1),
    //             1}),
    //     0.f /*std::fmod(timef, glm::pi<f32>() * 2)*/,
    //     Vecf3{0, 0, 1});
    // Vecf2 bg_offset{0, std::fmod(timef, stl_types::math::pi_f * 4.f)};

    // f32 time_left =
    //     loading_screen_gone_time.has_value()
    //         ? std::chrono::duration_cast<stl_types::chrono::seconds_f32>(
    //               *loading_screen_gone_time - time)
    //               .count()
    //         : 2.f;
    // f32 curtain_fade = time_left / 2.f;

    // auto res = api->submit(
    //     gfx::draw_command{
    //         .program  = loading_bg_program,
    //         .vertices = quad_vao,
    //         .call =
    //             {
    //                 .indexed = false,
    //                 .mode    = gfx::drawing::primitive::triangle_fan,
    //             },
    //         .data = {{.arrays = {.count = 34, .offset = 4}}},
    //     },
    //     gfx::make_uniform_list(
    //         typing::graphics::ShaderStage::Vertex,
    //         gfx::uniform_pair{{"transform"sv}, semantic::SpanOne(transform)},
    //         gfx::uniform_pair{{"tex_offset"sv}, semantic::SpanOne(bg_offset)}),
    //     gfx::make_uniform_list(
    //         typing::graphics::ShaderStage::Fragment,
    //         gfx::uniform_pair{
    //             {"curtain_fade"sv}, semantic::SpanOne(curtain_fade)}),
    //     gfx::make_sampler_list(
    //         gleam::sampler_definition_t{
    //             typing::graphics::ShaderStage::Fragment,
    //             {"noise"},
    //             loading_sampler,
    //         }),
    //     gfx::blend_state{});

    render_ring_texture(*api);

    // Full-screen pass: radial blur over the baked ring, standing in for the
    // original's accumulation buffer. The quad's first four vertices are the
    // full-screen one, in 0..1, so map them onto clip space.
    Matf4 zoom_transform = glm::scale(
        glm::translate(glm::identity<Matf4>(), Vecf3{-1, -1, 0}),
        Vecf3{2, 2, 1});
    Vecf2 zoom_offset{0.f, 0.f};
    // The original swept the convergence point slowly side to side; timef is
    // still seconds here, before the spinner reuses it. Its resting place was
    // measured off loading.dds at (0.95, 0.20).
    constexpr f32   sweep_period = 12.f;   // seconds for a full cycle
    constexpr f32   sweep_extent = 0.5f;  // how far either side of rest
    constexpr Vecf2 sweep_rest{0.5f, 0.5f};
    Vecf2           zoom_center{
        sweep_rest.x +
            sweep_extent *
                std::sin(timef * (stl_types::math::pi_f * 2.f / sweep_period)),
        sweep_rest.y};
    f32 zoom_strength{0.25f};
    f32 zoom_exposure{2.5f};
    // An ellipse rather than a disc: narrow across x so the sweep reads as a
    // travelling band, tall enough in y to reach past both screen edges. The
    // 0.6 exponent is the decay measured off the original.
    Vecf2 falloff_extent{0.1f, 1.f};
    f32   falloff_power{0.6f};

    auto zoom_res = api->submit(
        gfx::draw_command{
            .program  = zoom_program,
            .vertices = quad_vao,
            .call =
                {
                    .indexed = false,
                    .mode    = gfx::drawing::primitive::triangle_fan,
                },
            .data = {{.arrays = {.count = 4}}},
        },
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"transform"sv}, semantic::SpanOne(zoom_transform)},
            gfx::uniform_pair{{"tex_offset"sv}, semantic::SpanOne(zoom_offset)}),
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{{"zoom_center"sv}, semantic::SpanOne(zoom_center)},
            gfx::uniform_pair{
                {"zoom_strength"sv}, semantic::SpanOne(zoom_strength)},
            gfx::uniform_pair{{"exposure"sv}, semantic::SpanOne(zoom_exposure)},
            gfx::uniform_pair{
                {"falloff_extent"sv}, semantic::SpanOne(falloff_extent)},
            gfx::uniform_pair{
                {"falloff_power"sv}, semantic::SpanOne(falloff_power)}),
        gfx::make_sampler_list(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source"},
                ring_color_sampler,
            }),
        gfx::blend_state{});
    if(zoom_res)
    {
        auto [err, msg] = *zoom_res;
        cWarning(
            "Failed to draw loading zoom: {}: {}",
            gleam::detail::draw_error_to_string(err),
            msg);
    }

    if(status->progress < 0)
        return;

    was_loading = true;

    Matf4 transform = glm::translate(
        // glm::scale(glm::identity<Matf4>(), glm::vec3(0.2f)),
        glm::scale(
            glm::identity<Matf4>(), glm::vec3(0.2f / screen_aspect, 0.2f, 1)),
        glm::vec3(1 / (0.2f / screen_aspect) - 1, -5, 0));
    timef     = std::fmod(timef, 10.f) / 10.f;
    f32 start = timef;
    f32 end   = std::fmod(timef + 0.2f, 1.f);

    auto res = api->submit(
        gfx::draw_command{
            .program  = loading_program,
            .vertices = quad_vao,
            .call =
                {
                    .indexed = false,
                    .mode    = gfx::drawing::primitive::triangle_fan,
                },
            .data = {{.arrays = {.count = 4}}},
        },
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{{"transform"sv}, semantic::SpanOne(transform)}),
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{{"range_start"sv}, semantic::SpanOne(start)},
            gfx::uniform_pair{{"range_end"sv}, semantic::SpanOne(end)}),
        gfx::blend_state{});
    if(res)
    {
        auto [err, msg] = *res;
        cWarning(
            "Failed to draw loading screen: {}: {}",
            gleam::detail::draw_error_to_string(err),
            msg);
    }
}

void LoadingScreen::load_resources(gleam::system& api)
{
    using vecb4 = typing::vectors::tvector<libc_types::i8, 4>;
    using vecb2 = typing::vectors::tvector<libc_types::i8, 2>;

    auto __ = api.debug().scope("LoadingScreen::load_resources");

    ProfContext _;

    struct vertex_t
    {
        vecb2 pos;
        vecb2 tex;
    };

    quad_vbo = api.alloc_buffer(gfx::buffers::vertex, RSCA::ReadOnly);
    quad_vbo->alloc();
    std::vector<vecb4> vbo = {
        // Full-screen quad
        vecb4{0, 0, 0, 0},
        vecb4{127, 0, 127, 0},
        vecb4{127, 127, 127, 127},
        vecb4{0, 127, 0, 127},

        // Zip-zoomy loading background
        vecb4{0, 0, 0, 0},
        // The rest of the vertices are generated
    };
    for(auto i : range<>(32))
    {
        f32            x_1 = stl_types::math::pi_f * 2.f * (i / 32.f);
        libc_types::i8 phi = glm::cos(x_1) * 127, theta = glm::sin(x_1) * 127;
        vbo.push_back(vecb4{phi, theta, 127 * (((i % 2) == 0) ? 1 : -1), 127});
    }
    vbo.push_back(vecb4{glm::cos(0) * 127, glm::sin(0) * 127, 127, 127});

    quad_vbo->commit(vbo);
    quad_vao = api.alloc_vertex_array();
    quad_vao->alloc();
    quad_vao->add(
        gfx::vertex_attribute::from_member(
            &vertex_t::pos, gfx::vertex_float_type));
    auto tex = gfx::vertex_attribute::from_member(
        &vertex_t::tex, gfx::vertex_float_type);
    tex.index = 1;
    quad_vao->add(tex);
    quad_vao->set_buffer(gfx::buffers::vertex, quad_vbo, 0);
    quad_vao->set_attribute_names({
        {"pos", 0},
        {"tex", 1},
    });
    quad_vao->force_attribute_names();

    // 32x32 noise texture :)
    constexpr std::string_view noise_tex =
        "M,#QAI+#;GJK?(BYBY?(E:'2FJ;7EJ+3@(R]9W.D86V><W^PAY/$H*S=F:76J+3E"
        "E*#1G*C9J+3EO\\O\\K;GJI+#AN,3UP,S]HJ[?A9'\"AI+#FZ?8K[OLEZ/4AY/$D)S-"
        "N\\?XE:'2B97&H*S=JK;GH:W>FJ;7FZ?8EZ/4AI+#?XN\\BI;'A9'\"B97&@(R]B97&"
        ">X>X@X^`F:76LK[OJ+3EIK+CM<'RO,CYL;WNE*#1EZ/4ML+SPL[_J+3EG:G:J+3E"
        "M\\/TG:G:E:'2HZ_@K;GJH*S=EJ+3E*#1E:'2F:76E*#1H*S=E:'2BY?(?XN\\@X^`"
        ">H:W?(BYCYO,J+3EK;GJJ+3EI+#AI[/DI+#ABI;'DI[/ML+SQ-$!K+CIF:76E*#1"
        "D)S-C9G*EJ+3F:76G*C9DY_0DI[/F:76HZ_@L;WNJ[?HL[_PK+CIFJ;7BI;'@8V^"
        "?XN\\AY/$F:76JK;GKKKKG:G:E*#1CIK+AI+#=8&R?XN\\H*S=M\\/TJ+3EF:76A9'\""
        "A)#!DI[/I+#AI;'BIK+CG*C9G:G:I+#ALK[OM\\/TL;WNK[OLL[_PI;'BG:G:AY/$"
        "AY/$DY_0EJ+3G:G:G*C9CIK+C9G*BI;'>H:W<GZO>86VD)S-HZ_@GJK;AI+#<W^P"
        "F*35FZ?8I+#AI+#AK+CII[/DL[_PM,#QL[_PJK;GH*S=H:W>I;'BFJ;7EJ+3AY/$"
        "C)C)DY_0FJ;7I;'BG:G:C)C)AY/$C9G*@8V^A9'\"B)3%G:G:J+3EHJ[?AI+#>X>X"
        "KKKKFZ?8G*C9GZO<I+#AHJ[?J[?HI[/DH*S=I+#AG:G:FJ;7D)S-CYO,FJ;7DY_0"
        "EZ/4D9W.EZ/4H*S=H:W>E:'2B97&C)C)A9'\"E:'2E:'2FZ?8EZ/4D)S-@(R]A9'\""
        "GJK;CIK+EJ+3F:76J+3EIK+CIK+CH:W>FJ;7I;'BHZ_@G*C9BI;'B)3%B97&A)#!"
        "B97&B)3%EZ/4GJK;I+#AH:W>D9W.@8V^?(BYC9G*E*#1E:'2E:'2C9G*@(R]B)3%"
        "H*S=BI;'B97&C9G*G:G:H*S=KKKKKKKKG*C9EJ+3EJ+3G:G:F:76G:G:D9W.C9G*"
        "DI[/EJ+3I[/DF:76EJ+3E*#1EJ+3A9'\"A)#!D9W.E:'2F:76IK+CG:G:E:'2I+#A"
        "IK+CE*#1C9G*C)C)GZO<GJK;J[?HI[/DEZ/4BI;'A)#!BY?(FZ?8J+3EG:G:FJ;7"
        "G*C9HZ_@K;GJCYO,@(R]@X^`D9W.C)C)D9W.E:'2DI[/G*C9J;7FJ;7FHZ_@K[OL"
        "BY?(D)S-F*35BI;'DI[/GJK;LK[OJ;7FD9W.A9'\"<GZO<'RM@8V^C9G*BI;'CYO,"
        "GJK;K;GJKKKKB97&>86VA)#!DY_0EZ/4FJ;7GJK;F:76I;'BJ;7FI+#AJ;7FL;WN"
        "=X.TC9G*DI[/B97&BY?(F:76IK+CFJ;7BY?(CIK+=(\"Q:G:G=(\"Q>H:W@(R]B)3%"
        "DY_0G:G:FJ;7?XN\\?(BYE*#1HZ_@H:W>FZ?8HJ[?G:G:HJ[?G:G:E*#1E:'2G:G:"
        ";'BIBY?(DY_0G:G:G:G:H:W>GZO<EZ/4DY_0FJ;7?8FZ;7FJ;WNL>(2UAY/$CYO,"
        "F:76HZ_@H*S=@HZ_=8&R@8V^CIK+CIK+E*#1H*S=J;7FL;WNJ[?HG:G:E*#1F*35"
        ";GJKE:'2I;'BM<'RK;GJH:W>H:W>HJ[?G:G:EJ+3@8V^>86V=X.T@8V^C)C)F:76"
        "K;GJML+SI;'BA)#!;GJK<'RM?HJ[@(R]C)C)G*C9G*C9K+CIN\\?XL[_PH*S=HZ_@"
        "<'RME*#1K;GJN<7VL+SMI+#AHZ_@GZO<EJ+3C9G*BY?(DY_0D9W.FJ;7EZ/4E:'2"
        "G:G:J;7FEZ/4A9'\"<7VN:W>H>X>X@8V^E*#1GJK;EZ/4J+3EML+SK;GJF*35EZ/4"
        "@(R]F:76K[OLNL;WML+SI[/DKKKKJ+3EF:76DI[/E*#1DY_0C9G*C9G*B97&CYO,"
        "C9G*B97&?8FZ?8FZ?(BY?(BYD)S-D9W.HZ_@GZO<C9G*CYO,FJ;7G*C9CIK+F:76"
        "@8V^C)C)HZ_@IK+CHJ[?G*C9K;GJI[/DCYO,BI;'E*#1C)C)AI+#?XN\\@X^`E*#1"
        "E*#1A9'\"?(BYAI+#C)C)D9W.IK+CI;'BK[OLHZ_@CYO,A9'\"A)#!AY/$A9'\"FZ?8"
        "<7VN@X^`I+#AI[/DHZ_@E*#1EZ/4CYO,A9'\"CYO,I;'BDI[/C)C)@(R]A)#!BI;'"
        "B97&AY/$AY/$FZ?8F*35F*35I[/DK+CIJK;GDY_0@X^`@8V^=8&R=(\"Q<W^P@X^`"
        "7VN<>(2UGJK;J+3EI;'BDI[/BI;'AI+#B97&F:76K+CIEZ/4DI[/BI;'E*#1C)C)"
        "AY/$B)3%D)S-J[?HIK+CG*C9FZ?8I[/DJ[?HD9W.@(R]>H:W<W^P>H:W?8FZAI+#"
        "6&25;7FJC)C)F*35H:W>D)S-B)3%BI;'E*#1F:76IK+CE:'2F:76D)S-DI[/BI;'"
        "BY?(E:'2FZ?8J+3EHZ_@I+#AFJ;7HJ[?LK[OF:76AY/$@(R]A9'\"EJ+3DI[/AI+#"
        "7&B9=8&RBY?(C)C)D)S-CIK+C)C)B)3%D9W.D9W.CIK+AY/$DY_0EZ/4E*#1EZ/4"
        "GJK;J[?HK;GJHZ_@FJ;7GJK;FZ?8HJ[?L;WNG*C9D)S-AI+#BY?(EJ+3D9W.AI+#"
        "<'RMAY/$DY_0DY_0DY_0EZ/4EJ+3E*#1FZ?8D)S-B97&CIK+FZ?8I+#AEJ+3EJ+3"
        "F*35G*C9H*S=D)S-@HZ_AI+#CIK+GZO<IK+CG*C9FZ?8E*#1F:76FJ;7F:76DY_0"
        "A9'\"D9W.GZO<J;7FJ;7FJK;GI[/DEZ/4CYO,B97&F:76I;'BJK;GI[/DD9W.DY_0"
        "EZ/4DI[/E:'2D)S-?XN\\>H:W>(2UA9'\"DI[/FZ?8FZ?8DY_0D)S-EJ+3FZ?8IK+C"
        "EZ/4D9W.D9W.G*C9GZO<G*C9G:G:BY?(B)3%CIK+JK;GIK+CE:'2BY?(>H:W@8V^"
        "B)3%C9G*H:W>J+3EE*#1AY/$=H*S=H*S?XN\\BY?(DI[/B)3%AY/$C)C)F:76GZO<"
        ">H:W?(BYAI+#FJ;7GZO<CYO,CIK+?HJ[?(BYB97&IK+CF*35@HZ_?HJ[=X.TAY/$"
        "CIK+EJ+3JK;GK[OLGZO<B97&=(\"Q<7VN=X.T?HJ[@(R]@8V^CYO,E:'2JK;GJK;G"
        ":'2E=X.TBY?(I+#AIK+CBI;'A)#!@HZ_A9'\"CYO,H*S=DI[/>X>X>(2U?(BYDY_0"
        "EZ/4HZ_@I[/DI[/DHJ[?DI[/?8FZ<7VN;WNL=8&R>H:WAI+#EZ/4GJK;L;WNO,CY"
        "=8&R?XN\\A)#!CYO,D)S-@8V^AY/$DI[/E:'2E:'2C)C)@(R]>86V>86V@HZ_G*C9"
        "HJ[?K+CII[/DH*S=H:W>DY_0@X^`>86V>X>X@X^`BI;'E*#1EZ/4I+#AK[OLN\\?X"
        "BY?(CIK+BY?(BY?(BY?(?XN\\@X^`C)C)DI[/CIK+@HZ_AI+#D9W.CIK+A9'\"DI[/"
        "HZ_@K;GJI[/DEZ/4CYO,BI;'?HJ[?HJ[AY/$C)C)C)C)EJ+3CYO,F*35FJ;7GJK;"
        "@X^`B)3%A)#!AI+#@(R]?(BYAY/$B)3%@HZ_@8V^BI;'G:G:K+CIJ;7FDI[/CIK+"
        "G*C9F:76DI[/C)C)A)#!AI+#@X^`BY?(G:G:H:W>DI[/CIK+C)C)GZO<FZ?8G*C9"
        ";GJK>86V?8FZ?XN\\?HJ[@(R]CYO,CIK+?XN\\@X^`FZ?8JK;GK[OLK[OLEZ/4E*#1"
        "J+3EGZO<EZ/4FJ;7@(R]<GZO<7VN>(2UA9'\"C9G*CIK+D9W.CYO,G*C9GZO<HZ_@"
        "6&25;'BI@HZ_?(BY>(2U@(R]G*C9E:'2AI+#D9W.N,3UP<W^ML+SJ;7FG:G:GZO<"
        "M,#QJK;GG:G:H*S=?XN\\:W>H<7VN>X>X?HJ[C)C)GZO<IK+CH*S=H:W>HJ[?H*S="
        "3UN,8V^@>H:W;7FJ9G*C=H*SF:76H:W>E:'2FJ;7P,S]OLK[I;'BF:76FZ?8F:76"
        "J[?HJ+3EHZ_@IK+CA9'\"=H*SBI;'EZ/4B)3%A)#!F*35J;7FH*S=I;'BH*S=D9W.";

    loading_program = api.alloc_program();
    loading_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_screen_vert)));
    loading_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_screen_spinner)));
    if(auto res = loading_program->compile(); res.has_error())
        cWarning("Error compiling loader shader: {}", res.error());

    loading_bg_program = api.alloc_program();
    loading_bg_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_screen_vert)));
    loading_bg_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_screen_background)));
    if(auto res = loading_bg_program->compile(); res.has_error())
        cWarning("Error compiling loader bg shader: {}", res.error());

    loading_tex =
        api.alloc_texture(gfx::textures::d2, PixDesc(pix_fmt::RGB8), 1);
    loading_tex->alloc(size_3d<u32>{32, 32, 1});
    loading_tex->upload(gsl::span(noise_tex), Veci2{}, size_2d<i32>{32, 32});
    loading_sampler = loading_tex->sampler();
    loading_sampler->alloc();
    loading_sampler->set_edge_policy(0, typing::WrapPolicy::MirrorRepeat);
    loading_sampler->set_edge_policy(1, typing::WrapPolicy::MirrorRepeat);

    loading_ebo = api.alloc_buffer(gfx::buffers::element, RSCA::ReadOnly);
    loading_vbo = api.alloc_buffer(gfx::buffers::vertex, RSCA::ReadOnly);
    loading_vao = api.alloc_vertex_array();
    ring_program = api.alloc_program();
    machine_tex = api.alloc_texture(gfx::textures::d2, PixDesc(pix_fmt::RGB565), 1);
    simplex_noise_tex = api.alloc_texture(gfx::textures::d2, PixDesc(pix_fmt::R8), 1);

    using semantic::SpanOver;
    loading_ebo->alloc();
    loading_vbo->alloc();
    struct mesh_source_t
    {
        model_name_t                    name;
        gsl::span<const u16>            indices;
        gsl::span<const crunch::vertex> vertices;
    };
    // Named explicitly; object order in the OBJ is not model_name_t order
    std::array<mesh_source_t, 3> meshes = {{
        {
            model_ring_exterior,
            gsl::span(blam::loading::loading_torus_indices),
            gsl::span(blam::loading::loading_torus_vertices),
        },
        {
            model_ring_interior,
            gsl::span(blam::loading::loading_cylinder_indices),
            gsl::span(blam::loading::loading_cylinder_vertices),
        },
        {
            model_threshold,
            gsl::span(blam::loading::loading_sphere_indices),
            gsl::span(blam::loading::loading_sphere_vertices),
        }
    }};
    loading_ebo->commit(stl_types::accumulate(
        meshes, 0u, [](auto const& m, u32 size) {
            return m.indices.size_bytes() + size;
        }));
    loading_vbo->commit(stl_types::accumulate(
        meshes, 0u, [](auto const& m, u32 size) {
            return m.vertices.size_bytes() + size;
        }));
    size_t ebo_ptr{0}, vbo_ptr{0};
    for(auto const& mesh : meshes)
    {
        loading_ebo->update(ebo_ptr, mesh.indices);
        loading_vbo->update(vbo_ptr, mesh.vertices);
        loading_draws[mesh.name] = {
            .elements = {
                .count = static_cast<u32>(mesh.indices.size()),
                // offset is in bytes, vertex_offset is in vertices
                .offset = ebo_ptr,
                .vertex_offset = vbo_ptr / sizeof(crunch::vertex),
                .type = semantic::type_t::u16,
            },
            .debug_identifier = std::string(magic_enum::enum_name(mesh.name)),
        };
        ebo_ptr += mesh.indices.size_bytes();
        vbo_ptr += mesh.vertices.size_bytes();
    }
    loading_draws[model_ring_exterior].instances.offset = 0;
    loading_draws[model_ring_interior].instances.offset = 1;
    loading_draws[model_threshold].instances.offset     = 2;
    loading_vao->alloc();
    loading_vao->add(gfx::vertex_attribute::from_member(&crunch::vertex::position).at(0));
    loading_vao->add(gfx::vertex_attribute::from_member(&crunch::vertex::texcoord).at(1));
    loading_vao->set_attribute_names({
        {"pos", 0},
        {"tex", 1},
    });
    // The scene shaders declare attributes by name, without locations
    loading_vao->force_attribute_names();
    loading_vao->set_buffer(gfx::buffers::element, loading_ebo);
    loading_vao->set_buffer(gfx::buffers::vertex, loading_vbo, 0);

    machine_tex->alloc(size_3d<u32>(64, 128, 1));
    machine_tex->upload(blam::loading::machinery_data, Veci2{0, 0}, size_2d<i32>{64, 128});
    simplex_noise_tex->alloc(size_3d<u32>(128, 128, 1));
    simplex_noise_tex->upload(blam::loading::threshold_noise_data, Veci2{0, 0}, size_2d<i32>{128, 128});
    simplex_noise_sampler = simplex_noise_tex->sampler();
    simplex_noise_sampler->alloc();

    loading_textures = {
        machine_tex,
        simplex_noise_tex,
        simplex_noise_tex,
    };

    ring_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_scene_vert)));
    ring_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_scene_frag)));
    if(auto res = ring_program->compile(); res.has_error())
    {
        cWarning("Failed to compile loading scene program: {}", res.error());
    }

    ring_color = api.alloc_texture(gfx::textures::d2, PixDesc(pix_fmt::RGB565), 1);
    ring_color->alloc(size_3d<u32>{640, 360, 1});
    ring_color_sampler = ring_color->sampler();
    ring_color_sampler->alloc();
    ring_rt = api.alloc_rendertarget();
    ring_rt->alloc();
    ring_rt->attach(gfx::render_targets::attachment::color, *ring_color, 0);
    // Deeper than 16-bit where the driver offers it; the ring's inner and
    // outer surfaces are under half a unit apart at ~100 units out
    ring_rt->attach_renderbuffer(
        gfx::render_targets::attachment::depth,
        PixDesc(api.feature_info().rendertarget.high_precision_depth_format));
    ring_rt->resize(typing::geometry::rect<i32>{0, 0, 640, 360});

    // Radial blur over the baked ring texture, standing in for the original's
    // accumulation buffer
    zoom_program = api.alloc_program();
    zoom_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_screen_vert)));
    zoom_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::loading_zoom_frag)));
    if(auto res = zoom_program->compile(); res.has_error())
    {
        cWarning("Failed to compile loading zoom program: {}", res.error());
    }
}

void LoadingScreen::render_ring_texture(gfx::system& api)
{
    if(m_ring_rendered)
        return;

    // Blender is Z-up while the exported mesh is Y-up, so a Blender position
    // (x, y, z) belongs here as (x, z, -y): (5.3, 14.6, 18.4) becomes the
    // below. The camera sits inside the ring (67.7 units from a centre of
    // radius 87.9) and looks along it, which is what gives the surface its
    // receding sweep; aiming across the band instead just fills the frame.
    Matf4 camera =
        glm::translate(glm::identity<Matf4>(), Vecf3(5.30f, 18.40f, -14.60f)) *
        glm::rotate(
            glm::identity<Matf4>(), glm::radians(175.054f), Vecf3(0, 0, 1)) *
        glm::rotate(
            glm::identity<Matf4>(), glm::radians(-18.222f), Vecf3(0, 1, 0)) *
        glm::rotate(
            glm::identity<Matf4>(), glm::radians(-158.379f), Vecf3(1, 0, 0));

    // A camera transform is a world transform; the view matrix is its inverse
    Matf4 scene_transform =
        // Near plane dominates depth precision, and the closest geometry is
        // 8.6 units away: at 0.1 a 16-bit buffer resolves only ~1.5 units at
        // ring distance, while the ring's two surfaces come within 0.48, so
        // they z-fight. 5.0 clips nothing and resolves ~0.03 units.
        glm::perspective(glm::radians(60.f), 1.77f, 5.f, 350.f) *
        glm::inverse(camera);

    ring_rt->clear(1.f);
    ring_rt->clear(Vecf4{0, 0, 0, 1});
    auto res = api.submit(gfx::draw_command{
            .program = ring_program,
            .vertices = loading_vao,
            .render_target = ring_rt,
            .call = {
                .indexed = true,
                .mode = gfx::drawing::primitive::triangle,
            },
            .data = stl_types::values(loading_draws),
        },
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"transform"sv}, semantic::SpanOne(scene_transform)}),
        gfx::base_instance_sampler_list{
            .slots = {
                gfx::base_instance_sampler_t{
                    .stage = gfx::program_t::stage_t::Fragment,
                    .location = gfx::uniform_key{"surface"},
                    .sampler = simplex_noise_sampler,
                    .textures = std::span(loading_textures.begin(), loading_textures.end()),
                },
            },
            .first_unit = 0,
        },
        gfx::view_state{
            .view = typing::vector_types::Veci4{0, 0, 640, 360},
            .depth = gfx::depth_state{},
        });
    if(res)
    {
        auto [err, msg] = *res;
        cWarning(
            "Failed to draw loading scene: {}: {}",
            gleam::detail::draw_error_to_string(err),
            msg);
    }

    // Everything below is released, so this must not run a second time
    m_ring_rendered = true;

    ring_rt->dealloc();
    ring_program->dealloc();
    loading_ebo->dealloc();
    loading_vbo->dealloc();
    loading_vao->dealloc();
    machine_tex->dealloc();
    simplex_noise_tex->dealloc();
    simplex_noise_sampler->dealloc();
    loading_textures.clear();
}
