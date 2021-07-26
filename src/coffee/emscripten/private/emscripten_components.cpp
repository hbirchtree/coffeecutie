#include <coffee/emscripten_comp/emscripten_components.h>

#include <emscripten/html5.h>

#include <coffee/core/CDebug>

namespace emscripten {

ControllerInput::~ControllerInput()
{
}

void ControllerInput::start_frame(
    comp_app::detail::ContainerProxy&, const comp_app::detail::time_point&)
{
    emscripten_sample_gamepad_data();
}

libc_types::u32 ControllerInput::count() const
{
    return C_CAST<libc_types::u32>(emscripten_get_num_gamepads());
}
ControllerInput::controller_map ControllerInput::state(
    libc_types::u32 idx) const
{
    using libc_types::u8;

    EmscriptenGamepadEvent state;
    emscripten_get_gamepad_status(C_CAST<int>(idx), &state);

    controller_map out = {};
    if(!state.connected || state.numButtons < 11)
        return out;
    constexpr stl_types::Array<stl_types::Pair<u8, u8>, 16> mapping = {{
        {0, 0}, // A
        {1, 1}, // B
        {2, 2}, // X
        {3, 3}, // Y

        {4, 9}, // Bumper left
        {5, 10}, // Bumper right
//        {6, 0}, // Trigger left
//        {7, 0}, // Bumper right

        {8, 4}, // Back
        {9, 5}, // Start
        {10, 8}, // Stick left
        {11, 9}, // Stick right

        {12, 12}, // D-pad up
        {13, 13}, // D-pad down
        {14, 14}, // D-pad left
        {15, 15}, // D-pad right
    }};

    for(auto const& map : mapping)
        out.buttons.d |= state.digitalButton[map.first] << map.second;

    return out;
}
comp_app::text_type_t ControllerInput::name(libc_types::u32 idx) const
{
    return {};
}

} // namespace emscripten
