#include <coffee/core/plat/sensor/android/sensor.h>

#include <coffee/core/CDebug>
#include <android/sensor.h>

namespace Coffee{
namespace Sensor{
namespace Android{

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
    cVerbose(5,"Initializing Android Sensor Manager");
    s_sensor_man = ASensorManager_getInstance();

    ASensorList sensors;
    int num_sensors = ASensorManager_getSensorList(s_sensor_man,&sensors);

    scalar scores[6] = {};

    for(int i=0;i<num_sensors;i++)
    {
        const ASensor* sens = sensors[i];
        scalar res = ASensor_getResolution(sens);
        int type = ASensor_getType(sens);
        int j;

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
            cVerbose(5,"Unknown sensor type: {0}, name={1},vendor={2}",type,
                   ASensor_getName(sens),ASensor_getVendor(sens));
            continue;
        }

        if(scores[j] == 0. || scores[j] > res)
        {
            scores[j] = res;
            m_sensors[j] = sens;
        }
    }

    for(int i=0;i<6;i++)
    {
        if(m_sensors[i])
        {
            auto s = m_sensors[i];
            cVerbose(5,"Selecting sensor {0} {1} for {2}",
                     ASensor_getVendor(s),
                     ASensor_getName(s),
                     ASensor_getType(s));
        }
    }
}
void Android_DestroySensors()
{
}

Vecf3 Android_SensorAPI::Gravity()
{
    return {};
}

Vecf3 Android_SensorAPI::Acceleration()
{
    return {};
}

Vecf3 Android_SensorAPI::Orientation()
{
    return {};
}

Vecf3 Android_SensorAPI::Gyroscope()
{
    return {};
}

Vecf3 Android_SensorAPI::Magnetometer()
{
    return {};
}

scalar Android_SensorAPI::Lux()
{
    return 0.f;
}

}
}
}
