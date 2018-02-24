#include <coffee/core/coffee.h>
#include <coffee/core/CUnitTesting>
#include <coffee/core/types/vector_types.h>

using namespace Coffee;

bool basic_format()
{
    CString real = "HELLO World! ÆØÅ";
    CString test = cStringFormat("{0}",real);
    return real == test;
}

bool string_format()
{
    CString real = "HELLO World! ÆØÅ";

    CString str1 = "HELLO";
    CString str2 = "ÆØÅ";
    CString test = cStringFormat("{0} World! {1}",str1,str2);

    if(real!=test)
    {
        cDebug("Formatting error: {0} != {1}",real,test);
        return false;
    }else
        return true;
}

bool cstring_format()
{
    CString real = "HELLO World! ÆØÅ";

    cstring str1 = "HELLO";
    cstring str2 = "ÆØÅ";
    CString test = cStringFormat("{0} World! {1}",str1,str2);

    if(real!=test)
    {
        cDebug("Formatting error: {0} != {1}",real,test);
        return false;
    }else
        return true;
}

bool integer_decimal_format()
{
    CString real = "Number: 1024";
    CString test = cStringFormat("Number: {0}",1024);
    if(real!=test)
    {
        cDebug("Formatting error: {0} != {1}",real,test);
        return false;
    }else
        return true;
}

bool integer_hex_format()
{
    CString real = "Number: 400";
    CString test = cStringFormat("Number: {0}",StrUtil::hexify(1024));
    if(real!=test)
    {
        cDebug("Formatting error: {0} != {1}",real,test);
        return false;
    }else
        return true;
}

bool pointer_format()
{
    c_cptr ptr = (c_cptr)0x4004;

    CString real = "Pointer: 0x4004";
    CString test = cStringFormat("Pointer: {0}",ptr);
    if(real!=test)
    {
        cWarning("Formatting error: {0} != {1}",real,test);
        return false;
    }else
        return true;
}

bool pointer_allocated_format()
{
    c_ptr ptr_f = Alloc(100);
    CFree(ptr_f);

    c_cptr ptr = ptr_f;

    CString real;
    real.resize(100);

    snprintf(&real[0],real.size(),"Pointer: %p",ptr);

    real.resize(Search::ChrFind(real.c_str(),0)-real.c_str());

    CString test = cStringFormat("Pointer: {0}",ptr);
    if(real!=test)
    {
        cWarning("Formatting error: {0} != {1}",real,test);
        return false;
    }else
        return true;
}

bool floating_point_format()
{
    CString real = "Number: 1000.1";
    CString test = cStringFormat("Number: {0:1}",1000.1);
    if(real!=test)
    {
        cWarning("Formatting error: {0} != {1}",real,test);
        return false;
    }else
        return true;
}

bool vector_format()
{
    CVec3 vec(1);

    CString real = "v(1.,1.,1.)";
    CString test = cStringFormat("{0}",vec);
    if(real!=test)
    {
        cWarning("Formatting error: {0} != {1}",real,test);
        return false;
    }else
        return true;
}

template<typename EnumType, EnumType EVal>
bool enum_fmt()
{
    u32 enum_i = 0;
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
    cStringFormat("{0}", BoundBox(1, 1, 1, false));
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

const constexpr CoffeeTest::Test _tests[27] = {
    {basic_format,"Basic text formatting"},
    {string_format,"Basic text insertion, part 1","Has special characters"},
    {cstring_format,"Basic text insertion, part 2","Has special characters"},
    {integer_decimal_format,"Integer decimals"},
    {integer_hex_format,"Integer hexadecimals"},
    {pointer_format,"Pointer formatting"},
    {pointer_allocated_format,"Pointer formatting, random pointer", nullptr,
	has_broken_ptr_fmt, false},
    {floating_point_format,"Floating-point with precision", "",
     true, false},
    {vector_format,"Floating-point vector", "", true, false},

    {enum_fmt<Severity, Severity::Fatal>, "Severity formatting"},
    {enum_fmt<DebugType, DebugType::Other>, "DebugType formatting"},
    {enum_fmt<DebugComponent,DebugComponent::Extension>, "DbgCmp formatting"},

    {hwdevice_fmt, "HWDeviceInfo format"},
    {swver_fmt, "SWVersionInfo format"},
    {bbox_fmt, "BoundBox format"},

    {generic_fmt<Display::CDContextBits>, "ContextBits format"},
    {generic_fmt<Display::CGLVersion>, "CGLVersion format"},
    {generic_fmt<Display::CDMonitor>, "CDMonitor format"},
    {generic_fmt<Version>, "Version format"},

    {generic_fmt<Vecf2>, "Vecf2 format"},
    {generic_fmt<Vecf3>, "Vecf3 format"},
    {generic_fmt<Vecf4>, "Vecf4 format"},
    {generic_fmt<Matf2>, "Matf2 format"},
    {generic_fmt<Matf3>, "Matf3 format"},
    {generic_fmt<Matf4>, "Matf4 format"},
    {generic_fmt<Quatf>, "Quatf format"},
    {apparg_fmt, "AppArg format"}
};

COFFEE_RUN_TESTS(_tests);
