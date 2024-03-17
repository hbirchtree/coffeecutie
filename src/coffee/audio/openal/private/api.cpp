#include <oaf/api.h>

#include <coffee/core/debug/formatting.h>
#include <fmt/format.h>

namespace oaf {

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
    m_device = alcOpenDevice(name != "" ? name.c_str() : nullptr);

    if(!m_device)
        return fmt::format("failed to open device: \"{}\"", name);

    m_context = alcCreateContext(m_device, nullptr);

    if(!m_context)
        return current_error();

    if(!alcMakeContextCurrent(m_context))
        return current_error();

    return std::nullopt;
}

std::string api::current_error()
{
    return error_string(alcGetError(m_device));
}

std::string api::device()
{
    auto name = alcGetString(m_device, ALC_DEVICE_SPECIFIER);
    if(name)
        return name;
    else
        return "OpenAL device";
}

std::vector<std::string> api::extensions()
{
    // auto exts = alcGetString(m_device, ALC_EXTENSIONS);
    return {};
}

void api::collect_info(comp_app::interfaces::AppInfo& appInfo)
{
#if defined(COFFEE_EMSCRIPTEN)
    appInfo.add("al:api", "Emscripten");
#else
    appInfo.add("al:api", "openal-soft");
#endif
    appInfo.add("al:device", device());
    if(auto extensions = alcGetString(m_device, ALC_EXTENSIONS))
        appInfo.add("al:extensions", extensions);
    ALCint major, minor;
    alcGetIntegerv(m_device, ALC_MAJOR_VERSION, 1, &major);
    alcGetIntegerv(m_device, ALC_MINOR_VERSION, 1, &minor);
    appInfo.add("al:version", fmt::format("{}.{}", major, minor));
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
        break;
    }
}

void system::start_frame(compo::ContainerProxy &, const compo::time_point &)
{
    if(auto err = current_error(); err != std::string())
    {
        cWarning("Audio system error: {}", err);
    }
}

} // namespace oaf
