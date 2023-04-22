#include <coffee/strings/geometry_types.h>
#include <coffee/strings/info.h>
#include <coffee/strings/vector_types.h>

#include <coffee/strings/format.h>

#include <coffee/core/CRegex>
#include <peripherals/libc/string_ops.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/string_ops.h>
#include <platforms/argument_parse.h>

namespace Coffee {
namespace Strings {

template<
    typename T,
    size_t num,
    char   v,

    typename is_pod<T>::type* = nullptr

    >
static inline std::string vector_to_string(
    typing::vectors::tvector<T, num> const& value)
{
    std::string out;

    out += v;
    out += '(';

    for(size_t i = 0; i < num; i++)
    {
        out += cast_pod(value[i]);
        if(i < num - 1)
            out += ',';
    }

    out += ')';

    return out;
}

template<typename T, size_t num, char v>
requires(std::is_standard_layout_v<T>&&
             std::is_trivial_v<T>) static inline std::string
    matrix_to_string(typing::vectors::tmatrix<T, num> const& value)
{
    std::string out;

    out += v;
    out += '(';

    for(size_t i = 0; i < num; i++)
    {
        out += vector_to_string<T, num, 'v'>(value[i]);
    }

    out += ')';

    return out;
}

std::string to_string(typing::vector_types::Quatf const& v)
{
    using namespace std::literals::string_literals;
    return "q(" +
        cast_pod(v.x) + ", " +
        cast_pod(v.y) + ", " +
        cast_pod(v.z) + ", " +
        cast_pod(v.w) + ")";
}

std::string to_string(typing::vector_types::Vecf4 const& v)
{
    return vector_to_string<scalar, 4, 'v'>(v);
}

std::string to_string(typing::vector_types::Vecf3 const& v)
{
    return vector_to_string<scalar, 3, 'v'>(v);
}

std::string to_string(typing::vector_types::Vecf2 const& v)
{
    return vector_to_string<scalar, 2, 'v'>(v);
}

std::string to_string(typing::vector_types::Matf4 const& v)
{
    return matrix_to_string<scalar, 4, 'm'>(v);
}

std::string to_string(typing::vector_types::Matf3 const& v)
{
    return matrix_to_string<scalar, 3, 'm'>(v);
}

using namespace ::semantic::debug;

std::string_view to_string(const Severity& sev)
{
    switch(sev)
    {
    case Severity::Critical:
        return "Critical";
    case Severity::Fatal:
        return "Fatal";
    case Severity::Debug:
        return "Debug";
    case Severity::High:
        return "High";
    case Severity::Medium:
        return "Medium";
    case Severity::Low:
        return "Low";
    case Severity::Information:
        return "Information";
    case Severity::Verbose:
        return "Verbose";
    }
    return "";
}

std::string_view to_string(const Type& type)
{
    switch(type)
    {
    case Type::Compatibility:
        return "Compatibility";
    case Type::Compliance:
        return "Compliance";
    case Type::Deprecated:
        return "Deprecated";
    case Type::Information:
        return "Information";
    case Type::Marker:
        return "Marker";
    case Type::Performance:
        return "Performance";
    case Type::UndefinedBehavior:
        return "Undefined behavior";
    case Type::Other:
        return "Other";
    }
    return "";
}

std::string_view to_string(const Component& comp)
{
    switch(comp)
    {
    case Component::Audio:
        return "Debug::Audio";
    case Component::Core:
        return "Debug::Core";
    case Component::Extension:
        return "Debug::Extension";
    case Component::GraphicsAPI:
        return "Debug::GraphicsAPI";
    case Component::Interface:
        return "Debug::Interface";
    case Component::LibraryLoader:
        return "Debug::LibraryLoader";
    case Component::Media:
        return "Debug::Media";
    case Component::ShaderCompiler:
        return "Debug::ShaderCompiler";
    }
    return "";
}

std::string to_string(const platform::args::AppArg& args)
{
    std::string out = "AppArg(";
#ifndef COFFEE_LOWFAT

    bool touched = false;

    for(cstring_w arg : args.arguments())
    {
        if(touched)
            out += ", ";
        ((out += "'") += (arg) ? arg : "NULL") += "'";
        touched = true;
    }
#endif

    return out + ")";
}

} // namespace Strings
} // namespace Coffee
