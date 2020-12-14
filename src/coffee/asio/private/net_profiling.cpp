#include <coffee/core/profiler/profiling-export.h>

#include <coffee/asio/asio_worker.h>
#include <coffee/asio/net_resource.h>
#include <coffee/core/CEnvironment>
#include <coffee/core/CFiles>
#include <coffee/ssl/hmac.h>
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

        if(Env::ExistsVar("COFFEE_REPORT_ID"))
            reportBinRsc.setHeaderField(
                "X-Coffee-Token", "token " + Env::GetVar("COFFEE_REPORT_ID"));
        reportBinRsc.setHeaderField(
            "X-Coffee-Signature",
            "sha1=" + hex::encode(net::hmac::digest(
                          C_OCAST<Bytes>(profile),
                          Env::Var("COFFEE_HMAC_KEY").value_or("0000"))));

        http::multipart::builder out("-----------NetProfile");

        reportBinRsc.setHeaderField(
            http::header_field::content_type, out.content_type());
        reportBinRsc.setHeaderField(
            http::header_field::accept,
            http::header::to_string::content_type(http::content_type::json));

        CString target_chrome;
        Profiling::ExportChromeTracerData(target_chrome);

        out.add(
            "machineProfile",
            Bytes::CreateString(target_chrome),
            {{"Content-Type", "text/plain"}});
        if(FileExists(profile))
        {
            out.add(
                "profile",
                C_OCAST<Bytes>(profile),
                {{"Content-Type", "text/plain"}});
        } else
        {
            out.add(
                "profile",
                Bytes::CreateString(target_chrome.c_str()),
                {{"Content-Type", "text/plain"}});
        }

        out.finalize();

        reportBinRsc.push(out);

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

int RegisterProfilingAtExit()
{
    cVerbose(10, "Registering network profiling");
    libc::signal::register_atexit(ProfilingExport);
    return 0;
}

} // namespace Net
} // namespace Coffee
