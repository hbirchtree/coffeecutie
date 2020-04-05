#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/CFiles>
#include <coffee/core/CJSONParser>
#include <coffee/core/CProfiling>
#include <coffee/core/argument_handling.h>
#include <coffee/core/base.h>
#include <coffee/core/coffee.h>
#include <coffee/core/internal_state.h>
#include <coffee/core/platform_data.h>
#include <coffee/strings/info.h>
#include <peripherals/stl/string_ops.h>
#include <platforms/environment.h>
#include <platforms/file.h>
#include <platforms/sysinfo.h>
#include <url/url.h>

#if defined(COFFEE_ANDROID)
#include <coffee/android/android_main.h>
#endif

#include <coffee/core/CDebug>

#ifndef COFFEE_LOWFAT

using namespace ::platform;

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
    JSON::Object build;

    build.AddMember(
        "version", FromString(compile_info::engine_version, alloc), alloc);
    build.AddMember(
        "compiler", FromString(compile_info::compiler::name, alloc), alloc);
    build.AddMember(
        "compilerVersion",
        FromString(compile_info::compiler::version_str, alloc),
        alloc);
    build.AddMember(
        "architecture", FromString(compile_info::architecture, alloc), alloc);
    build.AddMember("target", FromString(compile_info::target, alloc), alloc);
    build.AddMember(
        "buildMode",
        FromString(compile_info::debug_mode ? "DEBUG" : "RELEASE", alloc),
        alloc);

    if constexpr(compile_info::platform::is_android)
    {
        build.AddMember(
            "androidTarget",
            FromString(cast_pod(compile_info::android::api), alloc),
            alloc);

#if defined(COFFEE_ANDROID)
        AndroidForeignCommand cmd;
        cmd.type = Android_QueryAPI;
        CoffeeForeignSignalHandleNA(
            CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

        build.AddMember(
            "androidSdkTarget",
            FromString(cast_pod(cmd.data.scalarI64), alloc),
            alloc);
#endif
    }

    if constexpr(compile_info::platform::is_windows)
        build.AddMember("windowsTarget", FromString("", alloc), alloc);

    if constexpr(compile_info::platform::is_macos)
        build.AddMember(
            "macTarget",
            FromString(cast_pod(compile_info::apple::macos::target), alloc),
            alloc);

    if constexpr(compile_info::platform::is_ios)
        build.AddMember(
            "iosTarget",
            FromString(cast_pod(compile_info::apple::ios::target), alloc),
            alloc);

    target.AddMember("build", build, alloc);

    JSON::Object runtime;

    runtime.AddMember(
        "system",
        FromString(PlatformData::SystemDisplayString(), alloc),
        alloc);
    runtime.AddMember(
        "distro",
        FromString(platform::info::device::system::runtime_distro(), alloc),
        alloc);
    runtime.AddMember(
        "distroVersion", FromString(SysInfo::GetSystemVersion(), alloc), alloc);
    runtime.AddMember(
        "architecture",
        FromString(platform::info::device::system::runtime_arch(), alloc),
        alloc);
    runtime.AddMember(
        "kernel",
        FromString(platform::info::device::system::runtime_kernel(), alloc),
        alloc);
    runtime.AddMember(
        "kernelVersion",
        FromString(
            platform::info::device::system::runtime_kernel_version(), alloc),
        alloc);

    auto cwd = Env::CurrentDir();
    runtime.AddMember("cwd", FromString(AnonymizePath(cwd), alloc), alloc);

    {
        JSON::Array args;
        PutArgs(args, alloc);

        runtime.AddMember("arguments", args, alloc);
    }

    target.AddMember("runtime", runtime, alloc);

    JSON::Object device;

    device.AddMember(
        "name",
        FromString(Strings::to_string(SysInfo::DeviceName()), alloc),
        alloc);
    device.AddMember("dpi", PlatformData::DeviceDPI(), alloc);
    device.AddMember("type", PlatformData::DeviceVariant(), alloc);
    device.AddMember("platform", PlatformData::PlatformVariant(), alloc);

    auto motherboard = SysInfo::Motherboard();

    device.AddMember(
        "motherboardVersion",
        FromString(Strings::to_string(motherboard.firmware), alloc),
        alloc);

    device.AddMember(
        "motherboard",
        FromString(
            Strings::to_string(platform::info::HardwareDevice(
                motherboard.manufacturer, motherboard.model, {})),
            alloc),
        alloc);

    device.AddMember(
        "chassis",
        FromString(Strings::to_string(SysInfo::Chassis()), alloc),
        alloc);

    if constexpr(compile_info::internal_build)
        device.AddMember(
            "hostname",
            FromString(Strings::to_string(SysInfo::HostName()), alloc),
            alloc);

    auto deviceName = SysInfo::DeviceName();
    device.AddMember(
        "machineManufacturer",
        FromString(deviceName.manufacturer, alloc),
        alloc);
    device.AddMember(
        "machineModel", FromString(deviceName.model, alloc), alloc);

    target.AddMember("device", device, alloc);

    JSON::Object processor;

    auto pinfo = SysInfo::Processor();
    processor.AddMember(
        "manufacturer", FromString(pinfo.manufacturer, alloc), alloc);
    processor.AddMember("model", FromString(pinfo.model, alloc), alloc);
    processor.AddMember("firmware", FromString(pinfo.firmware, alloc), alloc);

    {
        auto freqs = SysInfo::ProcessorFrequencies();

        JSON::Array freq_j;
        if(freqs.size())
            for(auto f : freqs)
                freq_j.PushBack(f, alloc);
        else
            freq_j.PushBack(SysInfo::ProcessorFrequency(), alloc);

        processor.AddMember("frequencies", freq_j, alloc);
    }

    processor.AddMember<u64>("cores", SysInfo::CoreCount(), alloc);
    processor.AddMember("threads", SysInfo::ThreadCount(), alloc);

    processor.AddMember("hyperthreading", SysInfo::HasHyperThreading(), alloc);
    processor.AddMember("pae", SysInfo::HasPAE(), alloc);
    processor.AddMember("fpu", SysInfo::HasFPU(), alloc);

    target.AddMember("processor", processor, alloc);

    {
        JSON::Object memory;

        memory.AddMember("bank", SysInfo::MemTotal(), alloc);

        JSON::Object virtmem;
        virtmem.AddMember("total", SysInfo::SwapTotal(), alloc);
        virtmem.AddMember("available", SysInfo::SwapAvailable(), alloc);

        memory.AddMember("virtual", virtmem, alloc);

        target.AddMember("memory", memory, alloc);
    }
}

} // namespace CT_Stuff

void ExportChromeTracerData(CString& target)
{
    using namespace CT_Stuff;

    JSON::Document doc;
    doc.SetObject();

    auto& alloc = doc.GetAllocator();

    JSON::Array events;
    PutEvents(events, doc.GetAllocator());

    JSON::Object extraData;
    PutExtraData(extraData, doc.GetAllocator());

    JSON::Object application;

    auto appd = GetCurrentApp();
    application.AddMember(
        "name", FromString(appd.application_name, alloc), alloc);
    application.AddMember(
        "organization", FromString(appd.organization_name, alloc), alloc);
    application.AddMember<u64>("version", appd.version_code, alloc);

    doc.AddMember("application", application, alloc);
    PutRuntimeInfo(doc, alloc);
    doc.AddMember("extra", extraData, alloc);
    doc.AddMember("traceEvents", events, alloc);

    target = JSON::Serialize(doc);
}

void ExportStringToFile(const CString& data, const Url& outfile)
{
    if constexpr(compile_info::lowfat_mode)
        return;

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
    if constexpr(!compile_info::debug_mode)
        return;

    auto profilerStore = State::GetProfilerStore();

    if(profilerStore)
        profilerStore->disable();

    file::file_error ec;

    /* Verify if we should export profiler data */
    {
        const constexpr cstring disable_flag = "COFFEE_NO_PROFILER_EXPORT";
        if(!(Env::ExistsVar(disable_flag) && Env::GetVar(disable_flag) == "1"))
        {
            auto log_name = (Path{Env::ExecutableName()}.fileBasename());

            if constexpr(
                compile_info::platform::is_android ||
                compile_info::platform::is_ios)
                log_name = Path("chrome");

            auto log_url = url::constructors::MkUrl("", RSCA::TemporaryFile);

            auto log_url2 =
                log_url +
                Path(log_name.internUrl + "-chrome").addExtension("json");

            CString target_chrome;
            Profiling::ExportChromeTracerData(target_chrome);
            Profiling::ExportStringToFile(target_chrome + " ", log_url2);

            cVerbose(
                6,
                "Saved profiler data to: {0}",
                file::FileFun::CanonicalName(log_url2, ec));
        }
    }
}

} // namespace Profiling
} // namespace Coffee

#endif
