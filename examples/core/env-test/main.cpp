#include <coffee/asio/net_profiling.h>
#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>
#include <coffee/core/plat/sensor.h>

#include <coffee/asio/http.h>

#include <coffee/core/CDebug>
#include <coffee/core/CPlatform>

#include <stack>

using namespace Coffee;

i32 coffee_main(i32, cstring_w*)
{
#if defined(FEATURE_ENABLE_CoffeeASIO)
    Net::RegisterProfiling();
#endif

    file_error ec;

    Profiler::PushContext("Configuration data");
    {
        /* Check out system directory strings and user data directories */
        Url test_dir = MkUrl("config", RSCA::ConfigFile);
        Url cfg_dir  = MkUrl("", RSCA::ConfigFile);
        Profiler::Profile("Get userdata directory");

        cDebug("Test directory: {0} '{1}'", test_dir, *test_dir);

        Url     app_dir  = Env::ApplicationDir();
        CString exe_name = Env::ExecutableName();
        Profiler::Profile("Get application location");

        cDebug("Settings directory: {0}", *cfg_dir);
        cDebug("Program directory:  {0}", app_dir);
        cDebug("Launching from      {0}", exe_name);
        cDebug("Current directory:  {0}", Env::CurrentDir());

        Profiler::Profile("Print some data");

        if(!CResources::FileMkdir(cfg_dir, true))
            cWarning("Failed to create settings directory");
        else
        {
            Url test_file = cfg_dir + Path{"test_file.sav"};
            FileFun::Touch(FileFun::File, test_file, ec);
        }
        Profiler::Profile("Create directory recursively");
    }
    Profiler::PopContext();

    cDebug("Sensor gravity: {0}", Sensor::Sensors::Gravity());
    cDebug("Sensor gyro: {0}", Sensor::Sensors::Gyroscope());
    cDebug("Sensor lux: {0}", Sensor::Sensors::Lux());
    cDebug("Sensor steps: {0}", Sensor::Sensors::StepCounter());
    cDebug("Sensor acceleration: {0}", Sensor::Sensors::Acceleration());
    cDebug("Sensor orientation: {0}", Sensor::Sensors::Orientation());

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
