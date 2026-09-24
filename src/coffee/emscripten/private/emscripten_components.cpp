#include <coffee/emscripten_comp/emscripten_components.h>

#include <coffee/comp_app/gl_config.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include <coffee/core/CDebug>

namespace emscripten {

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
