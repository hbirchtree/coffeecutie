#include <coffee/core/CApplication>
#include <coffee/core/CPlatform>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>

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

        CResources::FileMkdir(cfg_dir.c_str(),true);
        Profiler::Profile("Create directory recursively");
    }
    Profiler::PopContext();

    cDebug("Battery info: present={0},powered={1},level={2},lifetime={3}",
           PowerInfo::HasBattery(),
           PowerInfo::IsPowered(),
           PowerInfo::BatteryPercentage());

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
