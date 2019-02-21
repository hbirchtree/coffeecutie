#include <coffee/core/profiler/profiling-export.h>

#include <coffee/asio/asio_worker.h>
#include <coffee/asio/net_resource.h>
#include <coffee/core/CEnvironment>
#include <peripherals/libc/signals.h>

#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace Net {

using namespace ::semantic;

void ProfilingExport()
{
#if !defined(COFFEE_LOWFAT) && MODE_DEBUG
    cVerbose(10, "Checking for network profiling...");

    const constexpr cstring network_server = "COFFEE_REPORT_URL";
    if(Env::ExistsVar(network_server))
    {
        cVerbose(10, "Network export starting");

        ApplyIfValid(
            State::GetProfilerStore(),
            [](platform::profiling::PContext* context) { context->disable(); });

        State::SwapState(ASIO::context_name, {});

        auto worker = ASIO::GenWorker();

        auto ctxt = worker->context;

        CString target_chrome;
        Profiling::ExportChromeTracerData(target_chrome);

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

        auto chromeData = Bytes::CreateString(target_chrome.c_str());

        reportBinRsc.push(chromeData);

        if(reportBinRsc.responseCode() != 200)
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
#endif
}

void RegisterProfiling()
{
    libc::signal::register_atexit(ProfilingExport);
}

} // namespace Net
} // namespace Coffee
