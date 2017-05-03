#include <coffee/core/CApplication>
#include <coffee/core/CPlatform>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>

#include <stdarg.h>

#include <coffee/core/plat/plat_sensor.h>

using namespace Coffee;

int32 coffee_main(int32, cstring_w*)
{
    CResources::FileResourcePrefix("sample_data/");

    Profiler::PushContext("Configuration data");
    {
        /* Check out system directory strings and user data directories */
        CString cfg_dir  = Env::GetUserData(
                    "hbirchtree",
                    "Best Coffee of All Time");
        Profiler::Profile("Get userdata directory");

        CString app_dir  = Env::ApplicationDir();
        CString exe_name = Env::ExecutableName();
        Profiler::Profile("Get application location");

        cDebug("Settings directory: {0}",cfg_dir);
        cDebug("Program directory:  {0}",app_dir);
        cDebug("Launching from      {0}",exe_name);
		cDebug("Current directory:  {0}",Env::CurrentDir());

        Profiler::Profile("Print some data");

        if(!CResources::FileMkdir(cfg_dir.c_str(),true))
            cWarning("Failed to create settings directory");
        else
        {
            CString test_file = Env::ConcatPath(cfg_dir.c_str(),"test_file.sav");
            FileFun::Touch(FileFun::File,test_file.c_str());
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

    cDebug("This process' mapped files:");

    Environment::Linux::MemMap::ProcMap m_map;
    Environment::Linux::MemMap::GetProcMap(ProcessProperty::Pid(), m_map);

    for(auto& e : m_map)
        cDebug("Entry: {0} -> sz:{1},acc:{2}", e.name, e.size(), (u32)e.access);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
