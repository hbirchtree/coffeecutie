#include <coffee/core/profiler/profiling-export.h>

#include <coffee/asio/asio_worker.h>
#include <coffee/asio/net_resource.h>
#include <coffee/core/CEnvironment>
#include <coffee/core/CFiles>
#include <peripherals/libc/signals.h>

#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace Net {

using namespace ::semantic;

void ProfilingExport()
{
    using http::response_class;
    using http::header::classify_status;
    using namespace platform::url::constructors;

    if constexpr(compile_info::lowfat_mode || compile_info::release_mode)
        return;

    cVerbose(10, "Checking for network profiling...");

    const constexpr cstring network_server = "COFFEE_REPORT_URL";
    if(Env::ExistsVar(network_server))
    {
        cVerbose(10, "Network export starting");

        State::SwapState("jsonProfiler", {});

        auto profilerState = State::GetProfilerStore();

        if(profilerState)
            profilerState->disable();

        auto worker = ASIO::GenWorker();

        auto ctxt = worker ? worker->context : ASIO::InitService();

        Coffee::Resource profile("profile.json", RSCA::TempFile);

        auto netServerUrl = Env::GetVar(network_server);

        auto reportBin = Net::MkUrl(
            netServerUrl.c_str(),
            HTTPAccess::DefaultPOST | HTTPAccess::NoVerify);
        Net::Resource reportBinRsc(ctxt, reportBin);

        if(!reportBinRsc.connected())
        {
            cDebug(
                "Failed to connect to {0}: {1}",
                reportBinRsc.resource(),
                reportBinRsc.errorCode().message());
            return;
        }

        reportBinRsc.setHeaderField(
            http::header_field::content_type,
            http::header::to_string::content_type(http::content_type::json));
        reportBinRsc.setHeaderField(
            http::header_field::accept,
            http::header::to_string::content_type(http::content_type::json));

        reportBinRsc.setHeaderField(
            "X-Coffee-Token", "token " + Env::GetVar("COFFEE_REPORT_ID"));

        Bytes chromeData;

        if(FileExists(profile))
            chromeData = C_OCAST<Bytes>(profile);
        else
        {
            CString target_chrome;
            Profiling::ExportChromeTracerData(target_chrome);
            chromeData = Bytes::CreateString(target_chrome.c_str());
        }

        reportBinRsc.push(chromeData);

        auto response_status = classify_status(reportBinRsc.responseCode());
        if(response_status != response_class::success)
        {
            cWarning(
                "Got bad response from server: {1}\n{0}",
                str::encapsulate(
                    (cstring)reportBinRsc.data().data,
                    reportBinRsc.data().size),
                reportBinRsc.responseCode());
        } else
        {
            auto data = reportBinRsc.data();
            cVerbose(
                10,
                "Network export successful with response: {0}",
                str::encapsulate((cstring)data.data, data.size));
        }

        worker->stop();

    } else
        cVerbose(10, "Network export cancelled");
}

#if !defined(COFFEE_APPLE)
int RegisterProfiling()
{
    cVerbose(10, "Registering network profiling");
    libc::signal::register_atexit(ProfilingExport);
    return 0;
}
#endif

} // namespace Net
} // namespace Coffee
