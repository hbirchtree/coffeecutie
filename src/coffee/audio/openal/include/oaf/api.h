#pragma once

#include <coffee/components/types.h>
#include <memory>
#include <peripherals/concepts/sound_api.h>
#include <peripherals/error/result.h>
#include <peripherals/semantic/handle.h>
#include <platforms/file.h>

#include <AL/al.h>
#if __has_include(<AL/alext.h>)
#include <AL/alext.h>
#endif
#include <AL/alc.h>

namespace oaf {

using namespace semantic::concepts::sound;

static_assert(std::is_same_v<ALuint, u32>);
static_assert(std::is_same_v<ALfloat, f32>);
static_assert(std::is_same_v<ALint, libc_types::i32>);

namespace detail {

void buffer_dealloc(ALuint buf);
void source_dealloc(ALuint src);
void filter_dealloc(ALuint filter);
void effect_dealloc(ALuint effect);
void effect_slot_dealloc(ALuint slot);
void check_error(std::string_view call);

} // namespace detail

using buffer_handle_t = semantic::generic_handle_t<
    ALuint,
    semantic::handle_modes::auto_close,
    0u,
    detail::buffer_dealloc>;

struct formats_t
{
    bool float32{false};
    bool ima4_adpcm{false};
    bool ms_adpcm{false};
};

struct format_t : Format
{
    ALenum to_al(formats_t const& formats) const;
};

struct features_t
{
    struct
    {
        bool block_alignment{false};
        bool spatialize{false};
        bool loopback{false};
    } soft;

    /* ALC_EXT_EFX, checked at runtime */
    bool efx{false};
    u32  efx_sends{0};
};

using filter_handle_t = semantic::generic_handle_t<
    ALuint,
    semantic::handle_modes::auto_close,
    0u,
    detail::filter_dealloc>;
using effect_handle_t = semantic::generic_handle_t<
    ALuint,
    semantic::handle_modes::auto_close,
    0u,
    detail::effect_dealloc>;
using effect_slot_handle_t = semantic::generic_handle_t<
    ALuint,
    semantic::handle_modes::auto_close,
    0u,
    detail::effect_slot_dealloc>;

/* EAX reverb parameters, linear gains. Defaults are EFX's */
struct reverb_t
{
    f32 density{1.f};
    f32 diffusion{1.f};
    f32 gain{0.32f};
    f32 gain_hf{0.89f};
    f32 decay_time{1.49f};
    f32 decay_hf_ratio{0.83f};
    f32 reflections_gain{0.05f};
    f32 reflections_delay{0.007f};
    f32 late_reverb_gain{1.26f};
    f32 late_reverb_delay{0.011f};
    f32 room_rolloff{0.f};
    f32 hf_reference{5000.f};
};

/* Low-pass, meant for a source's dry path */
struct filter_t
{
    filter_t();

    void set_lowpass(f32 gain, f32 gain_hf);

    filter_handle_t m_handle{};
};

/* Reverb effect loaded into an auxiliary slot */
struct effect_slot_t
{
    effect_slot_t();

    void set_reverb(reverb_t const& reverb);
    void set_gain(f32 gain);

    effect_handle_t      m_effect{};
    effect_slot_handle_t m_handle{};
    bool                 m_eax{false};
};

struct buffer_t
{
    buffer_t(formats_t const& formats)
        : m_formats(formats)
    {
        alGenBuffers(1, &m_handle.hnd);
    }

    template<typename T>
    void upload(gsl::span<T> const& data, format_t const& fmt)
    {
        alBufferData(
            m_handle,
            fmt.to_al(m_formats),
            data.data(),
            data.size_bytes(),
            fmt.frequency);
        detail::check_error("alBufferData");
    }

    buffer_handle_t  m_handle{};
    formats_t const& m_formats;
};

using source_handle_t = semantic::generic_handle_t<
    ALuint,
    semantic::handle_modes::auto_close,
    0u,
    detail::source_dealloc>;

ALenum enum_to_al(source_property prop);

struct source_t
{
    source_t(features_t const& features)
        : m_features(features)
    {
        alGenSources(1, &m_handle.hnd);
        set_property<source_property::rolloff_factor>(1.5f);
    }

    template<source_property Prop>
    requires is_bool_property<Prop>
    void set_property(bool prop)
    {
        alSourcei(m_handle, enum_to_al(Prop), prop ? AL_TRUE : AL_FALSE);
        detail::check_error("alSourcei");
    }

    template<source_property Prop>
    requires is_scalar_property<Prop>
    void set_property(f32 prop)
    {
        alSourcef(m_handle, enum_to_al(Prop), prop);
        detail::check_error("alSourcef");
    }

    template<source_property Prop>
    requires is_vector_property<Prop>
    void set_property(Vecf3 prop)
    {
        alSourcefv(m_handle, enum_to_al(Prop), &prop[0]);
        detail::check_error("alSourcefv");
    }

    void queue(buffer_t const& buf)
    {
        alSourceQueueBuffers(m_handle, 1, &buf.m_handle.hnd);
        detail::check_error("alSourceQueueBuffers");
        alSourcePlay(m_handle);
        detail::check_error("alSourcePlay");
    }

    void unqueue(buffer_t const& buf)
    {
        ALuint hnd = buf.m_handle;
        alSourceUnqueueBuffers(m_handle, 1, &hnd);
        detail::check_error("alSourceUnqueueBuffers");
    }

    std::pair<u32, u32> buffer_queue()
    {
        ALint queued, processed;
        alGetSourcei(m_handle, AL_BUFFERS_QUEUED, &queued);
        detail::check_error("alGetSourcei");
        alGetSourcei(m_handle, AL_BUFFERS_PROCESSED, &processed);
        detail::check_error("alGetSourcei");
        return std::make_pair(
            static_cast<u32>(queued), static_cast<u32>(processed));
    }

    enum spatialize_t
    {
        always,
        mono_only,
        never,
    };

    void spatialize_as(spatialize_t v);

    void set_direct_filter(filter_t const* filter);
    void set_send(
        u32 send, effect_slot_t const* slot, filter_t const* filter = nullptr);

    source_handle_t   m_handle{};
    features_t const& m_features;
};

struct listener_t
{
    template<source_property Prop>
    requires is_scalar_property<Prop> && is_listener_property<Prop>
    void set_property(f32 prop)
    {
        alListenerf(enum_to_al(Prop), prop);
        detail::check_error("alListenerf");
    }

    template<source_property Prop>
    requires is_vector_property<Prop> && is_listener_property<Prop>
    void set_property(Vecf3 prop)
    {
        alListenerfv(enum_to_al(Prop), &prop[0]);
        detail::check_error("alListenerfv");
    }

    template<source_property Prop>
    requires is_mat_property<Prop> && is_listener_property<Prop>
    void set_property(Matf3 const& rotation)
    {
        // Rows of the world->view rotation; the camera looks down -Z
        Vecf3 const up = Vecf3{rotation[0][1], rotation[1][1], rotation[2][1]};
        Vecf3 const forward =
            -Vecf3{rotation[0][2], rotation[1][2], rotation[2][2]};
        f32 const prop[6] = {
            forward.x, forward.y, forward.z, up.x, up.y, up.z};
        alListenerfv(enum_to_al(Prop), prop);
        detail::check_error("alListenerfv");
    }
};

struct api
{
    using buffer_type   = buffer_t;
    using source_type   = source_t;
    using listener_type = listener_t;

    static std::string error_string(ALCenum err);

    std::optional<std::string> load(DeviceHandle&& device = {});

    std::string current_error();

    std::string device();

    auto alloc_buffer()
    {
        return std::make_shared<buffer_t>(m_formats);
    }

    auto alloc_source()
    {
        return std::make_shared<source_t>(m_features);
    }

    std::shared_ptr<filter_t>      alloc_filter();
    std::shared_ptr<effect_slot_t> alloc_effect_slot();

    auto const& features() const
    {
        return m_features;
    }

    auto& listener()
    {
        return m_listener;
    }

    auto const& formats()
    {
        return m_formats;
    }

    enum distance_model_t : ALenum
    {
        linear      = AL_LINEAR_DISTANCE,
        inverse     = AL_INVERSE_DISTANCE,
        exponential = AL_EXPONENT_DISTANCE,
    };

    void set_distance_model(distance_model_t model, bool clamped = false)
    {
        alDistanceModel(model + (clamped ? 1 : 0));
        detail::check_error("alDistanceModel");
    }

  protected:
    void resume_playback();

    ALCdevice*  m_device{nullptr};
    ALCcontext* m_context{nullptr};

    listener_t m_listener;
    formats_t  m_formats{};
    features_t m_features{};

    struct loopback_data_t
    {
        Format                      fmt{};
        compo::time_point           last_render_time{};
        platform::file::file_handle rendered;
        f32                         speed{1.f};
        u32                         sample_size{1};
    };

    std::optional<loopback_data_t> m_loopback{};
};

// static_assert(API<api>);

} // namespace oaf
