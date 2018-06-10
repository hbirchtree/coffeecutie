#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>

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

COFFEE_TEST_SUITE(5) = {
    {wrap_other_type, "Wrap data type of different size"},
    {unsafe_data_wrap, "Wrapping regular data", "Uses unsafe semantics"},
    {unsafe_data_wrap_2,
     "Wrapping regular data, extended",
     "Uses unsafe semantics"},
    {span_vector, "Wrapping std::vector"},
    {span_variable, "Wrapping a variable in Bytes()"},
};

COFFEE_EXEC_TESTS();
