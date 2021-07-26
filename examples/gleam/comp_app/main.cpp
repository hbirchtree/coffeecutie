#include <coffee/comp_app/app_wrap.h>
#include <coffee/comp_app/bundle.h>
#include <coffee/comp_app/eventapp_wrapper.h>
#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/components/components.h>
#include <coffee/core/CApplication>
#include <coffee/core/coffee.h>
#include <coffee/core/input/eventhandlers.h>
#include <coffee/core/task_queue/task.h>

#if defined(GLEAM_MAX_VERSION)
#include <coffee/graphics/apis/CGLeam>
#endif
#if defined(FEATURE_ENABLE_Gexxo)
#include <coffee/gexxo/gexxo_api.h>
#include <ogc/gx.h>
#endif

#include <coffee/strings/geometry_types.h>
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/vector_types.h>

namespace Coffee {
namespace Strings {

static CString to_string(
    comp_app::interfaces::ControllerInput::controller_map const& box)
{
    return fmt(
        "controller_map("
        "L({0},{1}):R({2},{3}):TRIG({4}:{5})"
        ":A={6}:B={7}:X={8}:Y={9}"
        ":BL={10}:BR={11}:SL={12}:SR={13}"
        ":START={14}:BACK={15}"
        ")",
        box.axes.e.l_x,
        box.axes.e.l_y,
        box.axes.e.r_x,
        box.axes.e.r_y,
        box.axes.e.t_l,
        box.axes.e.t_r,
        box.buttons.e.a,
        box.buttons.e.b,
        box.buttons.e.x,
        box.buttons.e.y,
        box.buttons.e.b_l,
        box.buttons.e.b_r,
        box.buttons.e.s_l,
        box.buttons.e.s_r,
        box.buttons.e.start,
        box.buttons.e.back);
}

} // namespace Strings
} // namespace Coffee

#include <coffee/core/CDebug>

using namespace ::libc_types;

i32 coffee_main(i32, cstring_w*)
{
    using namespace Coffee;
    using namespace Coffee::Components;
    using namespace Coffee::Display;
    using namespace Coffee::Input;

#if defined(GLEAM_MAX_VERSION)
    using gfx = gleam::api;
#elif defined(FEATURE_ENABLE_Gexxo)
    using gfx = gexxo::api;
#endif

    SetPrintingVerbosity(15);

    EntityContainer& e = comp_app::createContainer();

    comp_app::app_error ec;

    auto& loader = comp_app::AppLoader::register_service(e);

    comp_app::configureDefaults(loader);

    {
        auto& windowConf = loader.config<comp_app::WindowConfig>();
        auto& ctlrConf   = loader.config<comp_app::ControllerConfig>();
        windowConf.size  = {1024, 1024};
        windowConf.title = "Hello World";
        ctlrConf.options = comp_app::ControllerConfig::BackgroundInput;

#if defined(GLEAM_USE_ES)
        auto& glConf          = loader.config<comp_app::GLConfig>();
        glConf.framebufferFmt = typing::pixels::PixFmt::RGB565;
//        glConf.profile |= comp_app::GLConfig::Debug;
//        glConf.version.major = 2;
//        glConf.version.minor = 0;
#endif
    }

    comp_app::addDefaults(e, loader, ec);

    auto& inputs = *e.service<comp_app::BasicEventBus<CIEvent>>();

    inputs.addEventFunction<CIControllerAtomicUpdateEvent>(
        10, [](CIEvent&, CIControllerAtomicUpdateEvent* ev) {
            cDebug("Controller: {0} -> {1}", ev->controller, ev->connected);
        });

    inputs.addEventHandler(
        0,
        EventHandlers::ExitOn<EventHandlers::OnKey<CK_Escape>>(
            e.service_ref<comp_app::Windowing>()));

    auto displays = e.service<comp_app::DisplayInfo>();

    if(displays)
    {
        cDebug("{0}", displays->virtualSize().convert<u32>());
        for(auto i : stl_types::Range<libc_types::u32>(displays->count()))
            cDebug("- {0}", displays->size(i).convert<u32>());
    }

    C_ERROR_CHECK(ec)

    RuntimeQueue::CreateNewQueue("Main");

    runtime_queue_error rqec;
    RuntimeQueue::QueuePeriodic(
        RuntimeQueue::GetCurrentQueue(rqec),
        Chrono::milliseconds(100),
        [&e]() {
            auto dump  = e.service<comp_app::ControllerInput>();
            auto mouse = e.service<comp_app::MouseInput>();

            if(!dump || !mouse)
                return;
            return;

            cDebug("Controllers: {0}", dump->count());
            for(auto i : Range<u32>(dump->count()))
                cDebug("Controller {0}: {1}", dump->name(i), dump->state(i));

            cDebug(
                "Mouse state: {0} {1}",
                mouse->position().toVector<f32>(),
                C_CAST<u32>(mouse->buttons()));
        },
        rqec);

    struct APIData
    {
        gfx                      context;
        ShPtr<gfx::buffer_type>  vertex;
        ShPtr<gfx::vertex_type>  verts;
        ShPtr<gfx::program_type> color;
        ShPtr<gfx::program_type> simple;

        i32 load_idx{0};
        f32 load_basis{0};
    };

#if defined(COFFEE_GEKKO)
    Mtx44 perspective;
    Mtx   modelview;
#endif

    struct vertex_t
    {
        tvec2<f32> pos;
        tvec3<f32> col;
        f32        idx;
    };

    static Vector<vertex_t> vertex_data;

    constexpr f32 i_d  = 0.628f;
    f32           v_id = 0;
    for(auto i : Range<f32>(stl_types::math::pi_f * 2, i_d))
    {
        auto v1 = tvec2<f32>{std::cos(i), std::sin(i)} / 5.f;
        auto v2 = tvec2<f32>{std::cos(i + i_d), std::sin(i + i_d)} / 5.f;

        vertex_data.push_back({{v1.x(), v1.y()}, {0.f, 0.f, 0.f}, v_id});
        vertex_data.push_back({{0.f, 0.f}, {1.f}, v_id});
        v_id += 1.f;
    }

    constexpr stl_types::Array<Vecf3, 4> button_colors = {
        {{0, 1, 0}, {1, 0, 0}, {0, 0, 1}, {1, 1, 0}}};
    for(auto const& i : Range<>(button_colors.size()))
    {
        Vecf2 offset = {0.05f * i, 0.f};
        vertex_data.push_back(
            {offset + Vecf2{-0.95f, -0.95f}, button_colors.at(i)});
        vertex_data.push_back(
            {offset + Vecf2{-0.95f, -1.f}, button_colors.at(i)});
        vertex_data.push_back(
            {offset + Vecf2{-1.f, -0.95f}, button_colors.at(i)});
        vertex_data.push_back(
            {offset + Vecf2{-1.f, -1.f}, button_colors.at(i)});
    }

    comp_app::AppContainer<APIData>::addTo(
        e,
#if defined(COFFEE_GEKKO)
        [&perspective, &modelview](
#else
        [](
#endif
            EntityContainer& e, APIData& d, Components::time_point const&) {
            d.context.load({
                .api_version = 0x300,
                .api_type    = gleam::api_type_t::es,
            });

            gleam::cmd::viewport(Veci2{0, 0}, size_2d<i32>{1024, 1024});

            d.vertex = d.context.alloc_buffer(
                gleam::buffers::vertex, RSCA::ReadOnly | RSCA::Streaming);
            d.vertex->alloc();

            d.vertex->commit(vertex_data);

            d.verts = d.context.alloc_vertex_array();
            d.verts->alloc();
            using attr_flags =
                gleam::vertex_array_t::attribute_type::attribute_flags;
            d.verts->add({
                .index = 0,
                .value =
                    {
                        .stride = sizeof(decltype(vertex_data)::value_type),
                        .count  = 2,
                        .type   = semantic::TypeEnum::Scalar,
                    },
            });
            d.verts->add({
                .index = 1,
                .value =
                    {
                        .offset = sizeof(f32) * 2,
                        .stride = sizeof(decltype(vertex_data)::value_type),
                        .count  = 3,
                        .type   = semantic::TypeEnum::Scalar,
                    },
            });
            d.verts->add({
                .index = 2,
                .value =
                    {
                        .offset = sizeof(f32) * 5,
                        .stride = sizeof(decltype(vertex_data)::value_type),
                        .count  = 1,
                        .type   = semantic::TypeEnum::Scalar,
                        //                        .flags  =
                        //                        attr_flags::normalized |
                        //                        attr_flags::packed,
                    },
            });
            d.verts->set_buffer(gleam::buffers::vertex, d.vertex, 0);

            d.color = d.context.alloc_program();
            d.color->add(
                gleam::program_t::stage_t::Vertex,
                d.context.alloc_shader(std::string_view(R"(#version 100
attribute vec2 pos;
attribute vec3 color;
attribute float idx;

varying vec3 fcolor;
varying float fidx;

void main()
{
    fidx = idx;
    fcolor = color;
    gl_Position = vec4(pos, 0, 1);
}
)")));
            d.color->add(
                gleam::program_t::stage_t::Fragment,
                d.context.alloc_shader(std::string_view(R"(#version 100
precision lowp float;
varying vec3 fcolor;
varying float fidx;

uniform int currIdx;

void main()
{
    int diff = int(abs(floor(fidx) - float(currIdx)));
    float amplify = diff < 1 ? 1.0 : 0.0;
    gl_FragColor = vec4(fcolor * amplify, 1.0);
}
)")));

            d.simple = d.context.alloc_program();
            d.simple->add(
                gfx::program_type::stage_t::Vertex,
                d.context.alloc_shader(std::string_view(R"(#version 100
attribute vec2 pos;
attribute vec3 color;

varying vec3 fcolor;

void main()
{
    fcolor = color;
    gl_Position = vec4(pos, 0, 1);
}
)")));
            d.simple->add(
                gfx::program_type::stage_t::Fragment,
                d.context.alloc_shader(std::string_view(R"(#version 100
precision lowp float;
varying vec3 fcolor;

void main()
{
    gl_FragColor = vec4(fcolor, 1.0);
}
)")));
            if(auto res = d.color->compile(); res.has_error())
                cFatal("Shader error log: {0}", res.error());
            else
                cDebug("Shader info log: {0}", res.value());

            if(auto res = d.simple->compile(); res.has_error())
                cFatal("Shader error log: {0}", res.error());
            else
                cDebug("Shader info log: {0}", res.value());

#if defined(COFFEE_GEKKO)
            guOrtho(perspective, 0, 479, 0, 639, 0, 300);
            GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);

            guMtxIdentity(modelview);
            guMtxTransApply(modelview, modelview, 0, 0, -5);
            GX_LoadPosMtxImm(modelview, GX_PNMTX0);

            GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
            GX_SetColorUpdate(GX_TRUE);

            GFX::DefaultFramebuffer()->clear(0, {0.5f, 0, 0, 1}, 1);
#endif
        },
        [](EntityContainer&              e,
           APIData&                      data,
           Components::time_point const& t,
           Components::duration const&   delta) {
            data.context.default_rendertarget()->clear(
                Vecf4{0, 0, 0, 1}, 1.0, 0x0);

            data.context.submit({
                .program  = data.color,
                .vertices = data.verts,
                .call =
                    {
                        .mode = gleam::drawing::primitive::triangle_fan,
                    },
                .data =
                    {
                        .arrays =
                            {
                                .count = 22,
                            },
                    },
            });
            gleam::cmd::uniform(0, 1, SpanOne(data.load_idx));

            auto controller = e.service<comp_app::ControllerInput>()->state(0);
            stl_types::Array<bool, 4> button_states{
                {controller.buttons.e.a,
                 controller.buttons.e.b,
                 controller.buttons.e.x,
                 controller.buttons.e.y}};

            for(auto const& i : Range<u32>(button_states.size()))
                if(button_states.at(i))
                    data.context.submit({
                        .program  = data.simple,
                        .vertices = data.verts,
                        .call =
                            {
                                .mode =
                                    gleam::drawing::primitive::triangle_strip,
                            },
                        .data =
                            {
                                .arrays =
                                    {
                                        .count  = 4,
                                        .offset = 22 + 4 * i,
                                    },
                            },
                    });

            data.load_basis += Chrono::to_float(delta) * 10.f;
            data.load_basis = std::fmod(data.load_basis, 10.f);
            data.load_idx   = i32(data.load_basis);
        },
        [](EntityContainer& e, APIData& d, Components::time_point const&) {
            d.vertex->dealloc();
            d.verts->dealloc();
            d.color->dealloc();
            d.simple->dealloc();
        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN(coffee_main)
