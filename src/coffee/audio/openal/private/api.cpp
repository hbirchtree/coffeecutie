#include <oaf/api.h>

#include <oaf/api_system.h>

#if __has_include(<AL/alext.h>)
#include <AL/alext.h>
#endif
#if __has_include(<AL/efx.h>)
#include <AL/efx.h>
#define OAF_HAS_EFX 1
#endif

#include <coffee/comp_app/subsystems.h>
#include <coffee/core/debug/formatting.h>
#include <fmt/format.h>
#include <algorithm>
#include <peripherals/stl/magic_enum.hpp>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/string/hex.h>

#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
#include <coffee/comp_app/dummy_plug.h>
#endif

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

// void alcDevicePauseSOFT(ALCdevice* device)
// {
//     devicePauseSOFT(device);
// }

void alcDeviceResumeSOFT(ALCdevice* device)
{
    deviceResumeSOFT(device);
}

#if defined(OAF_HAS_EFX)
struct efx_procs_t
{
    LPALGENFILTERS                 gen_filters{};
    LPALDELETEFILTERS              delete_filters{};
    LPALFILTERI                    filteri{};
    LPALFILTERF                    filterf{};
    LPALGENEFFECTS                 gen_effects{};
    LPALDELETEEFFECTS              delete_effects{};
    LPALEFFECTI                    effecti{};
    LPALEFFECTF                    effectf{};
    LPALGENAUXILIARYEFFECTSLOTS    gen_slots{};
    LPALDELETEAUXILIARYEFFECTSLOTS delete_slots{};
    LPALAUXILIARYEFFECTSLOTI       sloti{};
    LPALAUXILIARYEFFECTSLOTF       slotf{};

    bool complete() const
    {
        return gen_filters && delete_filters && filteri && filterf &&
               gen_effects && delete_effects && effecti && effectf &&
               gen_slots && delete_slots && sloti && slotf;
    }
} efx;
#endif

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

void detail::filter_dealloc(ALuint filter)
{
#if defined(OAF_HAS_EFX)
    efx.delete_filters(1, &filter);
#endif
}

void detail::effect_dealloc(ALuint effect)
{
#if defined(OAF_HAS_EFX)
    efx.delete_effects(1, &effect);
#endif
}

void detail::effect_slot_dealloc(ALuint slot)
{
#if defined(OAF_HAS_EFX)
    efx.delete_slots(1, &slot);
#endif
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

void source_t::set_direct_filter(filter_t const* filter)
{
#if defined(OAF_HAS_EFX)
    if(!m_features.efx)
        return;
    alSourcei(
        m_handle,
        AL_DIRECT_FILTER,
        filter ? static_cast<ALint>(filter->m_handle.hnd) : AL_FILTER_NULL);
    detail::check_error("alSourcei(AL_DIRECT_FILTER)");
#else
    (void)filter;
#endif
}

void source_t::set_send(u32 send, effect_slot_t const* slot)
{
#if defined(OAF_HAS_EFX)
    if(!m_features.efx || send >= m_features.efx_sends)
        return;
    alSource3i(
        m_handle,
        AL_AUXILIARY_SEND_FILTER,
        slot ? static_cast<ALint>(slot->m_handle.hnd) : AL_EFFECTSLOT_NULL,
        static_cast<ALint>(send),
        AL_FILTER_NULL);
    detail::check_error("alSource3i(AL_AUXILIARY_SEND_FILTER)");
#else
    (void)send;
    (void)slot;
#endif
}

filter_t::filter_t()
{
#if defined(OAF_HAS_EFX)
    if(!efx.gen_filters)
        return;
    efx.gen_filters(1, &m_handle.hnd);
    efx.filteri(m_handle, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    detail::check_error("alGenFilters");
#endif
}

void filter_t::set_lowpass(f32 gain, f32 gain_hf)
{
#if defined(OAF_HAS_EFX)
    if(!m_handle)
        return;
    efx.filterf(m_handle, AL_LOWPASS_GAIN, std::clamp(gain, 0.f, 1.f));
    efx.filterf(m_handle, AL_LOWPASS_GAINHF, std::clamp(gain_hf, 0.f, 1.f));
    detail::check_error("alFilterf");
#else
    (void)gain;
    (void)gain_hf;
#endif
}

effect_slot_t::effect_slot_t()
{
#if defined(OAF_HAS_EFX)
    if(!efx.gen_effects)
        return;
    efx.gen_effects(1, &m_effect.hnd);
    /* EAX reverb has the HF reference, plain reverb is the fallback */
    alGetError();
    efx.effecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
    m_eax = alGetError() == AL_NO_ERROR;
    if(!m_eax)
        efx.effecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
    efx.gen_slots(1, &m_handle.hnd);
    efx.sloti(m_handle, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(m_effect.hnd));
    detail::check_error("alGenAuxiliaryEffectSlots");
#endif
}

void effect_slot_t::set_reverb(reverb_t const& r)
{
#if defined(OAF_HAS_EFX)
    if(!m_handle)
        return;
    /* Out-of-range values are rejected outright, keeping the old value */
    auto set = [this](ALenum eax, ALenum std_, f32 v, f32 lo, f32 hi) {
        efx.effectf(m_effect, m_eax ? eax : std_, std::clamp(v, lo, hi));
    };
    set(AL_EAXREVERB_DENSITY, AL_REVERB_DENSITY, r.density, 0.f, 1.f);
    set(AL_EAXREVERB_DIFFUSION, AL_REVERB_DIFFUSION, r.diffusion, 0.f, 1.f);
    set(AL_EAXREVERB_GAIN, AL_REVERB_GAIN, r.gain, 0.f, 1.f);
    set(AL_EAXREVERB_GAINHF, AL_REVERB_GAINHF, r.gain_hf, 0.f, 1.f);
    set(AL_EAXREVERB_DECAY_TIME,
        AL_REVERB_DECAY_TIME,
        r.decay_time,
        0.1f,
        20.f);
    set(AL_EAXREVERB_DECAY_HFRATIO,
        AL_REVERB_DECAY_HFRATIO,
        r.decay_hf_ratio,
        0.1f,
        2.f);
    set(AL_EAXREVERB_REFLECTIONS_GAIN,
        AL_REVERB_REFLECTIONS_GAIN,
        r.reflections_gain,
        0.f,
        3.16f);
    set(AL_EAXREVERB_REFLECTIONS_DELAY,
        AL_REVERB_REFLECTIONS_DELAY,
        r.reflections_delay,
        0.f,
        0.3f);
    set(AL_EAXREVERB_LATE_REVERB_GAIN,
        AL_REVERB_LATE_REVERB_GAIN,
        r.late_reverb_gain,
        0.f,
        10.f);
    set(AL_EAXREVERB_LATE_REVERB_DELAY,
        AL_REVERB_LATE_REVERB_DELAY,
        r.late_reverb_delay,
        0.f,
        0.1f);
    set(AL_EAXREVERB_ROOM_ROLLOFF_FACTOR,
        AL_REVERB_ROOM_ROLLOFF_FACTOR,
        r.room_rolloff,
        0.f,
        10.f);
    if(m_eax)
        efx.effectf(
            m_effect,
            AL_EAXREVERB_HFREFERENCE,
            std::clamp(r.hf_reference, 1000.f, 20000.f));
    /* A slot keeps its own copy, so the effect has to be reattached */
    efx.sloti(m_handle, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(m_effect.hnd));
    detail::check_error("alEffectf(reverb)");
#else
    (void)r;
#endif
}

void effect_slot_t::set_gain(f32 gain)
{
#if defined(OAF_HAS_EFX)
    if(!m_handle)
        return;
    efx.slotf(m_handle, AL_EFFECTSLOT_GAIN, std::clamp(gain, 0.f, 1.f));
    detail::check_error("alAuxiliaryEffectSlotf");
#else
    (void)gain;
#endif
}

std::shared_ptr<filter_t> api::alloc_filter()
{
    if(!m_features.efx)
        return nullptr;
    return std::make_shared<filter_t>();
}

std::shared_ptr<effect_slot_t> api::alloc_effect_slot()
{
    if(!m_features.efx)
        return nullptr;
    return std::make_shared<effect_slot_t>();
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
#if defined(ALC_SOFT_loopback)
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
#endif
        m_device = alcOpenDevice(name != "" ? name.c_str() : nullptr);

    if(!m_device)
        return fmt::format("failed to open device: \"{}\"", name);

    std::vector<ALCint> attrs{};

    while(has_extension("ALC_SOFT_HRTF") && device.enable_hrtf)
    {
        ALCint num_hrtfs{};
        alcGetIntegerv(m_device, ALC_NUM_HRTF_SPECIFIERS_SOFT, 1, &num_hrtfs);

        if(num_hrtfs < 1)
        {
            cDebug("HRTF supported requested, but no HRTFs available");
            break;
        }

#if defined(COFFEE_WASM64)
        /* TODO: Bug on Wasm64 gives mismatched function signature
         * Maybe fixed in a later Emscripten version? */
#else
        get_proc("alcGetStringiSOFT", getStringiSOFT);
        if(getStringiSOFT)
        {
            cDebug("OpenAL HRTFs detected:");
            for(auto i : stl_types::range(num_hrtfs))
                cDebug(
                    " - {}",
                    alcGetStringiSOFT(m_device, ALC_HRTF_SPECIFIER_SOFT, i));
        }
#endif

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
#if defined(ALC_SOFT_loopback)
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

        attrs.push_back(ALC_FORMAT_CHANNELS_SOFT);
        attrs.push_back(channelFormat);
        attrs.push_back(ALC_FORMAT_TYPE_SOFT);
        attrs.push_back(dataType);
        attrs.push_back(ALC_FREQUENCY);
        attrs.push_back(info.fmt.frequency);
    }
#endif

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

#if defined(OAF_HAS_EFX)
    if(has_extension("ALC_EXT_EFX"))
    {
        const auto get_al_proc = []<typename T>(const char* name, T& proc) {
            proc = reinterpret_cast<T>(alGetProcAddress(name));
        };
        get_al_proc("alGenFilters", efx.gen_filters);
        get_al_proc("alDeleteFilters", efx.delete_filters);
        get_al_proc("alFilteri", efx.filteri);
        get_al_proc("alFilterf", efx.filterf);
        get_al_proc("alGenEffects", efx.gen_effects);
        get_al_proc("alDeleteEffects", efx.delete_effects);
        get_al_proc("alEffecti", efx.effecti);
        get_al_proc("alEffectf", efx.effectf);
        get_al_proc("alGenAuxiliaryEffectSlots", efx.gen_slots);
        get_al_proc("alDeleteAuxiliaryEffectSlots", efx.delete_slots);
        get_al_proc("alAuxiliaryEffectSloti", efx.sloti);
        get_al_proc("alAuxiliaryEffectSlotf", efx.slotf);

        ALCint sends{0};
        alcGetIntegerv(m_device, ALC_MAX_AUXILIARY_SENDS, 1, &sends);
        m_features.efx       = efx.complete() && sends > 0;
        m_features.efx_sends = static_cast<u32>(std::max(sends, 0));
    }
#endif
    cDebug(
        "OpenAL EFX: {} ({} sends)",
        m_features.efx ? "enabled" : "unavailable",
        m_features.efx_sends);

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
#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
    auto const& dummyPlug = e.service<comp_app::AppLoader>()
                                ->config<comp_app::dummy_plug::Config>();

    if(dummyPlug.enabled)
    {
        device.dummy = DummyInfo{
            .fmt =
                Format{
                    .frequency = dummyPlug.audio_config.frequency,
                    .channels  = dummyPlug.audio_config.channels,
                    .bits      = dummyPlug.audio_config.bits,
                    .format    = dummyPlug.audio_config.format,
                },
            .speed = 1.f,
        };
    }
#else
    (void)e;
#endif

    return api::load(std::move(device));
}

void system::start_frame(compo::ContainerProxy&, const compo::time_point& t)
{
#if defined(ALC_SOFT_loopback)
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
#endif
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
    appInfo.add("al:efx", m_features.efx ? "yes" : "no");

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
