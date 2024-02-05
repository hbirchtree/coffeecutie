#include <coffee/emscripten_comp/emscripten_components.h>

#include <coffee/comp_app/gl_config.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include <coffee/core/CDebug>

namespace {

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE  context_handle;
EmscriptenWebGLContextAttributes context_attributes;

} // namespace

namespace emscripten {

/* WebGL initialization code borrowed from SDL3, modified so we can set more
 * attributes such as powerPreference */

libc_types::i32 GLSwapControl::swapInterval() const
{
    int mode, value;
    emscripten_get_main_loop_timing(&mode, &value);
    return mode == EM_TIMING_SETTIMEOUT ? 0 : value;
}

void GLSwapControl::setSwapInterval(libc_types::i32 interval)
{
    if(interval == 0)
        emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, 0);
    else if(interval > 0)
        emscripten_set_main_loop_timing(EM_TIMING_RAF, interval);
}

void GLFramebuffer::load(entity_container& c, comp_app::app_error&)
{
}

void GLFramebuffer::swapBuffers(comp_app::app_error& ec)
{
// clang-format off
    EM_ASM(
        const parent = document.querySelector('#canvas').closest('body, div, span');
        document.querySelector("#canvas").width = parent.width;
        document.querySelector("#canvas").height = parent.height;
    );
// clang-format on
    if(emscripten_has_asyncify())
        emscripten_sleep(0);
    else
        emscripten_webgl_commit_frame();
}

comp_app::size_2d_t GLFramebuffer::size() const
{
    int width = 0, height = 0;
    emscripten_webgl_get_drawing_buffer_size(context_handle, &width, &height);
    return comp_app::size_2d_t{width, height};
}

void GLFramebuffer::start_frame(compo::ContainerProxy&, const time_point&)
{
}

void GLContext::load(entity_container& c, comp_app::app_error& ec)
{
    using namespace typing::pixels;

    emscripten_webgl_init_context_attributes(&context_attributes);

    auto& glConfig = comp_app::AppLoader::config<comp_app::GLConfig>(c);
    auto  color = properties::get<properties::layout>(glConfig.framebufferFmt);
    auto depth_stencil = properties::get<properties::layout>(glConfig.depthFmt);

    context_attributes.alpha     = color.a > 0;
    context_attributes.depth     = depth_stencil.depth > 0;
    context_attributes.stencil   = depth_stencil.stencil > 0;
    context_attributes.antialias = glConfig.multisampling.enabled;

    context_attributes.majorVersion = 2;
    context_attributes.minorVersion = 0;

    if((glConfig.profile & comp_app::GLConfig::PerfMask) != 0)
    {
        context_attributes.powerPreference
            = (glConfig.profile & comp_app::GLConfig::PreferHighPerf)
                  ? EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE
                  : EM_WEBGL_POWER_PREFERENCE_LOW_POWER;
        if(context_attributes.powerPreference
           == EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE)
            context_attributes.failIfMajorPerformanceCaveat = EM_TRUE;
    }

    context_handle
        = emscripten_webgl_create_context("#canvas", &context_attributes);

    if(context_handle < 0)
    {
        ec = comp_app::AppError::NoContext;
        ec = "Failed to create WebGL context";
        return;
    }

    if(auto err = emscripten_webgl_make_context_current(context_handle))
    {
        ec = comp_app::AppError::ContextFailedBind;
        ec = "Failed to make WebGL context current";
        return;
    }

    emscripten_set_webglcontextlost_callback(
        "#canvas", nullptr, false, ([](int, const void*, void*) {
            Coffee::Logging::cDebug("WebGL context lost");
            return 0;
        }));
    emscripten_set_webglcontextrestored_callback(
        "#canvas", nullptr, false, ([](int, const void*, void*) {
            Coffee::Logging::cDebug("WebGL context restored");
            return 0;
        }));

    auto& bindConf
        = comp_app::AppLoader::config<comp_app::GraphicsBindingConfig>(c);
    bindConf.loader = emscripten_webgl_get_proc_address;

    if((glConfig.profile & comp_app::GLConfig::Debug) != 0)
    {
        emscripten_webgl_enable_extension(
            context_handle, "WEBGL_debug_renderer_info");
        emscripten_webgl_enable_extension(
            context_handle, "WEBGL_debug_shaders");
    }

    // c.service<GLSwapControl>()->setSwapInterval(glConfig.swapInterval);
}

void GLContext::unload(entity_container&, comp_app::app_error&)
{
    emscripten_webgl_destroy_context(context_handle);
}

void ControllerInput::start_frame(
    comp_app::detail::ContainerProxy& c, const comp_app::detail::time_point&)
{
    using libc_types::f32;
    using libc_types::i16;

    auto& config = comp_app::AppLoader::config<comp_app::ControllerConfig>(
        c.underlying());
    constexpr auto max_val  = std::numeric_limits<i16>::max();
    auto           dead_val = max_val - config.deadzone;

    m_scale    = max_val / f32(dead_val);
    m_deadzone = config.deadzone;

    emscripten_sample_gamepad_data();
}

libc_types::u32 ControllerInput::count() const
{
    return C_CAST<libc_types::u32>(emscripten_get_num_gamepads());
}
ControllerInput::controller_map ControllerInput::state(
    libc_types::u32 idx) const
{
    using libc_types::i16;
    using libc_types::u8;

    EmscriptenGamepadEvent state;
    emscripten_get_gamepad_status(C_CAST<int>(idx), &state);

    controller_map out = {};
    if(!state.connected || state.numButtons < 11)
        return out;
    constexpr std::array<std::pair<u8, u8>, 16> mapping = {{
        {0, 0}, // A
        {1, 1}, // B
        {2, 2}, // X
        {3, 3}, // Y

        {4, 9},  // Bumper left
        {5, 10}, // Bumper right
                 //        {6, 0}, // Trigger left
                 //        {7, 0}, // Bumper right

        {8, 4},  // Back
        {9, 5},  // Start
        {10, 8}, // Stick left
        {11, 9}, // Stick right

        {12, 12}, // D-pad up
        {13, 13}, // D-pad down
        {14, 14}, // D-pad left
        {15, 15}, // D-pad right
    }};

    for(auto const& map : mapping)
        out.buttons.d |= state.digitalButton[map.first] << map.second;

    constexpr std::array<std::pair<u8, u8>, 6> axes = {{
        {0, 0}, /* Left stick X */
        {1, 1}, /* Left stick Y */
        {3, 2}, /* Right stick X */
        {4, 3}, /* Right stick Y */
        {2, 4}, /* Left trigger */
        {5, 5}, /* Right trigger */
    }};

    for(auto const& map : axes)
    {
        i16 value = static_cast<i16>(
            state.axis[map.first] * std::numeric_limits<i16>::max());
        if(map.second >= 4)
            value = value / 2 + std::numeric_limits<i16>::max() / 2;
        if(value < m_deadzone && value > -m_deadzone)
            out.axes.d[map.second] = 0;
        else
        {
            auto offset = value > 0 ? -m_deadzone : m_deadzone;
            //            out.axes.d[map.second] = (value + m_deadzone) *
            //            m_scale;
            out.axes.d[map.second] = value + offset;
        }
    }

    return out;
}
comp_app::text_type_t ControllerInput::name(libc_types::u32 idx) const
{
    return {};
}

BatteryProvider::PowerSource BatteryProvider::source() const
{
    EmscriptenBatteryEvent battery{};
    if(emscripten_get_battery_status(&battery) != EMSCRIPTEN_RESULT_SUCCESS)
        return PowerSource::AC;
    return std::isinf(battery.dischargingTime) ? PowerSource::AC
                                               : PowerSource::Battery;
}

libc_types::u16 BatteryProvider::percentage() const
{
    EmscriptenBatteryEvent battery{};
    if(emscripten_get_battery_status(&battery) != EMSCRIPTEN_RESULT_SUCCESS)
        return 0;
    return std::numeric_limits<libc_types::u16>::max() * battery.level;
}

} // namespace emscripten
