#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/argument_handling.h>
#include <coffee/core/datastorage/text/json/json.h>
#include <coffee/core/libc_types.h>
#include <peripherals/libc/signals.h>
#include <peripherals/posix/process.h>
#include <peripherals/stl/string/hex.h>
#include <platforms/environment.h>
#include <platforms/file.h>

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/asio_worker.h>
#include <coffee/asio/http.h>
#include <coffee/asio/net_resource.h>
#include <coffee/ssl/hmac.h>
#endif

#include <coffee/strings/format.h>
#include <coffee/core/CDebug>

#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#endif

using namespace Coffee;
using namespace semantic;
using namespace semantic;
using namespace libc_types::size_literals;

std::string read_output(platform::common::posix::fd_t stream)
{
    size_t numBytes = 0;
    ioctl(stream, FIONREAD, &numBytes);

    std::string out(numBytes, '\0');
    ::read(stream, &out[0], numBytes);

    return out;
}

i32 crash_main(i32, cstring_w*)
{
    using libc::signal::sig;
    using platform::url::Url;
    using platform::url::constructors::MkUrl;
    using namespace libc;
    using namespace platform::common::posix;
    using namespace platform;

    std::string stdoutBuf;
    std::string stderrBuf;

    errno = 0;

    auto args = GetInitArgs();

    if(args.size() < 2)
    {
        cOutputPrint("No program specified");
        cOutputPrint(" - Specify CRASH_API to submit reports");
        cOutputPrint(" - Specify CRASH_APITRACE to enable apitrace");
        cOutputPrint(" - Specify CRASH_WORKING_DIR to change working directory "
                     "of child");
        return -1;
    }

    /* Remove the CrashRecovery arg from the list,
     * everything else is passed to the child */
    args.erase(args.begin());
    args.push_back(nullptr);

    std::vector<std::string> apitrace_args;

    posix_ec ec;
    Url      workingDir;

    if(auto apitrace = env::var("CRASH_APITRACE"); apitrace.has_value())
    {
        apitrace_args.push_back(apitrace.value());
        apitrace_args.push_back("trace");

        for(auto it = apitrace_args.rbegin(); it != apitrace_args.rend(); ++it)
            args.insert(args.begin(), it->c_str());
    }

    if(auto working_dir = env::var("CRASH_WORKING_DIR");
       working_dir.has_value())
        workingDir = MkUrl(working_dir.value());

#if defined(COFFEE_APPLE)
    if(!workingDir.isLocal())
        workingDir = platform::path::dir(
                         platform::path::dir(MkUrl(args.at(0))).value())
                         .value()
                     + platform::url::Path("Resources");
#endif

    cDebug("Spawning child");
    auto spawnInfo = proc::spawn<const char*>({args.at(0), args, workingDir});

    if(auto e = fd::close(STDIN_FILENO))
        Throw(posix_runtime_error(*e));
    if(auto e = fd::replace_fd(spawnInfo.in, STDIN_FILENO))
        Throw(posix_runtime_error(*e));

    std::vector<pollfd> handles(2);

    handles.push_back({spawnInfo.out, POLL_IN, 0});
    handles.push_back({spawnInfo.err, POLL_IN, 0});

    int exitCode = -1;

    while(!proc::is_exited(spawnInfo.child, &exitCode))
    {
        int ret = poll(handles.data(), handles.size(), 500);

        if(ret > 0)
        {
            for(pollfd& fd : handles)
            {
                if(fd.revents & POLL_IN)
                {
                    std::string* buf = nullptr;

                    if(fd.fd == spawnInfo.out)
                        buf = &stdoutBuf;
                    else if(fd.fd == spawnInfo.err)
                        buf = &stderrBuf;

                    auto len = buf->size();
                    buf->append(read_output(fd.fd));

                    if(len < buf->size())
                        fprintf(
                            buf == &stdoutBuf ? stdout : stderr,
                            "%s",
                            &buf->at(len));
                }

                fd.revents = 0;
            }
        } else if(ret == 0)
        {
            continue;
        } else
            break;
    }

    cDebug("Waiting...");
    if(exitCode == -1)
    {
        proc::wait_for(proc::wait_by::any, ec, 0, &exitCode);
        C_ERROR_CHECK(ec);
    }

    cDebug("Child exited with: {0}", proc::code_to_string(exitCode));

    if(exitCode == 0)
        return 0;

    if(!platform::env::var("CRASH_API"))
    {
        cDebug("CRASH_API not set, nowhere to submit to");
        return 1;
    }

    http::multipart::builder multipart("-----CrashRecovery");

    Url profileLocation, machineProfileLocation, stacktraceLocation;

    {
        auto appNameIdx = stdoutBuf.find(",");

        if(appNameIdx != std::string::npos)
        {
            auto appName = stdoutBuf.substr(0, appNameIdx);

            profileLocation
                = (Path("..") / appName / "profile.json").url(RSCA::TempFile);
            machineProfileLocation
                = (Path("..") / appName
                   / (platform::path::base(MkUrl(args.at(0))).value().internUrl
                      + "-chrome.json"))
                      .url(RSCA::TempFile);
            stacktraceLocation = (Path("..") / appName / "stacktrace.json")
                                     .url(RSCA::TempFile);
        }
    }

    if(profileLocation.valid() && platform::file::exists(profileLocation))
    {
        cDebug("Located profile: {0}", *profileLocation);
        auto profileResource = Resource(profileLocation);

        multipart.add(
            "profile",
            C_OCAST<BytesConst>(profileResource),
            {{"Content-Type", "text/plain"}});
    }

    if(machineProfileLocation.valid()
       && platform::file::exists(machineProfileLocation))
    {
        cDebug("Located machine profile: {0}", *machineProfileLocation);
        auto machineProfile = Resource(machineProfileLocation);

        multipart.add(
            "machineProfile",
            C_OCAST<BytesConst>(machineProfile),
            {{"Content-Type", "text/plain"}});
    }

    if(stacktraceLocation.valid() && platform::file::exists(stacktraceLocation))
    {
        cDebug("Located crash stacktrace: {0}", *stacktraceLocation);
        auto stacktrace = Resource(stacktraceLocation);

        multipart.add(
            "stacktrace",
            C_OCAST<BytesConst>(stacktrace),
            {{"Content-Type", "text/plain"}});
    }

    auto exitCodeStr = stl_types::cast_pod(exitCode);
    multipart.add(
        "exitCode",
        BytesConst::ofString(exitCodeStr),
        {{"Content-Type", "text/plain"}});
    multipart.add(
        "stdout",
        BytesConst::ofString(stdoutBuf),
        {{"Content-Type", "text/plain"}});
    multipart.add(
        "stderr",
        BytesConst::ofString(stderrBuf),
        {{"Content-Type", "text/plain"}});

    multipart.finalize();

    cDebug(
        "Payload: {0} bytes ({1} MB)",
        multipart.m_data.size(),
        multipart.m_data.size() / 1_MB);

    auto worker = ASIO::GenWorker();

    net::Resource crashPush(
        worker->context, net::MkUrl(platform::env::var("CRASH_API").value()));

    crashPush.setHeaderField(
        http::header_field::content_type, multipart.content_type());

    crashPush.setHeaderField(
        "X-Coffee-Signature",
        "sha1="
            + hex::encode(net::hmac::digest(
                semantic::Span<char>(multipart.m_data),
                platform::env::var("COFFEE_HMAC_KEY").value_or("0000"))));

    if(auto error = crashPush.push(http::method_t::post, multipart))
    {
        auto responseData = crashPush.data().value();
        cWarning("Failed to push crash report: {0}", crashPush.responseCode());
        cWarning("{0}", std::string(responseData.begin(), responseData.end()));
    } else if(auto response = crashPush.response().value();
              crashPush.responseCode())
    {
        auto responseData = crashPush.data().value();
        cDebug("{0}", std::string(responseData.begin(), responseData.end()));

        auto const& headers = response.header.standard_fields;

        if(auto loc = headers.find(http::header_field::location);
           loc != headers.end())
        {
            cDebug(" >> Crash report available at: {0}", loc->second);
        }
    }

    worker->stop();

    return exitCode;
}

COFFEE_APPLICATION_MAIN_CUSTOM(crash_main, 0x1 | 0x2)
