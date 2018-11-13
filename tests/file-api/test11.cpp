#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>
#include <coffee/core/types/chunk.h>

using namespace Coffee;

bool wrap_other_type()
{
    Matf4 test_data;

    auto wrapper = Bytes::From(test_data);

    return wrapper.size == wrapper.elements &&
           wrapper.size == sizeof(test_data);
}

bool unsafe_data_wrap()
{
    Matf4 test_data;

    auto wrapper = Bytes::Unsafe(&test_data, sizeof(test_data));

    return wrapper.size == sizeof(test_data) && wrapper.elements == 0;
}

bool unsafe_data_wrap_2()
{
    Matf4 test_data;

    auto wrapper =
        Bytes::Unsafe(&test_data, sizeof(test_data), sizeof(test_data));

    return wrapper.size == sizeof(test_data) &&
           wrapper.elements == wrapper.size;
}

bool span_vector()
{
    using SpanT = Span<i32>;

    Vector<i32> test_data;
    test_data.resize(100);

    auto wrapper = SpanT::CreateFrom(test_data);

    return wrapper.size != wrapper.elements &&
           wrapper.elements == test_data.size();
}

bool span_variable()
{
    using SpanT = Span<i32>;

    i32 test_data;

    SpanT wrapper = test_data;

    return wrapper.size == sizeof(i32) && wrapper.elements == 1;
}

bool memcpy_variable()
{
    Bytes source = Bytes::Alloc(100);
    Bytes target = Bytes::Alloc(100);

    source[25] = 10;
    source[26] = 20;
    source[27] = 30;
    source[50] = 10;

    MemCpy(source, target);

    return memcmp(&source[0], &target[0], target.size) == 0;
}

bool memcpy_stl_variable()
{
    Vector<byte_t> source;
    Bytes          target = Bytes::Alloc(100);

    source.resize(100);

    source[25] = 10;
    source[26] = 20;
    source[27] = 30;
    source[50] = 10;

    MemCpy(source, target);

    return memcmp(&source[0], &target[0], target.size) == 0;
}

bool memcpy_to_stl_variable()
{
    Bytes          source = Bytes::Alloc(100);
    Vector<byte_t> target;

    target.reserve(100);

    source[25] = 10;
    source[26] = 20;
    source[27] = 30;
    source[50] = 10;

    MemCpy(source, target);

    return memcmp(&source[0], &target[0], target.size()) == 0;
}

bool memcpy_tofrom_stl_variable()
{
    Vector<byte_t> target;
    Vector<byte_t> source;

    target.reserve(100);
    source.resize(100);

    source[25] = 10;
    source[26] = 20;
    source[27] = 30;
    source[50] = 10;

    MemCpy(target, source);

    return memcmp(&source[0], &target[0], target.size()) == 0;
}

COFFEE_TEST_SUITE(9) = {
    {wrap_other_type, "Wrap data type of different size"},
    {unsafe_data_wrap, "Wrapping regular data", "Uses unsafe semantics"},
    {unsafe_data_wrap_2,
     "Wrapping regular data, extended",
     "Uses unsafe semantics"},
    {span_vector, "Wrapping std::vector"},
    {span_variable, "Wrapping a variable in Bytes()"},

    {memcpy_variable, "MemCpy() between Bytes containers"},
    {memcpy_stl_variable, "MemCpy() from STL container to Bytes"},
    {memcpy_to_stl_variable, "MemCpy() from Bytes to STL container"},
    {memcpy_tofrom_stl_variable, "Wrapping between STL containers"},
};

COFFEE_EXEC_TESTS();
