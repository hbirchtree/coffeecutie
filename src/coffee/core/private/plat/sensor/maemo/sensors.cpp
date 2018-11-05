#include <coffee/core/plat/sensor/maemo/sensors.h>
#include <coffee/core/plat/plat_file.h>

#include <coffee/core/plat/plat_memory.h>
#include <peripherals/stl/string_casting.h>
#include <coffee/core/CDebug>

#define const_string static const constexpr cstring

namespace Coffee{
namespace Sensor{
namespace Maemo{

const_string LIS302DL_NAME_PATH = "/sys/class/i2c-adapter/i2c-3/3-001d/name";
const_string LIS302DL_DATA_PATH = "/sys/class/i2c-adapter/i2c-3/3-001d/coord";

const_string AIC34B_NAME_PATH = "/sys/class/i2c-adapter/i2c-2/2-0029/name";
const_string AIC34B_LUX_DATA_PATH = "/sys/class/i2c-adapter/i2c-2/2-0029/lux";
const_string AIC34B_AMBIENT_ADC0_DATA_PATH = "/sys/class/i2c-adapter/i2c-2/2-0029/adc0";
const_string AIC34B_AMBIENT_ADC1_DATA_PATH = "/sys/class/i2c-adapter/i2c-2/2-0029/adc1";

Vecf3 Maemo_SensorAPI::Gravity()
{
    CString data = FileFun::sys_read(LIS302DL_DATA_PATH);

    Vecf3 v = {};

    if(data.size() < 1)
        return v;

    cVerbose(15, "Gravity input: {0}", data);

    size_t p_i = 0;
    size_t i = data.find(' ');

    size_t v_i = 0;
    while(v_i < 3)
    {
        if(i > data.size())
            i = data.size();
        CString en = StrUtil::encapsulate(&data[p_i], i - p_i);
        cVerbose(15, "Gravity string: {0}", en);
        if(en[0] == '-')
            v[v_i] = -cast_string<scalar>(&en[1]) / 100.f;
        else
            v[v_i] = cast_string<scalar>(en) / 100.f;
        v_i++;
        p_i = i;
        i = data.find(' ', p_i + 1);
        cVerbose(15, "Gravity is looking at: {0}, {1}", p_i, i);
    }

    return v;
}

scalar Maemo_SensorAPI::Lux()
{
    return cast_string<scalar>(FileFun::sys_read(AIC34B_LUX_DATA_PATH));
}

}
}
}
