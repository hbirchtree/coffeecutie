#include <coffee/core/plat/sensor/maemo/sensors.h>
#include <coffee/core/plat/plat_file.h>

#include <coffee/core/plat/plat_memory.h>
#include <coffee/core/string_casting.h>

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

    size_t p_i = 0;
    size_t i = data.find(' ');

    size_t v_i = 0;
    while(i <= data.size())
    {
        CString en = StrUtil::encapsulate(&data[0], i - p_i);
        v[v_i] = cast_string<scalar>(en) / 100.f;
        v_i++;
        p_i = i;
        i = data.find(' ');
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
