#include <coffee/core/CUnitTesting>

#include <coffee/core/CInput>
#include <coffee/core/string_casting.h>

using namespace Coffee;
using namespace CInput;

bool basic_tests()
{
    /* Verify storage sizes */
    if(sizeof(uint8)!=1)
        return false;
    if(sizeof(uint16)!=2)
        return false;
    if(sizeof(uint32)!=4)
        return false;
    if(sizeof(uint64)!=8)
        return false;

    if(sizeof(int8)!=1)
        return false;
    if(sizeof(int16)!=2)
        return false;
    if(sizeof(int32)!=4)
        return false;
    if(sizeof(int64)!=8)
        return false;

    if(sizeof(szptr)!=sizeof(intptr_t))
    {
        cDebug("Inconsistent szptr: {0} != {1}", sizeof(szptr),sizeof(intptr_t));
        return false;
    }

    return true;
}

const constexpr szptr lscalar_expected = 16;

bool floating_storage_tests()
{
    return sizeof(bigscalar)==8 && sizeof(scalar)==4;
}

bool longdoub_test()
{
    if(sizeof(lscalar)!=lscalar_expected)
    {
        cWarning("Inconsistent Lscalar: {0} != {1}",sizeof(lscalar),lscalar_expected);
        return false;
    }
    return true;
}

bool wrapping_tests()
{
    /* Unsigned */
    {
        uint8 v = UInt8_Max;
        v++;
        if(v != 0)
            return false;
    }
    {
        uint16 v = UInt16_Max;
        v++;
        if(v != 0)
            return false;
    }
    {
        uint32 v = UInt32_Max;
        v++;
        if(v != 0)
            return false;
    }
    {
        uint64 v = UInt64_Max;
        v++;
        if(v != 0)
            return false;
    }

    /* Signed, positive */
    {
        int8 v = Int8_Max;
        v++;
        if(v != Int8_Min)
            return false;
    }
    {
        int16 v = Int16_Max;
        v++;
        if(v != Int16_Min)
            return false;
    }
    {
        int32 v = Int32_Max;
        v++;
        if(v != Int32_Min)
            return false;
    }
    {
        int64 v = Int64_Max;
        v++;
        if(v != Int64_Min)
            return false;
    }

    /* Signed, negative */
    {
        int8 v = Int8_Min;
        v--;
        if(v != Int8_Max)
            return false;
    }
    {
        int16 v = Int16_Min;
        v--;
        if(v != Int16_Max)
            return false;
    }
    {
        int32 v = Int32_Min;
        v--;
        if(v != Int32_Max)
            return false;
    }
    {
        int64 v = Int64_Min;
        v--;
        if(v != Int64_Max)
            return false;
    }

    return true;
}

bool uint24_test()
{
    if(sizeof(uint24)==3)
        return true;
    cDebug("Inconsistent uint24: {0} != {1}", sizeof(uint24),3);
    return false;
}

bool data_unit_tests()
{
    cDebug("User-verifiable: ");
    cDebug("500 kB: {0}",Unit_kB*500);
    cDebug("500 GB: {0}",Unit_GB*500);
    cDebug("500 TB: {0}",Unit_TB*500);
    return true;
}

template<typename T>
static void CheckSize(Vector<CString>& names, Vector<szptr>& sizes)
{
    CString type_name = Stacktracer::DemangleSymbol(typeid(T).name());
    names.push_back(type_name);
    sizes.push_back(sizeof(T));

    Profiler::AddExtraData(cStringFormat("sizeof:{0}", CStrReplace(type_name, "::", "_")),
                           cast_pod(sizeof(T)));
}

bool input_size_tests()
{
    Table::Header head = {"Type name", "Size"};
    Table::Table table(head);

    Vector<CString> typenames;
    Vector<szptr> typesizes;

    CheckSize<CIEvent>(typenames, typesizes);

    CheckSize<CIKeyEvent>(typenames, typesizes);
    CheckSize<CITextEvent>(typenames, typesizes);

    CheckSize<CIMouseMoveEvent>(typenames, typesizes);
    CheckSize<CIMouseButtonEvent>(typenames, typesizes);
    CheckSize<CIScrollEvent>(typenames, typesizes);

    CheckSize<CIWriteEvent>(typenames, typesizes);
    CheckSize<CIWEditEvent>(typenames, typesizes);

    CheckSize<CIControllerAtomicEvent>(typenames, typesizes);
    CheckSize<CIControllerAtomicUpdateEvent>(typenames, typesizes);
    CheckSize<CIControllerState>(typenames, typesizes);

    CheckSize<CIHapticEvent>(typenames, typesizes);
    CheckSize<CIDropEvent>(typenames, typesizes);
    CheckSize<CISensorEvent>(typenames, typesizes);

    CheckSize<CITouchTapEvent>(typenames, typesizes);
    CheckSize<CITouchMotionEvent>(typenames, typesizes);
    CheckSize<CIMTouchMotionEvent>(typenames, typesizes);
    CheckSize<CIGestureEvent>(typenames, typesizes);

    table.push_back(Table::GenColumn(typenames.data(),typenames.size()));
    table.push_back(Table::GenColumn(typesizes.data(),typesizes.size()));

    cBasicPrint("Sizes of input structures:");
    cBasicPrint("{0}", table);

    return true;
}

bool pix_size_tests()
{
    return false;
}

const constexpr CoffeeTest::Test _tests[7] = {
    {basic_tests,"Integer sizes","Checking the storage of integer types", false, false},
    {floating_storage_tests,"Floating-point sizes","Checking the storage of floating-point types",
     false, false},
    {longdoub_test,"Long double size","long double size test",true, false},
    {wrapping_tests,"Wrapping tests","Checking that numbers wrap correctly",
     true, false},
    {uint24_test,"Unsigned 24-bit integer","Special sauce",true, false},
    {data_unit_tests,"Verify data units","Special sauce",true, false},
    {input_size_tests,"Check sizes of input structures","Useful for aligning data optimally",
     true, false},
};

COFFEE_RUN_TESTS(_tests);