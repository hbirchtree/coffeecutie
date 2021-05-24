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
#include <coffee/strings/url_types.h>
#include <peripherals/stl/string_ops.h>
#include <platforms/environment.h>
#include <platforms/file.h>
#include <platforms/sysinfo.h>
#include <url/url.h>

#if defined(COFFEE_ANDROID)
#include <coffee/android/android_main.h>
#endif

#if defined(COFFEE_WINDOWS)
#include <peripherals/platform/windows.h>
#if !defined(COFFEE_MINGW64)
#include <VersionHelpers.h>
#endif
#endif

#include <coffee/core/CDebug>

#ifndef COFFEE_LOWFAT

using namespace ::platform;

namespace Coffee {
namespace Profiling {

using namespace ::platform::profiling;

static CString AnonymizePath(CString const& p)
{
    if(auto home = env::home_dir(); home.has_value())
        return str::replace::str<char>(
            str::replace::str<char>(p, home.value().internUrl, "~"),
            env::var("USER").value_or(""),
            "user");
    return p;
}

static CString AnonymizePath(Url const& p)
{
    return AnonymizePath(p.internUrl);
}

namespace CT_Stuff {

STATICINLINE void PutArgs(json::ArrayBuilder& target)
{
    for(auto const& arg : GetInitArgs().originalArguments())
        target.push_back(AnonymizePath(arg));
}

STATICINLINE void PutExtraData(json::ObjectBuilder& target)
{
    for(auto info : ExtraData::Get())
        target.put(info.first, info.second);
}

STATICINLINE void PutRuntimeInfo(json::ObjectBuilder& target)
{
    json::ObjectBuilder build(target.allocator());

    build.put("version", compile_info::engine_version)
        .put("compiler", compile_info::compiler::name)
        .put(
            "compilerVersion",
            Strings::fmt(
                "{0}.{1}.{2}",
                compile_info::compiler::version.major,
                compile_info::compiler::version.minor,
                compile_info::compiler::version.rev))
        .put("architecture", compile_info::architecture)
        .put("target", compile_info::target)
        .put("buildMode", compile_info::debug_mode ? "DEBUG" : "RELEASE");

    if constexpr(compile_info::platform::is_android)
    {
        build.put("androidTarget", cast_pod(compile_info::android::api))
            .put("androidNdk", cast_pod(compile_info::android::ndk_ver));
    }

    if constexpr(compile_info::platform::is_windows)
    {
        build
            .put(
                "windowsTarget",
                stl_types::str::convert::hexify(compile_info::windows::target))
            .put(
                "windowsWdk",
                stl_types::str::convert::hexify(compile_info::windows::wdk))
#if defined(COFFEE_WINDOWS) && !defined(COFFEE_MINGW64)
            .put("windowsServer", IsWindowsServer() ? true : false)
#endif
            ;
    }

    if constexpr(compile_info::platform::is_macos)
    {
        build.put("macTarget", cast_pod(compile_info::apple::macos::target))
            .put(
                "macMinTarget",
                cast_pod(compile_info::apple::macos::min_target));
    }

    if constexpr(compile_info::platform::is_ios)
    {
        build.put("iosTarget", cast_pod(compile_info::apple::ios::target))
            .put(
                "iosMinTarget", cast_pod(compile_info::apple::ios::min_target));
    }

    if constexpr(compile_info::platform::is_linux)
    {
        build.put("libcRuntime", compile_info::linux_::libc_runtime);

        if constexpr(compile_info::linux_::glibc::major != 0)
        {
            build.put(
                "libcVersion",
                cast_pod(compile_info::linux_::glibc::major) + "." +
                    cast_pod(compile_info::linux_::glibc::minor));
        } else if constexpr(compile_info::linux_::libcpp::version != 0)
        {
            build.put(
                "libcVersion", cast_pod(compile_info::linux_::libcpp::version));
        }
    }

    target.put("build", build.eject());

    auto cwd = path::current_dir();

    json::ObjectBuilder runtime(target.allocator());

    runtime.put("system", PlatformData::SystemDisplayString())
        .put("distro", platform::info::device::system::runtime_distro())
        .put("distroVersion", SysInfo::GetSystemVersion())
        .put("architecture", platform::info::device::system::runtime_arch())
        .put("kernel", platform::info::device::system::runtime_kernel())
        .put(
            "kernelVersion",
            platform::info::device::system::runtime_kernel_version())
        .put(
            "libcVersion",
            platform::info::device::system::runtime_libc_version())
        .put("cwd", AnonymizePath(cwd.value()));

#if defined(COFFEE_ANDROID)
    AndroidForeignCommand cmd;
    cmd.type = Android_QueryAPI;
    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    runtime.put("androidLevel", cast_pod(cmd.data.scalarI64));
#endif

    {
        json::ArrayBuilder args(target.allocator());
        PutArgs(args);

        runtime.put("arguments", args.eject());
    }

    target.put("runtime", runtime.eject());

    {
        json::ObjectBuilder device(target.allocator());

        auto motherboard = SysInfo::Motherboard();
        auto deviceName  = SysInfo::DeviceName();

        device.put("name", Strings::to_string(SysInfo::DeviceName()))
            .put("dpi", PlatformData::DeviceDPI())
            .put("type", PlatformData::DeviceVariant())
            .put("platform", PlatformData::PlatformVariant())
            .put(
                "motherboard",
                Strings::to_string(platform::info::HardwareDevice(
                    motherboard.manufacturer, motherboard.model, {})))
            .put("motherboardVersion", Strings::to_string(motherboard.firmware))
            .put("chassis", Strings::to_string(SysInfo::Chassis()))
            .put("machineManufacturer", deviceName.manufacturer)
            .put("machineModel", deviceName.model);

        if constexpr(compile_info::internal_build)
            device.put("hostname", Strings::to_string(SysInfo::HostName()));

        target.put("device", device.eject());
    }

    {
        json::ObjectBuilder processor(target.allocator());
        json::ArrayBuilder  freq(target.allocator());

        auto pinfo = SysInfo::Processor();
        auto freqs = SysInfo::ProcessorFrequencies();

        if(freqs.size())
            for(auto f : freqs)
                freq.push_back(f);
        else
            freq.push_back(SysInfo::ProcessorFrequency());

        processor.put("frequencies", freq.eject())
            .put("manufacturer", pinfo.manufacturer)
            .put("model", pinfo.model)
            .put("firmware", pinfo.firmware)
            .put("cores", SysInfo::CoreCount())
            .put("threads", SysInfo::ThreadCount())
            .put("hyperthreading", SysInfo::HasHyperThreading())
            .put("pae", SysInfo::HasPAE())
            .put("fpu", SysInfo::HasFPU());

        target.put("processor", processor.eject());
    }

    {
        json::ObjectBuilder memory(target.allocator());

        json::ObjectBuilder virtmem(target.allocator());
        virtmem.put("total", SysInfo::SwapTotal());
        virtmem.put("available", SysInfo::SwapAvailable());

        memory.put("bank", SysInfo::MemTotal()).put("virtual", virtmem.eject());

        target.put("memory", memory.eject());
    }
}

} // namespace CT_Stuff

void ExportChromeTracerData(CString& target)
{
    using namespace CT_Stuff;

    json::Document doc;
    doc.SetObject();
    json::ObjectBuilder root(doc.GetAllocator());

    json::ObjectBuilder extraData(root.allocator());
    PutExtraData(extraData);

    json::ObjectBuilder application(root.allocator());

    auto appd = GetCurrentApp();
    application.put("name", appd.application_name)
        .put("organization", appd.organization_name)
        .put("version", appd.version_code);

    root.put("application", application.eject())
        .put("extra", extraData.eject());
    PutRuntimeInfo(root);

    auto rootDoc = root.eject();
    for(auto it = rootDoc.MemberBegin(); it != rootDoc.MemberEnd(); ++it)
        doc.AddMember(it->name, it->value, doc.GetAllocator());

    target = json::Serialize(doc);
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

    /* Verify if we should export profiler data */
    {
        const constexpr cstring disable_flag = "COFFEE_NO_PROFILER_EXPORT";
        if(auto flag = env::var("COFFEE_NO_PROFILER_EXPORT");
           flag.has_value() && flag.value() == "1")
        {
            auto log_name = Path(path::executable().value()).fileBasename();

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

            cVerbose(6, "Saved profiler data to: {0}", path::canon(log_url2));
        }
    }
}

} // namespace Profiling
} // namespace Coffee

#endif
