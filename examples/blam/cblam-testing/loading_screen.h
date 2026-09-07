#pragma once

#include "rendering.h"

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

    Matf4 transform = glm::rotate(
        glm::scale(
            glm::identity<Matf4>(),
            Vecf3{
                2 / (screen_aspect < 1 ? screen_aspect : 1),
                2 * (screen_aspect > 1 ? screen_aspect : 1),
                1}),
        0.f /*std::fmod(timef, glm::pi<f32>() * 2)*/,
        Vecf3{0, 0, 1});
    Vecf2 bg_offset{0, std::fmod(timef, stl_types::math::pi_f * 4.f)};

    f32 time_left =
        loading_screen_gone_time.has_value()
            ? std::chrono::duration_cast<stl_types::chrono::seconds_f32>(
                  *loading_screen_gone_time - time)
                  .count()
            : 2.f;
    f32 curtain_fade = time_left / 2.f;

    auto res = api->submit(
        gfx::draw_command{
            .program  = loading_bg_program,
            .vertices = quad_vao,
            .call =
                {
                    .indexed = false,
                    .mode    = gfx::drawing::primitive::triangle_fan,
                },
            .data = {{.arrays = {.count = 34, .offset = 4}}},
        },
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{{"transform"sv}, semantic::SpanOne(transform)},
            gfx::uniform_pair{{"tex_offset"sv}, semantic::SpanOne(bg_offset)}),
        gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{
                {"curtain_fade"sv}, semantic::SpanOne(curtain_fade)}),
        gfx::make_sampler_list(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"noise"},
                loading_sampler,
            }),
        gfx::blend_state{});

    if(status->progress < 0)
        return;

    was_loading = true;

    transform = glm::translate(
        // glm::scale(glm::identity<Matf4>(), glm::vec3(0.2f)),
        glm::scale(
            glm::identity<Matf4>(), glm::vec3(0.2f / screen_aspect, 0.2f, 1)),
        glm::vec3(1 / (0.2f / screen_aspect) - 1, -5, 0));
    timef     = std::fmod(timef, 10.f) / 10.f;
    f32 start = timef;
    f32 end   = std::fmod(timef + 0.2f, 1.f);

    res = api->submit(
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
    using namespace gleam::literals;
    using vecb4 = typing::vectors::tvector<libc_types::i8, 4>;
    using vecb2 = typing::vectors::tvector<libc_types::i8, 2>;

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

    constexpr std::string_view vertex_shader = R"(#version 100
precision highp float;
attribute vec2 pos;
attribute vec2 tex;
varying vec2 in_tex;
varying vec2 in_pos;
uniform mat4 transform;
uniform vec2 tex_offset;
void main()
{
    in_pos = pos.xy;
    in_tex = tex - (tex_offset / 4.0);
    gl_Position = transform * vec4(pos.x, pos.y, 0.3, 1.0);
}
)";

    constexpr std::string_view fragment_shader = R"(#version 100
precision highp float;

varying vec2 in_tex;
uniform float range_start;

void main()
{
    float dist_from_center = length(in_tex * 1.3 - 0.65);
    if(dist_from_center > 0.6)
        discard;
    highp float alpha = sin(abs(dist_from_center) * 10.0 + 3.34) * 1.4 - 0.4;
    vec2 p1 = in_tex * 1.3 - 0.65;
    float p2_rad = range_start * 31.4;
    vec2 p2 = vec2(sin(p2_rad), cos(p2_rad));
    alpha = clamp(alpha, 0.0, 1.0) * (length(p2 - p1) / 4.0);
    alpha = pow(alpha * 2.5, 4.0);
    gl_FragColor = vec4(vec3(1.0), alpha);
})";

    constexpr std::string_view background_fragment_shader = R"(#version 100
precision highp float;

varying vec2 in_pos;
varying vec2 in_tex;

uniform sampler2D noise;
uniform float curtain_fade;

void main()
{
    float fade = pow(length(abs(in_pos)), 1.2);
    gl_FragColor = vec4(
        vec3(
            length(texture2D(noise, in_tex).rgb * fade) *
            3.0 *
            texture2D(noise, in_tex * vec2(-0.2, 0.05)).rgb
        ),
        curtain_fade
    );
})";

    loading_program = api.alloc_program();
    loading_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(vertex_shader)));
    loading_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(fragment_shader)));
    if(auto res = loading_program->compile(); res.has_error())
        cWarning("Error compiling loader shader: {}", res.error());

    loading_bg_program = api.alloc_program();
    loading_bg_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(vertex_shader)));
    loading_bg_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                background_fragment_shader)));
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
}
