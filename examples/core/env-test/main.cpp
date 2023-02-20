#include <coffee/core/CApplication>
#include <coffee/core/CEnvironment>
#include <coffee/core/CFiles>
#include <coffee/core/CPlatform>
#include <coffee/core/coffee.h>
#include <platforms/sensor.h>
#include <platforms/sysinfo.h>

#if defined(FEATURE_ENABLE_MediaFfmpeg)
#include <av/decode.h>
#endif

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

    Profiler::PushContext("Configuration data");
    {
        /* Check out system directory strings and user data directories */
        Url test_dir = MkUrl("config", RSCA::ConfigFile);
        Profiler::Profile("Get userdata directory");

        cDebug("Test directory: {0} '{1}'", test_dir, *test_dir);

        Profiler::Profile("Get application location");

        cDebug("Settings directory: {0}", *MkUrl("", RSCA::ConfigFile));
        cDebug("Program directory:  {0}", platform::path::app_dir());
        cDebug("Launching from      {0}", platform::path::executable());
        cDebug("Current directory:  {0}", platform::path::current_dir());

        Profiler::Profile("Print some data");

        {
            platform::file::create(
                MkUrl("", RSCA::ConfigFile) / "test_file.sav",
                {.mode = platform::file::mode_t::file});
        }
        Profiler::Profile("Create directory recursively");
    }
    Profiler::PopContext();

    if(auto model = platform::info::proc::model(); model.has_value())
    {
        auto [vendor, model_] = model.value();
        cBasicPrint("Processor: {0} {1}", vendor, model_);
    }
    if(auto device = platform::info::device::device())
    {
        auto [vendor, model] = device.value();
        cBasicPrint("Device: {0} {1}", vendor, model);
    }
    cBasicPrint(
        "Processor: nodes={0},cpus={1},cores={2},threads={3}",
        platform::info::proc::node_count(),
        platform::info::proc::cpu_count(),
        platform::info::proc::core_count(),
        platform::info::proc::thread_count());
    cBasicPrint("Processor frequency: {0}", platform::info::proc::frequency());

    cBasicPrint("Kernel: {0}", platform::info::os::kernel());
    cBasicPrint("Kernel version: {0}", platform::info::os::kernel_version());
    cBasicPrint("Architecture: {0}", platform::info::os::architecture());
    cBasicPrint("OS name: {0}", platform::info::os::name());
    cBasicPrint("OS version: {0}", platform::info::os::version());

//    cDebug("Sensor gravity: {0}", Sensor::Gravity());
//    cDebug("Sensor gyro: {0}", Sensor::Gyroscope());
//    cDebug("Sensor lux: {0}", Sensor::Lux());
//    cDebug("Sensor steps: {0}", Sensor::StepCounter());
//    cDebug("Sensor acceleration: {0}", Sensor::Acceleration());
//    cDebug("Sensor orientation: {0}", Sensor::Orientation());

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
