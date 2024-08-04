#include <oaf/api.h>

#include <oaf/api_system.h>

#if __has_include(<AL/alext.h>)
#include <AL/alext.h>
#endif

#include <coffee/comp_app/subsystems.h>
#include <coffee/comp_app/dummy_plug.h>
#include <coffee/core/debug/formatting.h>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <peripherals/stl/string/hex.h>

namespace oaf {

namespace {

#if !defined(ALC_HRTF_SOFT)
// For Emscripten, these constants are missing
constexpr u32 ALC_HRTF_SOFT                     = 0x1992;
constexpr u32 ALC_HRTF_ID_SOFT                  = 0x1996;
constexpr u32 ALC_DONT_CARE_SOFT                = 0x0002;
constexpr u32 ALC_HRTF_STATUS_SOFT              = 0x1993;
constexpr u32 ALC_NUM_HRTF_SPECIFIERS_SOFT      = 0x1994;
constexpr u32 ALC_HRTF_SPECIFIER_SOFT           = 0x1995;
constexpr u32 ALC_HRTF_DISABLED_SOFT            = 0x0000;
constexpr u32 ALC_HRTF_ENABLED_SOFT             = 0x0001;
constexpr u32 ALC_HRTF_DENIED_SOFT              = 0x0002;
constexpr u32 ALC_HRTF_REQUIRED_SOFT            = 0x0003;
constexpr u32 ALC_HRTF_HEADPHONES_DETECTED_SOFT = 0x0004;
constexpr u32 ALC_HRTF_UNSUPPORTED_FORMAT_SOFT  = 0x0005;
#endif
#if !defined(AL_SOURCE_SPATIALIZE_SOFT)
constexpr u32 AL_SOURCE_SPATIALIZE_SOFT = 0x1214;
constexpr u32 AL_AUTO_SOFT              = 0x0002;
#endif
#if !defined(AL_FORMAT_MONO_FLOAT32)
constexpr u32 AL_FORMAT_MONO_FLOAT32   = 0x10010;
constexpr u32 AL_FORMAT_STEREO_FLOAT32 = 0x10011;
#endif

using LOOPBACKOPENDEVICESOFT = ALCdevice* (*)(const ALCchar*);
using ISRENDERFORMATSUPPORTEDSOFT =
    ALCboolean (*)(ALCdevice*, ALCsizei, ALCenum, ALCenum);
using RENDERSAMPLESSOFT = void (*)(ALCdevice*, ALCvoid*, ALCsizei);

LOOPBACKOPENDEVICESOFT      loopbackOpenDeviceSOFT;
ISRENDERFORMATSUPPORTEDSOFT isRenderFormatSupportedSOFT;
RENDERSAMPLESSOFT           renderSamplesSOFT;

ALCdevice* alcLoopbackOpenDeviceSOFT(const ALCchar* name)
{
    return loopbackOpenDeviceSOFT(name);
}

ALCboolean alcIsRenderFormatSupportedSOFT(
    ALCdevice* device, ALCsizei frequency, ALCenum channels, ALCenum type)
{
    return isRenderFormatSupportedSOFT(device, frequency, channels, type);
}

void alcRenderSamplesSOFT(ALCdevice* device, ALCvoid* buffer, ALCsizei samples)
{
    return renderSamplesSOFT(device, buffer, samples);
}

using GETSTRINGISOFT   = ALCchar* (*)(ALCdevice*, ALCenum, ALCsizei);
using DEVICEPAUSESOFT  = void (*)(ALCdevice*);
using DEVICERESUMESOFT = void (*)(ALCdevice*);

GETSTRINGISOFT   getStringiSOFT;
DEVICEPAUSESOFT  devicePauseSOFT;
DEVICERESUMESOFT deviceResumeSOFT;

const ALCchar* alcGetStringiSOFT(
    ALCdevice* device, ALCenum paramName, ALCsizei index)
{
    return getStringiSOFT(device, paramName, index);
}

void alcDevicePauseSOFT(ALCdevice* device)
{
    devicePauseSOFT(device);
}

void alcDeviceResumeSOFT(ALCdevice* device)
{
    deviceResumeSOFT(device);
}

} // namespace

using namespace Coffee::Logging;

void detail::buffer_dealloc(ALuint buf)
{
    alDeleteBuffers(1, &buf);
}

void detail::source_dealloc(ALuint src)
{
    alDeleteSources(1, &src);
}

void detail::check_error(std::string_view call)
{
    return;

    using stl_types::str::fmt::pointerify;
    ALenum error{AL_NO_ERROR};
    do
    {
        if(error = alGetError(); error != AL_NO_ERROR)
            cWarning("AL Error: {}: {}", call, pointerify(error));
    } while((error != AL_NO_ERROR));
}

ALenum format_t::to_al(const formats_t& formats) const
{
    using fmt_t = Format::format_t;

    if(channels < 1 || channels > 2)
        return AL_NONE;
    if(bits != 8 && bits != 16 && bits != 32)
        return AL_NONE;

    switch(format)
    {
    case fmt_t::pcm:
        return AL_FORMAT_MONO8 + (channels == 1 ? 0 : 2) + (bits == 8 ? 0 : 1);
    case fmt_t::f32:
        if(!formats.float32)
            break;
        return AL_FORMAT_MONO_FLOAT32 + (channels == 1 ? 0 : 1);
#if defined(AL_FORMAT_MONO_MSADPCM_SOFT)
    case fmt_t::ms_adpcm:
        if(!formats.ms_adpcm)
            break;
        return AL_FORMAT_MONO_MSADPCM_SOFT + (channels == 1 ? 0 : 1);
#endif
#if defined(AL_FORMAT_MONO_IMA4)
    case fmt_t::ima_adpcm:
        if(!formats.ima4_adpcm)
            break;
        return AL_FORMAT_MONO_IMA4 + (channels == 1 ? 0 : 1);
#endif
    default:
        break;
    }
    return AL_NONE;
}

void source_t::spatialize_as(spatialize_t v)
{
    if(!m_features.soft.spatialize)
        return;
    alSourcei(
        m_handle,
        enum_to_al(source_property::spatialized),
        v == spatialize_t::mono_only ? AL_AUTO_SOFT
        : v == spatialize_t::never   ? AL_FALSE
                                     : AL_TRUE);
}

std::string api::error_string(ALCenum err)
{
    switch(err)
    {
    case ALC_INVALID_CONTEXT:
        return "invalid context";
    case ALC_INVALID_DEVICE:
        return "invalid device";
    case ALC_INVALID_ENUM:
        return "invalid enum";
    case ALC_INVALID_VALUE:
        return "invalid value";
    case ALC_OUT_OF_MEMORY:
        return "out of memory";
    default:
        break;
    }
    return {};
}

std::optional<std::string> api::load(DeviceHandle&& device)
{
    const auto has_extension = [this](const char* name) -> bool {
        return alcIsExtensionPresent(m_device, name);
    };
    const auto get_proc = [this]<typename T>(const char* name, T& proc) {
        auto proc_ptr = alcGetProcAddress(m_device, name);
        proc          = reinterpret_cast<T>(proc_ptr);
    };

    const bool loopback_supported = has_extension("ALC_SOFT_loopback");
    const bool loopback_requested = device.dummy.has_value();

    if(!loopback_supported && loopback_requested)
        return "loopback was requested, but not supported by platform";

    auto name = device.name.value_or("");
    if(loopback_supported && loopback_requested)
    {
        using namespace platform::url::constructors;
        using semantic::RSCA;

        auto rendered_fd = platform::file::open_file(
            "rendered_audio"_tmp,
            RSCA::Truncate | RSCA::NewFile | RSCA::Append | RSCA::WriteOnly);
        if(rendered_fd.has_error())
            return "failed to open dummy output file";
        auto const& fmt = device.dummy->fmt;
        m_loopback      = loopback_data_t{
                 .fmt              = fmt,
                 .last_render_time = compo::clock::now(),
                 .rendered         = std::move(rendered_fd.value()),
                 .speed            = device.dummy->speed,
                 .sample_size      = fmt.format == Format::f32 ? 4u
                                     : fmt.bits == 8           ? 1u
                                     : fmt.bits == 16          ? 2u
                                                               : 4u,
        };
        get_proc("alcLoopbackOpenDeviceSOFT", loopbackOpenDeviceSOFT);
        get_proc("alcIsRenderFormatSupportedSOFT", isRenderFormatSupportedSOFT);
        get_proc("alcRenderSamplesSOFT", renderSamplesSOFT);
        m_device = alcLoopbackOpenDeviceSOFT(nullptr);
    } else
        m_device = alcOpenDevice(name != "" ? name.c_str() : nullptr);

    if(!m_device)
        return fmt::format("failed to open device: \"{}\"", name);

    std::vector<ALCint> attrs{};

    while(has_extension("ALC_SOFT_HRTF") && device.enable_hrtf)
    {
        get_proc("alcGetStringiSOFT", getStringiSOFT);

        ALCint num_hrtfs{};
        alcGetIntegerv(m_device, ALC_NUM_HRTF_SPECIFIERS_SOFT, 1, &num_hrtfs);

        if(num_hrtfs < 1)
        {
            cDebug("HRTF supported requested, but no HRTFs available");
            break;
        }

        cDebug("OpenAL HRTFs detected:");
        for(auto i : stl_types::range(num_hrtfs))
            cDebug(
                " - {}",
                alcGetStringiSOFT(m_device, ALC_HRTF_SPECIFIER_SOFT, i));

        attrs.push_back(ALC_HRTF_SOFT);
        attrs.push_back(ALC_TRUE);
        break;
    }
    if(!has_extension("ALC_SOFT_HRTF") && device.enable_hrtf)
        cDebug("OpenAL HRTF requested, but not available");
    if(has_extension("ALC_SOFT_pause_device"))
    {
        get_proc("alcDevicePauseSOFT", devicePauseSOFT);
        get_proc("alcDeviceResumeSOFT", deviceResumeSOFT);
    }
    if(m_loopback.has_value())
    {
        auto info = *device.dummy;
        auto channelFormat =
            info.fmt.channels == 2 ? ALC_STEREO_SOFT : ALC_MONO_SOFT;
        auto dataType = info.fmt.format == Format::f32 ? ALC_FLOAT_SOFT
                        : info.fmt.bits == 8           ? ALC_BYTE_SOFT
                        : info.fmt.bits == 16          ? ALC_SHORT_SOFT
                                                       : ALC_INT_SOFT;
        if(!alcIsRenderFormatSupportedSOFT(
               m_device, info.fmt.frequency, channelFormat, dataType))
            return fmt::format(
                "unsupported render format: freq={}, channels={}, type={}",
                info.fmt.frequency,
                info.fmt.channels,
                magic_enum::enum_name(info.fmt.format));

        using format_t = Format::format_t;
        attrs.push_back(ALC_FORMAT_CHANNELS_SOFT);
        attrs.push_back(channelFormat);
        attrs.push_back(ALC_FORMAT_TYPE_SOFT);
        attrs.push_back(dataType);
        attrs.push_back(ALC_FREQUENCY);
        attrs.push_back(info.fmt.frequency);
    }

    attrs.push_back(0);

    m_context = alcCreateContext(m_device, attrs.data());

    if(!m_context)
        return fmt::format("failed to create context: {}", current_error());

    if(!alcMakeContextCurrent(m_context))
        return fmt::format(
            "failed to make context current: {}", current_error());

    m_formats.float32    = alIsExtensionPresent("AL_EXT_float32");
    m_formats.ima4_adpcm = alIsExtensionPresent("AL_EXT_IMA4");
    m_formats.ms_adpcm   = alIsExtensionPresent("AL_SOFT_MSADPCM");

    m_features.soft.block_alignment =
        alIsExtensionPresent("AL_SOFT_block_alignment");
    m_features.soft.spatialize =
        alIsExtensionPresent("AL_SOFT_source_spatialize");

    return std::nullopt;
}

std::string api::current_error()
{
    return error_string(alcGetError(m_device));
}

std::string api::device()
{
    auto name     = alcGetString(m_device, ALC_DEVICE_SPECIFIER);
    auto ext_name = alcGetString(m_device, ALC_ALL_DEVICES_SPECIFIER);
    if(name)
        return fmt::format("{} ({})", name, ext_name ? ext_name : "no name");
    else if(ext_name)
        return ext_name;
    else
        return "OpenAL device";
}

void api::resume_playback()
{
    if(deviceResumeSOFT)
        alcDeviceResumeSOFT(m_device);
}

ALenum enum_to_al(source_property prop)
{
    switch(prop)
    {
    case semantic::concepts::sound::source_property::gain:
        return AL_GAIN;
    case semantic::concepts::sound::source_property::min_gain:
        return AL_MIN_GAIN;
    case semantic::concepts::sound::source_property::max_gain:
        return AL_MAX_GAIN;
    case semantic::concepts::sound::source_property::pitch:
        return AL_PITCH;
    case semantic::concepts::sound::source_property::max_distance:
        return AL_MAX_DISTANCE;
    case semantic::concepts::sound::source_property::looping:
        return AL_LOOPING;
    case semantic::concepts::sound::source_property::relative:
        return AL_SOURCE_RELATIVE;
    case semantic::concepts::sound::source_property::position:
        return AL_POSITION;
    case semantic::concepts::sound::source_property::velocity:
        return AL_VELOCITY;
    case semantic::concepts::sound::source_property::direction:
        return AL_DIRECTION;
    case semantic::concepts::sound::source_property::orientation:
        return AL_ORIENTATION;
    case semantic::concepts::sound::source_property::spatialized:
        return AL_SOURCE_SPATIALIZE_SOFT;
    case semantic::concepts::sound::source_property::rolloff_factor:
        return AL_ROLLOFF_FACTOR;
    case semantic::concepts::sound::source_property::reference_distance:
        return AL_REFERENCE_DISTANCE;
    case semantic::concepts::sound::source_property::inner_cone_angle:
        return AL_CONE_INNER_ANGLE;
    case semantic::concepts::sound::source_property::outer_cone_angle:
        return AL_CONE_OUTER_ANGLE;
    case semantic::concepts::sound::source_property::outer_cone_gain:
        return AL_CONE_OUTER_GAIN;
    default:
        return AL_NONE;
    }
}

std::optional<std::string> system::load(
    compo::EntityContainer& e, DeviceHandle&& device)
{
    auto const& dummyPlug =
        e.service<comp_app::AppLoader>()->config<comp_app::dummy_plug::Config>();

    if(dummyPlug.enabled)
    {
        device.dummy = DummyInfo{
            .fmt = Format{
                .frequency = dummyPlug.audio_config.frequency,
                .channels  = dummyPlug.audio_config.channels,
                .bits      = dummyPlug.audio_config.bits,
                .format    = dummyPlug.audio_config.format,
            },
            .speed = 1.f,
        };
    }

    return api::load(std::move(device));
}

void system::start_frame(compo::ContainerProxy& p, const compo::time_point& t)
{
    if(m_loopback.has_value())
    {
        using namespace std::chrono_literals;
        auto&             loopback = *m_loopback;
        std::vector<char> rendered;

        auto time_delta = t - loopback.last_render_time;
        u32  num_millis =
            (time_delta > 0ms)
                 ? std::chrono::duration_cast<std::chrono::milliseconds>(
                      time_delta)
                      .count()
                 : 0u;
        num_millis *= loopback.speed;
        rendered.resize(
            (loopback.sample_size * loopback.fmt.channels *
             loopback.fmt.frequency * num_millis) /
            1000);
        const u32 num_samples =
            rendered.size() / (loopback.sample_size * loopback.fmt.channels);
        alcRenderSamplesSOFT(m_device, rendered.data(), num_samples);
        auto err = platform::file::write(
            loopback.rendered,
            gsl::span<const char>(rendered.data(), rendered.size()));
        if(err.has_value())
            cDebug(
                "Error writing loopback audio: {}",
                platform::file::posix::error_message(err.value()));
        loopback.last_render_time = t;
    }
    if(auto err = current_error(); err != std::string())
    {
        cWarning("Audio system error: {}", err);
    }
}

void system::collect_info(comp_app::interfaces::AppInfo& appInfo)
{
#if defined(COFFEE_EMSCRIPTEN)
    appInfo.add("al:api", "Emscripten");
#else
    appInfo.add("al:api", "openal-soft");
#endif
    appInfo.add("al:device", device());
    std::string all_extensions;
    if(auto extensions = alcGetString(m_device, ALC_EXTENSIONS))
        all_extensions.append(extensions);
    if(auto extensions = alGetString(AL_EXTENSIONS))
    {
        if(!all_extensions.empty())
            all_extensions.push_back(' ');
        all_extensions.append(extensions);
    }
    appInfo.add("al:extensions", all_extensions);
    if(auto vendor = alGetString(AL_VENDOR))
        appInfo.add("al:vendor", vendor);
    if(auto renderer = alGetString(AL_RENDERER))
        appInfo.add("al:renderer", renderer);
    ALCint major, minor;
    alcGetIntegerv(m_device, ALC_MAJOR_VERSION, 1, &major);
    alcGetIntegerv(m_device, ALC_MINOR_VERSION, 1, &minor);
    appInfo.add("al:version", fmt::format("{}.{}", major, minor));

    if constexpr(compile_info::platform::is_emscripten)
    {
        cDebug(
            "OpenAL info dump:\nDevice: {}\nExtensions: {} {}",
            device(),
            alcGetString(m_device, ALC_EXTENSIONS),
            alGetString(AL_EXTENSIONS));
    }
}

} // namespace oaf
