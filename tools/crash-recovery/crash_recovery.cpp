#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/argument_handling.h>
#include <coffee/core/datastorage/text/json/cjsonparser.h>
#include <coffee/core/libc_types.h>
#include <peripherals/libc/signals.h>
#include <peripherals/posix/process.h>
#include <platforms/environment.h>
#include <platforms/posix/file.h>

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/asio_worker.h>
#include <coffee/asio/http.h>
#include <coffee/asio/net_resource.h>
#include <coffee/ssl/hmac.h>
#endif

#include <coffee/strings/libc_types.h>

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

using namespace ::Coffee;

CString read_output(posix::fd_t stream)
{
    size_t numBytes = 0;
    ioctl(stream, FIONREAD, &numBytes);

    CString out(numBytes, '\0');
    ::read(stream, &out[0], numBytes);

    return out;
}

i32 crash_main(i32, cstring_w*)
{
    using libc::signal::sig;
    using platform::file::file_error;
    using platform::url::Url;
    using platform::url::constructors::MkUrl;
    using namespace libc;
    using namespace posix;

    CString stdoutBuf;
    CString stderrBuf;

    errno = 0;

    auto args = GetInitArgs().arguments();

    args.push_back(nullptr);

    if(args.size() < 2)
    {
        cOutputPrint("No program specified");
        cOutputPrint(" - Specify CRASH_API to submit reports");
        cOutputPrint(" - Specify CRASH_APITRACE to enable apitrace");
        cOutputPrint(" - Specify CRASH_WORKING_DIR to change working directory "
                     "of child");
        return -1;
    }

    Vector<CString> apitrace_args;

    posix_ec   ec;
    file_error fec;
    Url        workingDir;

    if(platform::Env::ExistsVar("CRASH_APITRACE"))
    {
        apitrace_args.push_back(platform::Env::GetVar("CRASH_APITRACE"));
        apitrace_args.push_back("trace");

        for(auto it = apitrace_args.rbegin(); it != apitrace_args.rend(); ++it)
            args.insert(args.begin(), C_CCAST<cstring_w>(it->c_str()));
    }

    if(platform::Env::ExistsVar("CRASH_WORKING_DIR"))
        workingDir = MkUrl(platform::Env::GetVar("CRASH_WORKING_DIR"));

#if defined(COFFEE_APPLE)
    if(!workingDir.isLocal())
        workingDir =
            platform::file::DirFun::Dirname(
                *platform::file::DirFun::Dirname(args.at(0), fec), fec) +
            platform::url::Path("Resources");
#endif

    cDebug("Spawning child");
    auto spawnInfo = posix::proc::spawn<char*>({args.at(0), args, workingDir});

    posix::fd::close(STDIN_FILENO, ec);
    C_ERROR_CHECK(ec)
    posix::fd::replace_fd(spawnInfo.in, STDIN_FILENO, ec);
    C_ERROR_CHECK(ec)

    Vector<pollfd> handles(2);

    handles.push_back({spawnInfo.out, POLL_IN, 0});
    handles.push_back({spawnInfo.err, POLL_IN, 0});

    int exitCode = -1;

    while(!posix::proc::is_exited(spawnInfo.child, &exitCode))
    {
        int ret = poll(handles.data(), handles.size(), 500);

        if(ret > 0)
        {
            for(pollfd& fd : handles)
            {
                if(fd.revents & POLL_IN)
                {
                    CString* buf = nullptr;

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
        posix::proc::wait_for(posix::proc::wait_by::any, ec, 0, &exitCode);
        C_ERROR_CHECK(ec);
    }

    cDebug("Child exited with: {0}", posix::proc::code_to_string(exitCode));

    if(exitCode == 0)
        return 0;

    if(!platform::Env::ExistsVar("CRASH_API"))
    {
        cDebug("CRASH_API not set, nowhere to submit to");
        return 1;
    }

    using Coffee::JSON;

    http::multipart::builder multipart("-----CrashRecovery");

    Url profileLocation, machineProfileLocation, stacktraceLocation;

    {
        auto appNameIdx = stdoutBuf.find(",");

        if(appNameIdx != CString::npos)
        {
            auto appName = stdoutBuf.substr(0, appNameIdx);

            profileLocation = MkUrl(
                Path("..") / appName / "profile.json",
                semantic::RSCA::TempFile);
            machineProfileLocation = MkUrl(
                Path("..") / appName /
                    (DirFun::Basename(args.at(0), fec).internUrl +
                     "-chrome.json"),
                semantic::RSCA::TempFile);
            stacktraceLocation = MkUrl(
                Path("..") / appName / "stacktrace.json",
                semantic::RSCA::TempFile);
        }
    }

    if(platform::file::FileFun::Exists(profileLocation, fec))
    {
        cDebug("Located profile: {0}", *profileLocation);
        auto profileResource = Resource(profileLocation);

        multipart.add(
            "profile",
            C_OCAST<Bytes>(profileResource),
            {{"Content-Type", "text/plain"}});
    }

    if(platform::file::FileFun::Exists(machineProfileLocation, fec))
    {
        cDebug("Located machine profile: {0}", *machineProfileLocation);
        auto machineProfile = Resource(machineProfileLocation);

        multipart.add(
            "machineProfile",
            C_OCAST<Bytes>(machineProfile),
            {{"Content-Type", "text/plain"}});
    }

    if(platform::file::FileFun::Exists(stacktraceLocation, fec))
    {
        cDebug("Located crash stacktrace: {0}", *stacktraceLocation);
        auto stacktrace = Resource(stacktraceLocation);

        multipart.add(
            "stacktrace",
            C_OCAST<Bytes>(stacktrace),
            {{"Content-Type", "text/plain"}});
    }

    multipart.add(
        "exitCode",
        Bytes::CreateString(cast_pod(exitCode)),
        {{"Content-Type", "text/plain"}});
    multipart.add(
        "stdout",
        Bytes::CreateString(stdoutBuf),
        {{"Content-Type", "text/plain"}});
    multipart.add(
        "stderr",
        Bytes::CreateString(stderrBuf),
        {{"Content-Type", "text/plain"}});

    multipart.finalize();

    cDebug(
        "Payload: {0} bytes ({1} MB)",
        multipart.m_data.size(),
        multipart.m_data.size() / 1_MB);

    auto worker = ASIO::GenWorker();

    Net::Resource crashPush(
        worker->context, Net::MkUrl(platform::Env::GetVar("CRASH_API")));

    crashPush.setHeaderField(
        http::header_field::content_type, multipart.content_type());

    crashPush.setHeaderField(
        "X-Coffee-Signature",
        "sha1=" + hex::encode(net::hmac::digest(
                      semantic::Bytes::CreateString(multipart.m_data),
                      platform::Env::Var("COFFEE_HMAC_KEY").value_or("0000"))));

    if(!crashPush.push(http::method_t::post, multipart))
    {
        auto responseData = crashPush.data();
        cWarning("Failed to push crash report: {0}", crashPush.responseCode());
        cWarning("{0}", CString(responseData.begin(), responseData.end()));
    } else
    {
        auto responseData = crashPush.data();
        cDebug("{0}", CString(responseData.begin(), responseData.end()));

        auto const& headers = crashPush.response().header.standard_fields;

        if(auto loc = headers.find(http::header_field::location);
           loc != headers.end())
        {
            cDebug(" >> Crash report available at: {0}", loc->second);
        }
    }

    worker->stop();

    return exitCode;
}

COFFEE_APPLICATION_MAIN_CUSTOM_ARG(crash_main)
