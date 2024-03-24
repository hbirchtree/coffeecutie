#include <oaf/api.h>

#if __has_include(<AL/alext.h>)
#include <AL/alext.h>
#endif

#include <coffee/core/debug/formatting.h>
#include <fmt/format.h>

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
    auto name = device.name.value_or("");
    m_device  = alcOpenDevice(name != "" ? name.c_str() : nullptr);

    if(!m_device)
        return fmt::format("failed to open device: \"{}\"", name);

    const auto has_extension = [this](const char* name) -> bool {
        return alcIsExtensionPresent(m_device, name);
    };
    const auto get_proc = [this]<typename T>(const char* name, T& proc) {
        auto proc_ptr = alcGetProcAddress(m_device, name);
        proc          = reinterpret_cast<T>(proc_ptr);
    };

    std::vector<ALCuint> attrs{};

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

    attrs.push_back(0);

    m_context = alcCreateContext(m_device, nullptr);

    if(!m_context)
        return current_error();

    if(!alcMakeContextCurrent(m_context))
        return current_error();

    m_formats.float32    = alcIsExtensionPresent(m_device, "AL_EXT_float32");
    m_formats.ima4_adpcm = alcIsExtensionPresent(m_device, "AL_EXT_IMA4");
    m_formats.ms_adpcm   = alcIsExtensionPresent(m_device, "AL_SOFT_MSADPCM");

    m_features.soft.block_alignment =
        alcIsExtensionPresent(m_device, "AL_SOFT_block_alignment");
    m_features.soft.spatialize =
        alcIsExtensionPresent(m_device, "AL_SOFT_source_spatialize");

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

void api::collect_info(comp_app::interfaces::AppInfo& appInfo)
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
            "OpenAL info dump:\nDevice: {}\nExtensions: {}",
            device(),
            alcGetString(m_device, ALC_EXTENSIONS));
    }
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
    case semantic::concepts::sound::source_property::rolloff_factor:
    case semantic::concepts::sound::source_property::reference_distance:
    case semantic::concepts::sound::source_property::inner_cone_angle:
    case semantic::concepts::sound::source_property::outer_cone_angle:
    case semantic::concepts::sound::source_property::outer_cone_gain:
        return AL_NONE;
    }
}

void system::start_frame(compo::ContainerProxy& p, const compo::time_point&)
{
    if(m_piggyback_input_event && !m_input_listener_registered)
    {
        using Coffee::Input::CIEvent;
        using Coffee::Input::CIMouseButtonEvent;
        p.underlying()
            .service<comp_app::BasicEventBus<CIEvent>>()
            ->addEventFunction<CIMouseButtonEvent>(
                1000, [this](CIEvent&, CIMouseButtonEvent* ev) {
                    if(!m_piggyback_input_event)
                        return;
                    cDebug("Resuming OpenAL device on input event");
                    resume_playback();
                    m_piggyback_input_event = false;
                });
        m_input_listener_registered = true;
    }

    if(auto err = current_error(); err != std::string())
    {
        cWarning("Audio system error: {}", err);
    }
}

} // namespace oaf
