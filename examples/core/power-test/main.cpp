#include <coffee/core/CApplication>
#include <coffee/core/CPlatform>
#include <coffee/strings/libc_types.h>
#include <platforms/base/power.h>

#include <coffee/core/CDebug>

using namespace Coffee;

using PowerInfo = platform::env::PowerInfoDef;

i32 coffee_main(i32, cstring_w*)
{
    cDebug(
        "Battery info: present={0},powered={1},level={2},lifetime={3}",
        PowerInfo::HasBattery(),
        PowerInfo::IsPowered(),
        PowerInfo::BatteryPercentage(),
        PowerInfo::BatteryLifetime());

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
