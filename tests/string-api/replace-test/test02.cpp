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

    real.resize(Search::ChrFind(real.c_str(),0)-1-real.c_str());

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

const constexpr CoffeeTest::Test _tests[9] = {
    {basic_format,"Basic text formatting"},
    {string_format,"Basic text insertion, part 1","Has special characters"},
    {cstring_format,"Basic text insertion, part 2","Has special characters"},
    {integer_decimal_format,"Integer decimals"},
    {integer_hex_format,"Integer hexadecimals"},
    {pointer_format,"Pointer formatting"},
    {pointer_allocated_format,"Pointer formatting, random pointer"},
    {floating_point_format,"Floating-point with precision"},
    {vector_format,"Floating-point vector"},
};

COFFEE_RUN_TESTS(_tests);
