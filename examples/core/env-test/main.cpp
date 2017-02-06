#include <coffee/core/CApplication>
#include <coffee/core/CPlatform>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>

#include <coffee/graphics/common/query/gpu_query.h>

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

    GpuInfo::GpuQueryInterface fun = {};
    if(GpuInfo::LoadDefaultGpuQuery(&fun))
    {
        cDebug("GPU Driver: {0}", fun.GetDriver());
        cDebug("GPU Devices: {0}", fun.GetNumGpus());
        for(GpuInfo::GpuView e : GpuInfo::GpuQueryView(fun))
        {
            cDebug("GPU Model: {0}", e.model());

            auto temp = e.temp();
            cDebug("Temperature: {0} // {1}", temp.current, temp.max);

            auto mem = e.mem();
            cDebug("Memory use: tot={0}, used={1}, free={2}", mem.total, mem.used, mem.free);
            cDebug("Memory used by this application: {0}", e.memUsage(ProcessProperty::Pid()));

            auto clk = e.clock(GpuInfo::Clock::Graphics);
            cDebug("Clock limits: {0} / {1} / {2}", clk.current, clk.min, clk.max);
            clk = e.clock(GpuInfo::Clock::Memory);
            cDebug("Memory limits: {0} / {1} / {2}", clk.current, clk.min, clk.max);
            clk = e.clock(GpuInfo::Clock::VideoDecode);
            cDebug("Video limits: {0} / {1} / {2}", clk.current, clk.min, clk.max);

            auto bus = e.bus();
            cDebug("Bus information: rx:{0} KB/s tx:{1} KB/s", bus.rx, bus.tx);

            auto util = e.usage();
            cDebug("GPU usage: GPU={0}%, MEM={1}%, DECODER={2}%, ENCODER={3}%",
                   util.gpu, util.mem, util.decoder, util.encoder);

            cDebug("Power mode: {0}", C_CAST<uint32>(e.pMode()));
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
