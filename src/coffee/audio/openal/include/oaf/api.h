#pragma once

#include <coffee/comp_app/services.h>
#include <memory>
#include <peripherals/concepts/sound_api.h>
#include <peripherals/error/result.h>

#include <AL/al.h>
#include <AL/alc.h>

namespace oaf {

using namespace semantic::concepts::sound;

static_assert(std::is_same_v<ALuint, u32>);
static_assert(std::is_same_v<ALfloat, f32>);
static_assert(std::is_same_v<ALint, libc_types::i32>);

namespace detail {

void buffer_dealloc(ALuint buf);
void source_dealloc(ALuint buf);

} // namespace detail

using buffer_handle_t = semantic::generic_handle_t<
    ALuint,
    semantic::handle_modes::auto_close,
    0u,
    detail::buffer_dealloc>;

struct buffer_t
{
    buffer_t()
    {
        alGenBuffers(1, &m_handle.hnd);
    }

    template<typename T>
    void upload(gsl::span<T> const& data)
    {
    }

    buffer_handle_t m_handle{};
};

using source_handle_t = semantic::generic_handle_t<
    ALuint,
    semantic::handle_modes::auto_close,
    0u,
    detail::source_dealloc>;

ALenum enum_to_al(source_property prop);

struct source_t
{
    source_t()
    {
        alGenSources(1, &m_handle.hnd);
    }

    template<source_property Prop>
    requires is_bool_property<Prop>
    void set_property(bool prop)
    {
        alSourcei(m_handle, enum_to_al(Prop), prop ? AL_TRUE : AL_FALSE);
    }

    template<source_property Prop>
    requires is_scalar_property<Prop>
    void set_property(f32 prop)
    {
        alSourcef(m_handle, enum_to_al(Prop), prop);
    }

    template<source_property Prop>
    requires is_vector_property<Prop>
    void set_property(Vecf3 prop)
    {
        alSourcefv(m_handle, enum_to_al(Prop), &prop[0]);
    }

    source_handle_t m_handle{};
};

struct listener_t
{
    template<source_property Prop>
    requires is_scalar_property<Prop> && is_listener_property<Prop>
    void set_property(f32 prop)
    {
        alListenerf(enum_to_al(Prop), prop);
    }

    template<source_property Prop>
    requires is_vector_property<Prop> && is_listener_property<Prop>
    void set_property(Vecf3 prop)
    {
        alListenerfv(enum_to_al(Prop), &prop[0]);
    }

    template<source_property Prop>
    requires is_mat_property<Prop> && is_listener_property<Prop>
    void set_property(Matf3 const& rotation)
    {
        glm::mat<3, 2, f32> prop;

        Vecf3 forward = Vecf3{rotation[0][2], rotation[1][2], rotation[2][2]};
        prop[0] = forward;
        Vecf3 right = Vecf3{rotation[0][0], rotation[1][0], rotation[2][0]};
        prop[1] = glm::cross(forward, right);
        alListenerfv(enum_to_al(Prop), &prop[0][0]);
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

    std::string              device();
    std::vector<std::string> extensions();

    void collect_info(comp_app::interfaces::AppInfo& appInfo);

    auto alloc_buffer()
    {
        return std::make_shared<buffer_t>();
    }

    auto alloc_source()
    {
        return std::make_shared<source_t>();
    }

    auto& listener()
    {
        return m_listener;
    }

  private:
    ALCdevice*  m_device{nullptr};
    ALCcontext* m_context{nullptr};

    listener_t m_listener;
};

struct system
    : api
    , compo::SubsystemBase
{
    using type = system;

    void start_frame(compo::ContainerProxy &, const compo::time_point &);
};

// static_assert(API<api>);

} // namespace oaf
