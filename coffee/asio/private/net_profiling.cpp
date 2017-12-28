#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/CMD>
#include <coffee/asio/net_resource.h>
#include <coffee/core/plat/plat_environment.h>

namespace Coffee{
namespace Net{
void ProfilingExport()
{
    cVerbose(10, "Checking for network profiling...");

    const constexpr cstring network_server = "COFFEE_REPORT_URL";
    if(Env::ExistsVar(network_server))
    {
        cVerbose(10, "Network export starting  1");

        CString target_chrome;
        Profiling::ExportChromeTracerData(target_chrome);

        auto ctxt = ASIO::ASIO_Client::InitService();

        auto netServerUrl = Env::GetVar(network_server);

        auto reportBin = Net::MkUrl(netServerUrl.c_str());
        Net::Resource reportBinRsc(ctxt, reportBin);

        if(!reportBinRsc.connected())
        {
            cDebug("Failed to connect to {0}: {1}",
                     reportBinRsc.resource(),
                     reportBinRsc.errorCode().message());
            return;
        }

        reportBinRsc.setHeaderField("Content-Type",
                                    "application/json");
        reportBinRsc.setHeaderField(
                    "X-Coffee-Token",
                    "token " + Env::GetVar("COFFEE_REPORT_ID"));
        auto chromeData = Bytes::CreateString(
                    target_chrome.c_str());

        reportBinRsc.push("POST",
                          chromeData);

        if(reportBinRsc.responseCode() != 200)
        {
            cWarning("Got bad response from server: {1}\n{0}",
                     StrUtil::encapsulate((cstring)reportBinRsc.data().data,
                                          reportBinRsc.data().size),
                     reportBinRsc.responseCode());
        }else
        {
            auto data = reportBinRsc.data();
            cVerbose(10, "Network export successful with response: {0}",
                   StrUtil::encapsulate(
                       (cstring)data.data,
                       data.size));
        }
    }else
        cVerbose(10, "Network export cancelled");
}

void RegisterProfiling()
{
    Cmd::RegisterAtExit(ProfilingExport);
}

}
}
