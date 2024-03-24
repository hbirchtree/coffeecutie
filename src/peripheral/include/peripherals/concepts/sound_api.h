#pragma once

#include <chrono>
#include <gsl/span>
#include <optional>
#include <peripherals/libc/types.h>
#include <peripherals/stl/any_of.h>
#include <peripherals/typing/vectors/vector_types.h>
#include <string>

namespace semantic::concepts::sound {

using ::libc_types::f32;
using ::libc_types::u16;
using ::libc_types::u32;
using ::typing::vector_types::Vecf3;
using ::typing::vector_types::Matf3;

namespace placeholder {
constexpr u32 num_samples = 0;
}

struct Format
{
    u32 frequency{44100};
    u16 channels{1};
    u16 bits{8};
    enum format_t
    {
        pcm,
        f32,
        ima_adpcm,
        ms_adpcm,
    } format{pcm};
};

template<class T>
concept Buffer = requires(T v) {
    {
        v.upload(gsl::span<char>())
    };
};

template<class T>
concept Timing = requires(T v) {
    {
        v.current_time()
    };
    {
        v.timestamp_at(std::chrono::system_clock::duration())
    };
    {
        v.time_until(std::chrono::system_clock::time_point())
    };
};

enum class source_property
{
    // float/int
    gain = 1,
    min_gain,
    max_gain,
    pitch,
    max_distance,
    rolloff_factor,
    reference_distance,
    inner_cone_angle,
    outer_cone_angle,
    outer_cone_gain,

    // true/false
    looping,
    relative,

    // vec3
    position,
    velocity,
    direction,

    // mat3x2
    orientation,
};

template<source_property Prop>
concept is_vector_property = stl_types::any_of(
    Prop,
    source_property::position,
    source_property::velocity,
    source_property::direction);

template<source_property Prop>
concept is_mat_property = stl_types::any_of(Prop, source_property::orientation);

template<source_property Prop>
concept is_bool_property = stl_types::any_of(
    Prop, source_property::looping, source_property::relative);

template<source_property Prop>
concept is_scalar_property = !is_vector_property<Prop> &&
                             !is_bool_property<Prop> && !is_mat_property<Prop>;

template<class T, class BufferT>
concept Source = Buffer<BufferT> && requires(T v, BufferT buffer) {
    {
        v.template set_property<source_property::gain>(1.f),
        v.template set_property<source_property::looping>(true),
        v.template set_property<source_property::position>(Vecf3{})
    };
    {
        v.queue(buffer)
    };
};

template<source_property Prop>
concept is_listener_property = stl_types::any_of(
    Prop,
    source_property::position,
    source_property::velocity,
    source_property::orientation,
    source_property::gain);

using listener_property = source_property;

template<class T>
concept Listener = requires(T v) {
    {
        v.template set_property<listener_property::gain>(1.f),
        v.template set_property<listener_property::position>(Vecf3{}),
        v.template set_property<listener_property::orientation>(Matf3{})
    };
};

struct DeviceHandle
{
    std::optional<std::string> name;
    bool enable_hrtf{true};
};

template<class T>
concept API = Source<typename T::source_type, typename T::buffer_type> &&
              Listener<typename T::listener_type> && requires(T v) {
                  {
                      T::create(DeviceHandle{.name = "default"})
                  };
                  {
                      v.alloc_buffer(Format())
                  };
                  {
                      v.alloc_source()
                  };
                  {
                      v.listener()
                  };
                  {
                      v.tick()
                  };
              };

} // namespace semantic::concepts::sound
