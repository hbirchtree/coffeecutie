#include <coffee/core/CUnitTesting>

#include <coffee/core/CInput>

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
        return false;

    if(sizeof(size_t)!=8)
        return false;

    return true;
}

const constexpr szptr lscalar_expected = 16;

bool floating_storage_tests()
{
    if(sizeof(scalar)!=4)
        return false;
    if(sizeof(bigscalar)!=8)
        return false;
    if(sizeof(lscalar)!=lscalar_expected)
    {
		cWarning("Size of Lscalar: {0}, expected {1}",sizeof(lscalar),lscalar_expected);
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
    return sizeof(uint24)==3;
}

bool data_unit_tests()
{
    cDebug("User-verifiable: ");
    cDebug("500 kB: {0}",Unit_kB*500);
    cDebug("500 GB: {0}",Unit_GB*500);
    cDebug("500 TB: {0}",Unit_TB*500);
    return true;
}

bool input_size_tests()
{
    Table::Table table;

    Table::Header head;
    head.push_back("Type Name");
    head.push_back("Size");

    Vector<CString> typenames;
    Vector<szptr> typesizes;

    typenames.push_back("CIEvent");
    typesizes.push_back(sizeof(CIEvent));

    typenames.push_back("CIKeyEvent");
    typesizes.push_back(sizeof(CIKeyEvent));

    typenames.push_back("CITextEvent");
    typesizes.push_back(sizeof(CITextEvent));

    typenames.push_back("CIMouseMoveEvent");
    typesizes.push_back(sizeof(CIMouseMoveEvent));

    typenames.push_back("CIMouseButtonEvent");
    typesizes.push_back(sizeof(CIMouseButtonEvent));

    typenames.push_back("CIScrollEvent");
    typesizes.push_back(sizeof(CIScrollEvent));

    typenames.push_back("CIWriteEvent");
    typesizes.push_back(sizeof(CIWriteEvent));

    typenames.push_back("CIWEditEvent");
    typesizes.push_back(sizeof(CIWEditEvent));

    typenames.push_back("CIControllerAtomicEvent");
    typesizes.push_back(sizeof(CIControllerAtomicEvent));

    typenames.push_back("CIControllerAtomicUpdateEvent");
    typesizes.push_back(sizeof(CIControllerAtomicUpdateEvent));

    typenames.push_back("CIControllerState");
    typesizes.push_back(sizeof(CIControllerState));

    typenames.push_back("CIHapticEvent");
    typesizes.push_back(sizeof(CIHapticEvent));

    typenames.push_back("CIDropEvent");
    typesizes.push_back(sizeof(CIDropEvent));

    typenames.push_back("CISensorEvent");
    typesizes.push_back(sizeof(CISensorEvent));

    typenames.push_back("CITouchTapEvent");
    typesizes.push_back(sizeof(CITouchTapEvent));

    typenames.push_back("CITouchMotionEvent");
    typesizes.push_back(sizeof(CITouchMotionEvent));

    typenames.push_back("CIMTouchMotionEvent");
    typesizes.push_back(sizeof(CIMTouchMotionEvent));

    typenames.push_back("CIGestureEvent");
    typesizes.push_back(sizeof(CIGestureEvent));

    table.push_back(Table::GenColumn(typenames.data(),typenames.size()));
    table.push_back(Table::GenColumn(typesizes.data(),typesizes.size()));

    cBasicPrint("Sizes of input structures:");
    cBasicPrint("{0}",Table::GenTable(table,head));

    return true;
}

const constexpr CoffeeTest::Test _tests[6] = {
    {basic_tests,"Integer sizes","Checking the storage of integer types"},
    {floating_storage_tests,"Floating-point sizes","Checking the storage of floating-point types"},
    {wrapping_tests,"Wrapping tests","Checking that numbers wrap correctly"},
    {uint24_test,"Unsigned 24-bit integer","Special sauce",false},
    {data_unit_tests,"Verify data units","Special sauce",false},
    {input_size_tests,"Check sizes of input structures","Useful for aligning data optimally",false},
};

COFFEE_RUN_TESTS(_tests);
