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
        glm::mat<3, 2, f32> prop;

        // Row 2 is the view-space +Z row; the camera looks down -Z, so
        // negate it to get the actual look/"at" direction (see the matching
        // extraction + comment in standard_input_handlers.h's tick()).
        Vecf3 forward = -Vecf3{rotation[0][2], rotation[1][2], rotation[2][2]};
        Vecf3 right   = Vecf3{rotation[0][0], rotation[1][0], rotation[2][0]};
        prop[0]       = forward;
        prop[1]       = glm::cross(right, forward);
        alListenerfv(enum_to_al(Prop), &prop[0][0]);
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
