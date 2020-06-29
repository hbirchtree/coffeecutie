#include <coffee/core/CApplication>
#include <coffee/core/CEnvironment>
#include <coffee/core/CFiles>
#include <coffee/core/CPlatform>
#include <coffee/core/coffee.h>
#include <platforms/sensor.h>
#include <platforms/sysinfo.h>

#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>
#include <coffee/strings/vector_types.h>

#include <coffee/core/CDebug>

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/net_profiling.h>
#endif

using namespace Coffee;
using namespace platform::file;
using namespace platform::url::constructors;

i32 coffee_main(i32, cstring_w*)
{
#if defined(FEATURE_ENABLE_ASIO)
    C_UNUSED(auto _) = Net::RegisterProfiling();
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

        cDebug("Settings directory: {0}", cfg_dir);
        cDebug("Program directory:  {0}", app_dir);
        cDebug("Launching from      {0}", exe_name);
        cDebug("Current directory:  {0}", Env::CurrentDir());

        Profiler::Profile("Print some data");

        if(!FileMkdir(cfg_dir, true))
            cWarning("Failed to create settings directory");
        else
        {
            Url test_file = cfg_dir + Path{"test_file.sav"};
            FileFun::Touch(FileType::File, test_file, ec);
        }
        Profiler::Profile("Create directory recursively");
    }
    Profiler::PopContext();

    cBasicPrint("Processor: {0}", platform::SysInfo::Processor());

    cDebug("Sensor gravity: {0}", Sensor::Gravity());
    cDebug("Sensor gyro: {0}", Sensor::Gyroscope());
    cDebug("Sensor lux: {0}", Sensor::Lux());
    cDebug("Sensor steps: {0}", Sensor::StepCounter());
    cDebug("Sensor acceleration: {0}", Sensor::Acceleration());
    cDebug("Sensor orientation: {0}", Sensor::Orientation());

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)

//#include <gccore.h>

//GXRModeObj* gamecube_rmode = NULL;
//void*       gamecube_xfb   = NULL;

//thread_local int dev;

//int main(int, char**)
//{
//    VIDEO_Init();

//    gamecube_rmode = VIDEO_GetPreferredMode(NULL);

//    gamecube_xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(gamecube_rmode));

//    VIDEO_Configure(gamecube_rmode);
//    VIDEO_SetNextFramebuffer(gamecube_xfb);
//    VIDEO_SetBlack(FALSE);
//    VIDEO_Flush();

//    VIDEO_WaitVSync();
//    if(gamecube_rmode->viTVMode & VI_NON_INTERLACE)
//        VIDEO_WaitVSync();

//    console_init(
//        gamecube_xfb,
//        60,
//        60,
//        gamecube_rmode->fbWidth,
//        gamecube_rmode->xfbHeight,
//        gamecube_rmode->fbWidth * 2);

//    printf("- Gamecube video initialized\n");

//    State::SetInternalState(State::CreateNewState());
//    State::SetInternalThreadState(State::CreateNewThreadState());

//    while(1)
//    {
//        VIDEO_WaitVSync();
//    }
//}
