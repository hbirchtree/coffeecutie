#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/CFiles>
#include <coffee/core/CJSONParser>
#include <coffee/core/CProfiling>
#include <coffee/core/base.h>
#include <coffee/core/coffee.h>
#include <platforms/environment.h>
#include <platforms/file.h>
#include <url/url.h>

#include <coffee/core/CDebug>

#ifndef COFFEE_LOWFAT

namespace Coffee {
namespace Profiling {

using namespace ::platform::profiling;

static Vector<DataPoint> GetSortedDataPoints()
{
    Vector<DataPoint> points;

    return points;
}

static CString AnonymizePath(CString const& p)
{
    return str::replace::str<char>(
        str::replace::str<char>(p, Env::GetUserHome().internUrl, "~"),
        Env::ExistsVar("USER") ? Env::GetVar("USER") : "",
        "user");
}

static CString AnonymizePath(Url const& p)
{
    return AnonymizePath(p.internUrl);
}

void PrintProfilerData()
{
}

void ExportProfilerData(CString&)
{
}

namespace CT_Stuff {

STATICINLINE JSON::Value FromString(
    CString const& s, JSON::Document::AllocatorType& alloc)
{
    JSON::Value j;
    j.SetString(s.c_str(), alloc);

    return j;
}

STATICINLINE void PutEvents(
    JSON::Value& target, JSON::Document::AllocatorType& alloc)
{
    /* Some parsing information */
    for(Profiling::DataPoint const& p : GetSortedDataPoints())
    {
        JSON::Value o;
        o.SetObject();

        CString tid = str::print::pointerify(p.tid);

        if(ThreadGetName(p.tid).size())
            tid = ThreadGetName(p.tid);

        auto catVal  = FromString(p.component, alloc);
        auto tidVal  = FromString(tid, alloc);
        auto nameVal = FromString(p.name, alloc);

        //        o.AddMember(
        //            "ts",
        //            JSON::Value(
        //                Chrono::duration_cast<Chrono::microseconds>(p.ts).count()
        //                - start),
        //            alloc);

        o.AddMember("name", nameVal, alloc);
        o.AddMember("pid", JSON::Value(1), alloc);
        o.AddMember("tid", tidVal, alloc);
        o.AddMember("cat", catVal, alloc);

        switch(p.flags.type)
        {
        case Profiling::DataPoint::Profile:
        {
            //            if(feval(p.at & Profiling::DataPoint::Hot))
            //                o.AddMember("ph", "P", alloc);
            //            else
            o.AddMember("ph", "i", alloc);

            o.AddMember("s", "t", alloc);
            break;
        }
        case Profiling::DataPoint::Push:
        {
            o.AddMember("ph", "B", alloc);
            break;
        }
        case Profiling::DataPoint::Pop:
        {
            o.AddMember("ph", "E", alloc);
            break;
        }
        case DataPoint::Complete:
        {
            o.AddMember("ph", "X", alloc);
            break;
        }
        }

        target.PushBack(o, alloc);
    }
}

STATICINLINE void PutArgs(
    JSON::Value& target, JSON::Document::AllocatorType& alloc)
{
    auto args = GetInitArgs().originalArguments();

    for(auto const& arg : args)
    {
        target.PushBack(FromString(AnonymizePath(arg), alloc), alloc);
    }
}

STATICINLINE void PutExtraData(
    JSON::Value& target, JSON::Document::AllocatorType& alloc)
{
    for(auto info : ExtraData::Get())
    {
        target.AddMember(
            FromString(info.first, alloc),
            FromString(info.second, alloc),
            alloc);
    }
}

STATICINLINE void PutRuntimeInfo(
    JSON::Value& target, JSON::Document::AllocatorType& alloc)
{
    auto const& buildi = State::GetBuildInfo();

    target.AddMember(
        "build.version", FromString(buildi.build_version, alloc), alloc);
    target.AddMember(
        "build.compiler", FromString(buildi.compiler, alloc), alloc);
    target.AddMember(
        "build.architecture", FromString(buildi.architecture, alloc), alloc);

    target.AddMember(
        "runtime.system",
        FromString(PlatformData::SystemDisplayString(), alloc),
        alloc);
    target.AddMember(
        "device",
        FromString(Strings::to_string(SysInfo::DeviceName()), alloc),
        alloc);

    target.AddMember("device.dpi", PlatformData::DeviceDPI(), alloc);
    target.AddMember("device.type", PlatformData::DeviceVariant(), alloc);
    target.AddMember("device.platform", PlatformData::PlatformVariant(), alloc);
    target.AddMember("device.debug", PlatformData::IsDebug(), alloc);

    target.AddMember(
        "device.version",
        FromString(Strings::to_string(SysInfo::GetSystemVersion()), alloc),
        alloc);
    target.AddMember(
        "device.motherboard",
        FromString(Strings::to_string(SysInfo::Motherboard()), alloc),
        alloc);
    target.AddMember(
        "device.chassis",
        FromString(Strings::to_string(SysInfo::Chassis()), alloc),
        alloc);

#if defined(COFFEE_INTERNAL_BUILD)
    target.AddMember(
        "device.hostname",
        FromString(Strings::to_string(SysInfo::HostName()), alloc),
        alloc);
#endif

    auto pinfo = SysInfo::Processor();
    target.AddMember(
        "processor.manufacturer", FromString(pinfo.manufacturer, alloc), alloc);
    target.AddMember("processor.model", FromString(pinfo.model, alloc), alloc);
    target.AddMember(
        "processor.firmware", FromString(pinfo.firmware, alloc), alloc);

    auto freqs = SysInfo::ProcessorFrequencies();
    if(freqs.size())
    {
        JSON::Value freq_j;
        freq_j.SetArray();
        for(auto f : freqs)
            freq_j.PushBack(f, alloc);
        target.AddMember("processor.frequency", freq_j, alloc);
    } else
        target.AddMember(
            "processor.frequency", SysInfo::ProcessorFrequency(), alloc);

    target.AddMember("processor.cores", SysInfo::CoreCount(), alloc);
    target.AddMember("processor.threads", SysInfo::ThreadCount(), alloc);

    target.AddMember(
        "processor.hyperthreading", SysInfo::HasHyperThreading(), alloc);
    target.AddMember("processor.pae", SysInfo::HasPAE(), alloc);
    target.AddMember("processor.fpu", SysInfo::HasFPU(), alloc);

    target.AddMember("memory.bank", SysInfo::MemTotal(), alloc);
    target.AddMember("memory.virtual.total", SysInfo::SwapTotal(), alloc);
    target.AddMember(
        "memory.virtual.available", SysInfo::SwapAvailable(), alloc);

    auto cwd = Env::CurrentDir();

    target.AddMember(
        "runtime.cwd", FromString(AnonymizePath(cwd), alloc), alloc);
}

} // namespace CT_Stuff

void ExportChromeTracerData(CString& target)
{
    using namespace CT_Stuff;

    JSON::Document doc;
    doc.SetObject();

    JSON::Value events;
    events.SetArray();
    PutEvents(events, doc.GetAllocator());

    JSON::Value args;
    args.SetArray();
    PutArgs(args, doc.GetAllocator());

    JSON::Value extraData;
    extraData.SetObject();
    PutExtraData(extraData, doc.GetAllocator());

    auto appd = ApplicationData();
    doc.AddMember(
        "application.name",
        FromString(appd.application_name, doc.GetAllocator()),
        doc.GetAllocator());
    doc.AddMember(
        "application.org",
        FromString(appd.organization_name, doc.GetAllocator()),
        doc.GetAllocator());
    doc.AddMember(
        "application.version",
        ApplicationData().version_code,
        doc.GetAllocator());
    PutRuntimeInfo(doc, doc.GetAllocator());
    doc.AddMember("runtime.arguments", args, doc.GetAllocator());

    doc.AddMember("extra", extraData, doc.GetAllocator());

    doc.AddMember("traceEvents", events, doc.GetAllocator());

    target = JSON::Serialize(doc);
}

void ExportStringToFile(const CString& data, const Url& outfile)
{
#ifdef COFFEE_LOWFAT
    return;
#endif

    cVerbose(6, "Creating filename");
    Resource out(outfile);
    out.data = C_CCAST<c_ptr>(C_FCAST<c_cptr>(data.c_str()));
    /* -1 because we don't want the null-terminator */
    out.size = C_CAST<szptr>(data.size() - 1);
    cVerbose(6, "Retrieving data pointers");

    if(!FileCommit(out, RSCA::Discard | RSCA::WriteOnly | RSCA::NewFile))
        cWarning("Failed to export string to file");

    cVerbose(6, "Wrote file");
}

void ExitRoutine()
{
#if defined(COFFEE_LOWFAT) || MODE_RELEASE
    return;
#endif

    State::GetProfilerStore()->disable();

    file_error ec;

    /* Verify if we should export profiler data */
    {
        const constexpr cstring disable_flag = "COFFEE_NO_PROFILER_EXPORT";
        if(!(Env::ExistsVar(disable_flag) && Env::GetVar(disable_flag) == "1"))
        {
            auto log_name = (Path{Env::ExecutableName()}.fileBasename());

            auto log_url = MkUrl("", RSCA::TemporaryFile);

            auto log_url2 =
                log_url +
                Path(log_name.internUrl + "-chrome").addExtension("json");

            CString target_chrome;
            Profiling::ExportChromeTracerData(target_chrome);
            Profiling::ExportStringToFile(target_chrome + " ", log_url2);

            cVerbose(
                6,
                "Saved profiler data to: {0}",
                FileFun::CanonicalName(log_url, ec));
        }
    }
}

} // namespace Profiling
} // namespace Coffee

#endif
