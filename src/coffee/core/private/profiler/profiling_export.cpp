#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/CFiles>
#include <coffee/core/CJSONParser>
#include <coffee/core/CProfiling>
#include <coffee/core/argument_handling.h>
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
    for(auto const& info : ExtraData::Get())
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

    runtime.put("architecture", platform::info::os::architecture())
        .put("kernel", platform::info::os::kernel())
        .put("kernelVersion", platform::info::os::kernel_version())
        .put("cwd", AnonymizePath(cwd.value()));

    if(auto sysname = platform::info::os::name())
        if(auto sysver = platform::info::os::version())
        {
            stl_types::String system_string(sysname->begin(), sysname->end());
            system_string.append(" ");
            system_string.append(sysver->begin(), sysver->end());
            runtime.put("system", system_string);
        }

    if constexpr(platform::info::os::has_libc_info)
        runtime.put("libcVersion", platform::info::os::libc_version());

    if(auto distro = platform::info::os::name(); distro)
        runtime.put("distro", *distro);
    if(auto version = platform::info::os::version(); version)
        runtime.put("distroVersion", *version);

#if defined(COFFEE_ANDROID)
//    AndroidForeignCommand cmd;
//    cmd.type = Android_QueryAPI;
//    CoffeeForeignSignalHandleNA(
//        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    runtime.put("androidLevel", cast_pod(android::app_info().sdk_version()));
#endif

    {
        json::ArrayBuilder args(target.allocator());
        PutArgs(args);

        runtime.put("arguments", args.eject());
    }

    target.put("runtime", runtime.eject());

    auto unknown_pair = std::pair{"<unknown>", "<unknown>"};

    {
        json::ObjectBuilder device(target.allocator());

        auto deviceName =
            platform::info::device::device().value_or(unknown_pair);
        auto motherboard =
            platform::info::device::motherboard().value_or(unknown_pair);
        auto chassis = platform::info::device::chassis().value_or(unknown_pair);

        device.put("name", deviceName.first + " " + deviceName.second)
            .put("machineManufacturer", deviceName.first)
            .put("machineModel", deviceName.second)
            .put("type", platform::info::device::variant())
            .put("platform", platform::info::os::variant())
            .put("motherboard", motherboard.first + " " + motherboard.second)
            .put("motherboardManufacturer", motherboard.first)
            .put("motherboardModel", motherboard.second)
            .put("chassis", chassis.first + " " + chassis.second);

        target.put("device", device.eject());
    }

    {
        json::ObjectBuilder processor(target.allocator());
        json::ArrayBuilder  freq(target.allocator());
        json::ArrayBuilder  clusters(target.allocator());

        for(auto i : stl_types::Range<u32>(platform::info::proc::cpu_count()))
        {
            json::ObjectBuilder model_info(target.allocator());
            auto frequency = platform::info::proc::frequency(false, i);
            freq.push_back(frequency / 1000000.f);
            if(auto model = platform::info::proc::model(i); model.has_value())
            {
                model_info.put("id", i);
                model_info.put("manufacturer", model.value().first);
                model_info.put("model", model.value().second);
                model_info.put("frequency", frequency);
                model_info.put("cores", platform::info::proc::core_count(i));
                model_info.put("threads", platform::info::proc::thread_count(i));
            }
            if(!model_info.empty())
                clusters.push_back(model_info.eject());
        }

        auto pinfo = platform::info::proc::model().value_or(unknown_pair);

        processor
            // Legacy CPU info
            .put("frequencies", freq.eject())
            .put("manufacturer", pinfo.first)
            .put("model", pinfo.second)
            .put("firmware", "")
            // New CPU info
            .put("clusters", clusters.eject())
            .put("nodes", platform::info::proc::node_count())
            .put("cpus", platform::info::proc::cpu_count())
            .put("cores", platform::info::proc::core_count())
            .put("threads", platform::info::proc::thread_count())
            .put("hyperthreading", platform::info::proc::is_hyperthreaded());

        target.put("processor", processor.eject());
    }

    {
        json::ObjectBuilder memory(target.allocator());
        json::ObjectBuilder virtmem(target.allocator());
        //        runtime.put("virtual", virtmem.eject());
        //        virtmem.put("total", SysInfo::SwapTotal());
        //        virtmem.put("available", SysInfo::SwapAvailable());

        //        memory.put("bank", SysInfo::MemTotal())

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

    json::ArrayBuilder emptyEvents(doc.GetAllocator());
    doc.AddMember("traceEvents", emptyEvents.eject(), doc.GetAllocator());

    target = json::Serialize(doc);
}

void ExportStringToFile(const CString& data, const Url& outfile)
{
    if constexpr(compile_info::lowfat_mode)
        return;

    cVerbose(6, "Creating filename");
    Resource out(outfile);
    out = mem_chunk<const byte_t>::ofContainer(data);
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
