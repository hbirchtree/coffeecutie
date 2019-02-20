#include <coffee/core/CArgParser>
#include <coffee/core/coffee.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/chunk.h>
#include <coffee/core/types/debug/component.h>
#include <coffee/core/types/debug/severity.h>
#include <coffee/core/types/debug/type.h>
#include <coffee/core/types/display/properties.h>
#include <coffee/core/types/hardware_info.h>
#include <coffee/core/types/software_info.h>
#include <coffee/core/types/vector_types.h>
#include <coffee/core/types/version.h>
#include <peripherals/stl/string_ops.h>
#include <peripherals/typing/geometry/bounding_box.h>

#include <coffee/strings/geometry_types.h>
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/vector_types.h>

#include <coffee/core/CStringFormat>
#include <coffee/core/CUnitTesting>

using namespace Coffee;
using namespace Coffee::debug;

bool basic_format()
{
    CString real = "HELLO World! ÆØÅ";
    CString test = cStringFormat("{0}", real);
    return real == test;
}

bool string_format()
{
    CString real = "HELLO World! ÆØÅ";

    CString str1 = "HELLO";
    CString str2 = "ÆØÅ";
    CString test = cStringFormat("{0} World! {1}", str1, str2);

    if(real != test)
    {
        cDebug("Formatting error: {0} != {1}", real, test);
        return false;
    } else
        return true;
}

bool cstring_format()
{
    CString real = "HELLO World! ÆØÅ";

    cstring str1 = "HELLO";
    cstring str2 = "ÆØÅ";
    CString test = cStringFormat("{0} World! {1}", str1, str2);

    if(real != test)
    {
        cDebug("Formatting error: {0} != {1}", real, test);
        return false;
    } else
        return true;
}

bool integer_decimal_format()
{
    CString real = "Number: 1024";
    CString test = cStringFormat("Number: {0}", 1024);
    if(real != test)
    {
        cDebug("Formatting error: {0} != {1}", real, test);
        return false;
    } else
        return true;
}

bool integer_hex_format()
{
    CString real = "Number: 400";
    CString test = cStringFormat("Number: {0}", str::convert::hexify(1024));
    if(real != test)
    {
        cDebug("Formatting error: {0} != {1}", real, test);
        return false;
    } else
        return true;
}

bool pointer_format()
{
    c_cptr ptr = (c_cptr)0x4004;

    CString real = "Pointer: 0x4004";
    CString test = cStringFormat("Pointer: {0}", ptr);
    if(real != test)
    {
        cWarning("Formatting error: {0} != {1}", real, test);
        return false;
    } else
        return true;
}

bool pointer_allocated_format()
{
    Bytes ptr_f = Bytes::Alloc(100);

    c_cptr ptr = ptr_f.data;

    CString real;
    real.resize(100);

    snprintf(&real[0], real.size(), "Pointer: %p", ptr);

    real.resize(libc::str::find(real.c_str(), '\0') - real.c_str());

    CString test = cStringFormat("Pointer: {0}", ptr);
    if(real != test)
    {
        cWarning("Formatting error: {0} != {1}", real, test);
        return false;
    } else
        return true;
}

bool floating_point_format()
{
    CString real = "Number: 1000.1";
    CString test = cStringFormat("Number: {0:1}", 1000.1);
    if(real != test)
    {
        cWarning("Formatting error: {0} != {1}", real, test);
        return false;
    } else
        return true;
}

bool vector_format()
{
    Vecf3 vec(1);

    CString real = "v(1.,1.,1.)";
    CString test = cStringFormat("{0}", vec);
    if(real != test)
    {
        cWarning("Formatting error: {0} != {1}", real, test);
        return false;
    } else
        return true;
}

template<typename EnumType, EnumType EVal>
bool enum_fmt()
{
    u32 enum_i   = 0;
    u32 enum_max = C_CAST<u32>(EVal);

    while(enum_i <= enum_max)
    {
        cStringFormat("{0}", C_CAST<EnumType>(enum_i));
        enum_i++;
    }

    return true;
}

bool hwdevice_fmt()
{
    HWDeviceInfo test("A", "B", "C", "D");

    cStringFormat("{0}", test);

    return true;
}

bool swver_fmt()
{
    SWVersionInfo test("A", 1, 0, 0, 0, "B");

    cStringFormat("{0}", test);

    return true;
}

bool bbox_fmt()
{
    cStringFormat("{0}", typing::geometry::boundingbox<i32>(1, 1, 1, false));
    return true;
}

bool apparg_fmt()
{
    cStringFormat("{0}", GetInitArgs());
    return true;
}

template<typename VectorT>
bool generic_fmt()
{
    cStringFormat("{0}", VectorT());
    return true;
}

#if defined(COFFEE_WINDOWS)
static constexpr bool has_broken_ptr_fmt = true;
#else
static constexpr bool has_broken_ptr_fmt = false;
#endif

COFFEE_TESTS_BEGIN(27)

    {basic_format, "Basic text formatting"},
    {string_format, "Basic text insertion, part 1", "Has special characters"},
    {cstring_format, "Basic text insertion, part 2", "Has special characters"},
    {integer_decimal_format, "Integer decimals"},
    {integer_hex_format, "Integer hexadecimals"},
    {pointer_format, "Pointer formatting"},
    {pointer_allocated_format,
     "Pointer formatting, random pointer",
     nullptr,
     has_broken_ptr_fmt,
     false},
    {floating_point_format, "Floating-point with precision", "", true, false},
    {vector_format, "Floating-point vector", "", true, false},

    {enum_fmt<Severity, Severity::Fatal>, "Severity formatting"},
    {enum_fmt<debug::Type, debug::Type::Other>, "DebugType formatting"},
    {enum_fmt<debug::Component, debug::Component::Extension>,
     "DbgCmp formatting"},

    {hwdevice_fmt, "HWDeviceInfo format"},
    {swver_fmt, "SWVersionInfo format"},
    {bbox_fmt, "BoundBox format"},

    {generic_fmt<Display::ContextBits>, "ContextBits format"},
    {generic_fmt<Display::GL::Version>, "CGLVersion format"},
    {generic_fmt<Display::Monitor>, "CDMonitor format"},
    {generic_fmt<Version>, "Version format"},

    {generic_fmt<Vecf2>, "Vecf2 format"},
    {generic_fmt<Vecf3>, "Vecf3 format"},
    {generic_fmt<Vecf4>, "Vecf4 format"},
    {generic_fmt<Matf2>, "Matf2 format"},
    {generic_fmt<Matf3>, "Matf3 format"},
    {generic_fmt<Matf4>, "Matf4 format"},
    {generic_fmt<Quatf>, "Quatf format"},
    {apparg_fmt, "AppArg format"}

COFFEE_TESTS_END()
