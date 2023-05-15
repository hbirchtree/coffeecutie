#include <coffee/emscripten_comp/emscripten_components.h>

#include <emscripten/html5.h>

#include <coffee/core/CDebug>

namespace emscripten {

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
//            out.axes.d[map.second] = (value + m_deadzone) * m_scale;
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
