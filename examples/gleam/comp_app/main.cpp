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
#include <coffee/interfaces/graphics_subsystem.h>

#if defined(FEATURE_ENABLE_GLeamRHI)
#include <coffee/graphics/apis/CGLeamRHI>
#endif
#if defined(FEATURE_ENABLE_Gexxo)
#include <coffee/gexxo/gexxo_api.h>
#include <ogc/gx.h>
#endif

#include <coffee/strings/geometry_types.h>
#include <coffee/strings/vector_types.h>
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>

namespace Coffee {
namespace Strings {

static CString to_string(comp_app::ControllerInput::controller_map const& box)
{
    return cStringFormat(
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

#if defined(FEATURE_ENABLE_GLeamRHI)
    using GFX = RHI::GLEAM::GLEAM_API;
#elif defined(FEATURE_ENABLE_Gexxo)
    using GFX = gexxo::GXAPI;
#else
    using GFX = RHI::NullAPI;
#endif
    using GFX_SYS = RHI::Components::GraphicsAllocator<GFX>;

    SetPrintingVerbosity(15);

    EntityContainer& e = comp_app::createContainer();

    comp_app::app_error ec;

    auto& loader = comp_app::AppLoader::register_service(e);

    comp_app::configureDefaults(loader);

    {
        auto& windowConf = loader.config<comp_app::WindowConfig>();
        auto& ctlrConf   = loader.config<comp_app::ControllerConfig>();

        windowConf.size  = {1024, 768};
        windowConf.title = "Hello World";

#if defined(FEATURE_ENABLE_GLeamRHI)
        auto& glConf = loader.config<comp_app::GLConfig>();
#endif
#if defined(FEATURE_ENABLE_GLADComponent)
        glConf.framebufferFmt = typing::pixels::PixFmt::RGB565;

        glConf.profile |= comp_app::GLConfig::Debug;
#if defined(FEATURE_ENABLE_GLAD_Core)
        glConf.version.major = 3;
        glConf.version.minor = 3;
#else
        glConf.version.major = 2;
        glConf.version.minor = 0;
#endif
#endif

        ctlrConf.options = comp_app::ControllerConfig::BackgroundInput;
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
        GFX::API_CONTEXT context;

        GFX::PIP          plain_color;
        GFX::D_DATA       plane_data;
        GFX::V_DESC       plane_desc;
        ShPtr<GFX::BUF_A> plane_vertex;
        ShPtr<GFX::BUF_E> plane_index;

        ~APIData()
        {
            abort();
        }
    };

#if defined(COFFEE_GEKKO)
    Mtx44 perspective;
    Mtx   modelview;
#endif

    static Array<u8, 20> vertex_data = {
        32,  32,  0xff, 0x00, 0xff,

        255, 32,  0xff, 0xff, 0x00,

        32,  255, 0xff, 0x00, 0xff,

        255, 255, 0x00, 0xff, 0xff,
    };
//    static Array<u8, 4> index_data = {0, 1, 2, 3};

    comp_app::AppContainer<APIData>::addTo(
        e,
#if defined(COFFEE_GEKKO)
        [&perspective, &modelview](
#else
        [](
#endif
            EntityContainer& e, APIData& d, Components::time_point const&) {
            e.register_subsystem_inplace<GFX_SYS::tag_type, GFX_SYS>(
                GFX::OPTS(), true);

            auto& sys = e.subsystem_cast<GFX_SYS>();

            d.plane_vertex = sys.alloc_buffer<GFX::BUF_A>(RSCA::ReadOnly, 0);

            d.plane_data = GFX::D_DATA(3);
            d.plane_desc = GFX::V_DESC();

            d.plane_vertex->alloc();
            d.plane_vertex->commit(BytesConst::CreateFrom(vertex_data));

            d.plane_desc.alloc();
            {
                GFX::V_ATTR pos, col;
                pos.m_idx    = 0;
                col.m_idx    = 1;
                pos.m_size   = 2;
                col.m_size   = 3;
                col.m_off    = sizeof(u8) * 2;
                pos.m_stride = col.m_stride = sizeof(u8) * 5;
                pos.m_type = col.m_type = semantic::TypeEnum::UByte;
                pos.m_bassoc = col.m_bassoc = 0;
                pos.m_boffset = col.m_boffset = 0;
                d.plane_desc.addAttribute(pos);
                d.plane_desc.addAttribute(col);
                d.plane_desc.bindBuffer(0, *d.plane_vertex);
            }

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
        [](EntityContainer& e,
           APIData&         data,
           Components::time_point const&,
           Components::duration const&) {

            GFX::DefaultFramebuffer()->clear(0, {0.5f, 0, 0, 1}, 1);

            GFX::Draw(
                data.plain_color,
                {},
                data.plane_desc,
                GFX::D_CALL(true, false),
                data.plane_data);

        },
        [](EntityContainer& e, APIData& d, Components::time_point const&) {
            e.remove_subsystems_matching<
                Components::matchers::all_subsystems_in<
                    type_list_t<GFX_SYS>>>();
            d.plain_color.dealloc();
            d.plane_desc.dealloc();
            GFX::UnloadAPI();
            d.context = nullptr;
        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN(coffee_main)
