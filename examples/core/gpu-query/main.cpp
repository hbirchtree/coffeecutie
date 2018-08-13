#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/graphics/common/query/gpu_query.h>

using namespace Coffee;

i32 coffee_main(i32, cstring_w*)
{
    GpuInfo::GpuQueryInterface fun = {};
    gpu_query_error ec;
    if(GpuInfo::LoadDefaultGpuQuery(fun, ec))
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

            cDebug("Power mode: {0}", C_CAST<u32>(e.pMode()));
        }
    }else
    {
        cDebug("Failed to load: {0}", ec.message());
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
