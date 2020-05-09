#include <platforms/android/sensor.h>

#include <coffee/android/android_main.h>
#include <android/sensor.h>

namespace platform {
namespace sensor {
namespace android {

static ASensorManager* s_sensor_man = nullptr;

enum Sensors
{
    SENS_Grav,
    SENS_Accel,
    SENS_Orient,
    SENS_Gyro,
    SENS_Mag,
    SENS_Lux,
    SENS_Count,
};

static const ASensor* m_sensors[SENS_Count] = {};

void Android_InitSensors()
{
#if __ANDROID_API__ >= 26
    auto pkg_name = ::android::app_info().package_name();
    s_sensor_man = ASensorManager_getInstanceForPackage(pkg_name.c_str());
#else
    s_sensor_man = ASensorManager_getInstance();
#endif

    ASensorList sensors;
    int num_sensors = ASensorManager_getSensorList(s_sensor_man, &sensors);

    scalar scores[6] = {};

    for(int i = 0; i < num_sensors; i++)
    {
        const ASensor* sens = sensors[i];
        scalar         res  = ASensor_getResolution(sens);
        int            type = ASensor_getType(sens);
        int            j;

        switch(type)
        {
        case ASENSOR_TYPE_ACCELEROMETER:
            j = 0;
            break;
        case ASENSOR_TYPE_GYROSCOPE:
            j = 3;
            break;
        case ASENSOR_TYPE_MAGNETIC_FIELD:
            j = 4;
            break;
        case ASENSOR_TYPE_LIGHT:
            j = 5;
            break;
        default:
            j = -1;
            break;
        }
        if(j == -1)
        {
//            cVerbose(
//                5,
//                "Unknown sensor type: {0}, name={1},vendor={2}",
//                type,
//                ASensor_getName(sens),
//                ASensor_getVendor(sens));
            continue;
        }

        if(scores[j] == 0. || scores[j] > res)
        {
            scores[j]    = res;
            m_sensors[j] = sens;
        }
    }

    for(int i = 0; i < 6; i++)
    {
        if(m_sensors[i])
        {
            auto s = m_sensors[i];
//            cVerbose(
//                5,
//                "Selecting sensor {0} {1} for {2}",
//                ASensor_getVendor(s),
//                ASensor_getName(s),
//                ASensor_getType(s));
        }
    }
}
void Android_DestroySensors()
{
}

Vecf3 Sensor::Gravity()
{
    return {};
}

Vecf3 Sensor::Acceleration()
{
    return {};
}

Vecf3 Sensor::Orientation()
{
    return {};
}

Vecf3 Sensor::Gyroscope()
{
    return {};
}

Vecf3 Sensor::Magnetometer()
{
    return {};
}

scalar Sensor::Lux()
{
    return 0.f;
}

} // namespace android
} // namespace sensor
} // namespace platform
