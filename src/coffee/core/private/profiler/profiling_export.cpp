#include "peripherals/constants.h"
#include "peripherals/stl/standard_exceptions.h"
#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/argument_handling.h>
#include <coffee/core/coffee.h>
#include <coffee/core/internal_state.h>
#include <coffee/core/platform_data.h>
#include <fmt_extensions/info.h>
#include <fmt_extensions/url_types.h>
#include <nlohmann/json_fwd.hpp>
#include <peripherals/stl/range.h>
#include <peripherals/stl/string/hex.h>
#include <peripherals/stl/string/replace.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/string_ops.h>
#include <platforms/environment.h>
#include <platforms/file.h>
#include <platforms/sysinfo.h>
#include <url/url.h>

#include <nlohmann/json.hpp>

#if defined(COFFEE_ANDROID)
#include <coffee/android/android_main.h>
#endif

#if defined(COFFEE_WINDOWS)
#include <peripherals/platform/windows.h>
#if !defined(COFFEE_MINGW64) && !defined(COFFEE_MINGW32)
#include <VersionHelpers.h>
#endif
#endif

#include <coffee/core/CDebug>

using json = nlohmann::json;

#ifndef COFFEE_LOWFAT

using namespace ::platform;

namespace Coffee {
namespace Profiling {

using namespace ::platform::profiling;

static std::string AnonymizePath(std::string const& p)
{
    if(auto home = env::home_dir(); home.has_value())
        return str::replace::str<char>(
            str::replace::str<char>(p, home.value().internUrl, "~"),
            env::var("USER").value_or(""),
            "user");
    return p;
}

static std::string AnonymizePath(Url const& p)
{
    return AnonymizePath(p.internUrl);
}

STATICINLINE void PutRuntimeInfo(json& target)
{
    using namespace stl_types;
    using libc_types::u32;
    json build;
    build["version"] = compile_info::engine_version;
    build["compiler"] = compile_info::compiler::name;
    build["compilerVersion"] = Strings::fmt(
        "{0}.{1}.{2}",
        compile_info::compiler::version.major,
        compile_info::compiler::version.minor,
        compile_info::compiler::version.rev);
    build["architecture"] = compile_info::architecture;
    build["target"] = compile_info::target;
    build["buildMode"] = compile_info::debug_mode ? "DEBUG" : "RELEASE";
    if constexpr(compile_info::platform::is_android)
    {
        build["androidTarget"] = compile_info::android::api;
        build["androidNdk"] = compile_info::android::ndk_ver;
    }
    if constexpr(compile_info::platform::is_windows)
    {
        build["windowsTarget"] = fmt::format("{:x}", compile_info::windows::target);
        build["windowsWdk"] = fmt::format("{:x}", compile_info::windows::wdk);
#if defined(COFFEE_WINDOWS) && !defined(COFFEE_MINGW64) && \
    !defined(COFFEE_MINGW32)
        build["windowsServer"] = IsWindowsServer() ? true : false;
#endif
    }
    if constexpr(compile_info::platform::is_macos)
    {
        build["macTarget"] = compile_info::apple::macos::target;
        build["macMinTarget"] = compile_info::apple::macos::min_target;
    }
    if constexpr(compile_info::platform::is_ios)
    {
        build["iosTarget"] = compile_info::apple::ios::target;
        build["iosMinTarget"] = compile_info::apple::ios::min_target;
    }
    if constexpr(compile_info::platform::is_linux)
    {
        build["libcRuntime"] = compile_info::linux_::libc_runtime;

        if constexpr(compile_info::linux_::glibc::major != 0)
        {
            build["libcVersion"] = fmt::format(
                "{}.{}",
                compile_info::linux_::glibc::major,
                compile_info::linux_::glibc::minor);
        } else if constexpr(compile_info::linux_::libcpp::version != 0)
        {
            build["libcVersion"] = compile_info::linux_::libcpp::version;
        }
    }
    target["build"] = std::move(build);

    json runtime;
    runtime["architecture"] = platform::info::os::architecture();
    runtime["kernel"] = platform::info::os::kernel();
    runtime["kernelVersion"] = platform::info::os::kernel_version();
    if(auto cwd = path::current_dir())
        runtime["cwd"] = AnonymizePath(cwd.value());
    if(auto distro = platform::info::os::name(); distro)
    {
        runtime["distro"] = *distro;
        if(auto version = platform::info::os::version(); version)
        {
            runtime["distroVersion"] = *version;
            runtime["system"] = fmt::format("{} {}", *distro, *version);
        }
    }
    if constexpr(platform::info::os::has_libc_info)
        runtime["libcVersion"] = platform::info::os::libc_version();
#if defined(COFFEE_WINDOWS)
    if(auto system = platform::info::os::wine_host_system())
    {
        auto [kernel, version] = system.value();
        runtime["underlyingKernel"] = kernel;
        runtime["underlyingKernelVersion"] = version;
    }
#endif
#if defined(COFFEE_ANDROID)
    if constexpr(compile_info::platform::is_android)
    {
        runtime["androidLevel"] = android::app_info().sdk_version();

        std::vector<std::string> systemFeatures =
            android::app_info().system_features();
        std::string systemFeaturesString;
        for(auto const& feature : systemFeatures)
        {
            systemFeaturesString.append(feature + " ");
        }
        if(systemFeaturesString.back() == ' ')
            systemFeaturesString.resize(systemFeaturesString.size() - 1);
        runtime["androidFeatures"] = systemFeaturesString;
    }
#endif
#if defined(COFFEE_EMSCRIPTEN)
    if(auto browser = platform::info::os::emscripten::browser_name())
        runtime.put("browserAgent", *browser);
#endif
    {
        json args = nlohmann::json::array();
        for(auto const& arg : GetInitArgs())
            args.push_back(AnonymizePath(arg));
        runtime["arguments"] = std::move(args);
    }
    target["runtime"] = std::move(runtime);

    auto unknown_pair = std::pair{"<unknown>", "<unknown>"};
    {
        json device;
        auto deviceName =
            platform::info::device::device().value_or(unknown_pair);
        auto motherboard =
            platform::info::device::motherboard().value_or(unknown_pair);
        auto chassis = platform::info::device::chassis().value_or(unknown_pair);

        device["name"] = fmt::format("{} {}", deviceName.first, deviceName.second);
        device["machineManufacturer"] = deviceName.first;
        device["machineModel"] = deviceName.second;
        device["type"] = platform::info::device::variant();
        device["platform"] = platform::info::os::variant();
        device["motherboard"] = motherboard.first + " " + motherboard.second;
        device["motherboardManufacturer"] = motherboard.first;
        device["motherboardModel"] = motherboard.second;
        device["chassis"] = fmt::format("{} {}", chassis.first, chassis.second);

        target["device"] = std::move(device);
    }
    {
        json freq = nlohmann::json::array();
        json clusters = nlohmann::json::array();
        for(auto i : stl_types::range<u32>(platform::info::proc::cpu_count()))
        {
            json model_info;
            auto frequency = platform::info::proc::frequency(false, i);
            freq.push_back(frequency / 1000000.f);
            if(auto model = platform::info::proc::model(i); model.has_value())
            {
                model_info["id"] = i;
                model_info["manufacturer"] = model.value().first;
                model_info["model"] = model.value().second;
                model_info["frequency"] = frequency;
                model_info["cores"] = platform::info::proc::core_count(i);
                model_info["threads"] = platform::info::proc::thread_count(i);
            }
            if(!model_info.empty())
                clusters.push_back(std::move(model_info));
        }
        auto pinfo = platform::info::proc::model().value_or(unknown_pair);
        json processor;
        // Legacy CPU info, without clustered CPU support
        processor["frequencies"] = std::move(freq);
        processor["manufacturer"] = pinfo.first;
        processor["model"] = pinfo.second;
        processor["firmware"] = "";
        // New CPU info
        processor["clusters"] = std::move(clusters);
        processor["nodes"] = platform::info::proc::node_count();
        processor["cpus"] = platform::info::proc::cpu_count();
        processor["cores"] = platform::info::proc::core_count();
        processor["threads"] = platform::info::proc::thread_count();
        processor["hyperthreading"] = platform::info::proc::is_hyperthreaded();
        target["processor"] = std::move(processor);
    }
    {
        json memory;
        memory["total"] = platform::info::memory::total();
        if(auto resident = platform::info::memory::resident(); resident != 0)
            memory["resident"] = resident;
        target["memory"] = std::move(memory);
    }
}

void ExportChromeTracerData(std::string& target)
{
    json profile;
    profile["application"] = [] {
        json application;
        try {
            auto appd = GetCurrentApp();
            application["name"] = appd.application_name;
            application["organization"] = appd.organization_name;
            application["version"] = appd.version_code;
        } catch(undefined_behavior const&)
        {
        }
        return application;
    }();
    profile["extra"] = [] {
        json extraData;
        for(auto const& info : ExtraData::Get())
            extraData[info.first] = info.second;
        return extraData;
    }();
    PutRuntimeInfo(profile);
    profile["traceEvents"] = nlohmann::json::array();
    target = nlohmann::to_string(profile);
}

void ExportStringToFile(const std::string& data, const Url& outfile)
{
    if constexpr(compile_info::lowfat_mode)
        return;

    cVerbose(6, "Creating filename");
    Resource out(outfile);
    out = semantic::mem_chunk<const byte_t>::ofContainer(data);
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
    if(auto flag = env::var("COFFEE_NO_PROFILER_EXPORT");
       flag.has_value() && flag.value() == "1")
        return;

    {
        auto log_name = Path(path::executable().value()).fileBasename();

        if constexpr(
            compile_info::platform::is_android ||
            compile_info::platform::is_ios)
            log_name = Path("chrome");

        auto log_url = url::constructors::MkUrl("", RSCA::TemporaryFile);

        auto log_url2 =
            log_url + Path(log_name.internUrl + "-chrome").addExtension("json");

        std::string target_chrome;
        Profiling::ExportChromeTracerData(target_chrome);
        Profiling::ExportStringToFile(target_chrome + " ", log_url2);

        cVerbose(6, "Saved profiler data to: {0}", path::canon(log_url2));
    }
}

} // namespace Profiling
} // namespace Coffee

#endif
