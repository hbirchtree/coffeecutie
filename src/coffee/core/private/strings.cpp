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

CString extArgReplacePhrase(
    const CString& fmt, const CString& phrase, const CString& replace)
{
    return str::replace::str(fmt, phrase, replace);
}

CString extArgReplace(
    const CString& fmt, const size_t& index, const CString& replace)
{
    // TODO: This needs optimizations
    CString subfmt = "{" + cast_pod(index) + "}";
    return str::replace::str(fmt, subfmt, replace);
}

template<
    typename T,

    typename std::enable_if<std::is_floating_point<T>::value, bool>::type* =
        nullptr

    >
CString cStringReplace(CString const& fmt, size_t const& index, T const& arg)
{
    /* Regexes, man, these fucking regexes */
    regex::Pattern  patt = regex::compile_pattern(".*?(\\{\\d+:(\\d+)\\}).*");
    Vector<CString> match;

    if(!regex::match(patt, fmt, match))
        return extArgReplace(fmt, index, Strings::to_string(arg));

    u32 prec = cast_string<u32>(match[2].c_str());

    CString rep;
#ifdef COFFEE_USE_IOSTREAMS
    std::stringstream ss;
    ss << std::fixed << std::setprecision(prec) << arg;
    ss >> rep;
#else
    rep       = str::convert::to_string(arg);
    auto dot1 = rep.find('.');
    auto dot2 = rep.find(',');
    if(dot1)
        rep.resize(dot1 + 1 + prec, '0');
    else if(dot2)
        rep.resize(dot2 + 1 + prec, '0');
#endif

    return extArgReplacePhrase(fmt, match[1], rep);
}

template<
    typename T,
    size_t num,
    char   v,

    typename is_pod<T>::type* = nullptr

    >
static inline CString vector_to_string(
    typing::vectors::tvector<T, num> const& value)
{
    CString out;

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

template<
    typename T,
    size_t num,
    char   v,

    typename std::is_pod<T>::type* = nullptr

    >
static inline CString matrix_to_string(
    typing::vectors::tmatrix<T, num> const& value)
{
    CString out;

    out += v;
    out += '(';

    for(size_t i = 0; i < num; i++)
    {
        out += vector_to_string<T, num, 'v'>(value[i]);
    }

    out += ')';

    return out;
}

CString to_string(Quatf const& v)
{
    return vector_to_string<scalar, 4, 'q'>(v);
}

CString to_string(Vecf4 const& v)
{
    return vector_to_string<scalar, 4, 'v'>(v);
}

CString to_string(Vecf3 const& v)
{
    return vector_to_string<scalar, 3, 'v'>(v);
}

CString to_string(Vecf2 const& v)
{
    return vector_to_string<scalar, 2, 'v'>(v);
}

CString to_string(Matf4 const& v)
{
    return matrix_to_string<scalar, 4, 'm'>(v);
}

CString to_string(Matf3 const& v)
{
    return matrix_to_string<scalar, 3, 'm'>(v);
}

using namespace ::semantic::debug;

cstring to_string(const Severity& sev)
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

cstring to_string(const Type& type)
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

cstring to_string(const Component& comp)
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

template<typename T>
static CString version_to_string(version<T> const& arg)
{
    return cast_pod(arg.major) + '.' + cast_pod(arg.minor) + '.' +
           cast_pod(arg.revision);
}

CString to_string(const version<u8>& arg)
{
    return version_to_string<u8>(arg);
}

CString to_string(const version<i32>& arg)
{
    return version_to_string<i32>(arg);
}

CString to_string(const version<u32>& arg)
{
    return version_to_string<u32>(arg);
}

void conditional_add(CString& src, bool add_space)
{
    if(add_space)
        src += ' ';
}

CString to_string(const HWDeviceInfo& arg)
{
    CString output;

    if(arg.manufacturer.size())
    {
        conditional_add(output, output.size());
        output += arg.manufacturer;
    }

    if(arg.model.size())
    {
        conditional_add(output, output.size());
        output += arg.model;
    }

    if(arg.firmware.size())
    {
        conditional_add(output, output.size());
        output += "running " + arg.firmware;
    }

    if(arg.serial.size())
    {
        conditional_add(output, output.size());
        output += "identified by " + arg.serial;
    }

    return output;
}

CString to_string(const SWVersionInfo& arg)
{
    return cStringFormat(
        "sw={0}, ver={1}.{2}.{3}.{4}.{5}",
        arg.name,
        arg.product,
        arg.major,
        arg.minor,
        arg.patch,
        arg.build);
}

CString to_string(const Display::GL::Version& arg)
{
    return cStringFormat("{0}.{1} {2}", arg.major, arg.minor, arg.driver);
}

CString to_string(const typing::geometry::boundingbox<libc_types::i32>& box)
{
    return cStringFormat(
        "b({0}->{1},{2}->{3},{4}->{5})",
        box.x1(),
        box.x2(),
        box.y1(),
        box.y2(),
        box.z1(),
        box.z2());
}

CString to_string(const Display::ContextBits& arg)
{
    return cStringFormat(
        "bits(rgba:{0},{1},{2},{3};ds:{4},{5},smp:{6})",
        arg.red,
        arg.green,
        arg.blue,
        arg.alpha,
        arg.depth,
        arg.stencil,
        arg.samples);
}

CString to_string(const Display::Monitor& arg)
{
    return cStringFormat(
        "monitor(rgbx:{0},{1},{2},{3})",
        arg.colorBits.red,
        arg.colorBits.green,
        arg.colorBits.blue,
        arg.colorBits.extra);
}

CString to_string(const platform::args::AppArg& args)
{
    CString out = "AppArg(";
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
