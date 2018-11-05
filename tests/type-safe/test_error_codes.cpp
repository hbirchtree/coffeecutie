#include <coffee/core/CUnitTesting>
#include <peripherals/stl/types.h>

#include <coffee/core/CDebug>

using namespace Coffee;

struct level1_error_category : error_category
{
    virtual const char* name() const noexcept
    {
        return "a";
    }
    virtual std::string message(int) const
    {
        return "a:none";
    }
};

struct level2_error_category : error_category
{
    virtual const char* name() const noexcept
    {
        return "b";
    }
    virtual std::string message(int) const
    {
        return "b:none";
    }
};

struct level3_error_category : error_category
{
    virtual const char* name() const noexcept
    {
        return "c";
    }
    virtual std::string message(int) const
    {
        return "c:none";
    }
};

bool simple_error_casting()
{
    using inner      = domain_error_code<u32, level1_error_category>;
    using outer_cast = domain_error_code<u32, level2_error_category>;

    using outer = nested_domain_error_code<u32, level2_error_category, inner>;

    outer ec;

    ec                 = 1;
    ec.m_internalError = 2;

    auto& inner_value = ec.as<inner>();
    auto& outer_value = ec.as<outer_cast>();

#if 0
    cDebug("{0}: {1}", ec.category().name(), ec.message());
    cDebug("-{0}: {1}", inner_value.category().name(), inner_value.message());
    cDebug("-{0}: {1}", outer_value.category().name(), outer_value.message());
#endif

    return inner_value == 2 && outer_value == 1;
}

bool deep_error_casting()
{
    using inner       = domain_error_code<u32, level1_error_category>;
    using middle_cast = domain_error_code<u64, level2_error_category>;
    using outer_cast  = domain_error_code<u16, level3_error_category>;

    using middle = nested_domain_error_code<u64, level2_error_category, inner>;
    using outer  = nested_domain_error_code<u16, level3_error_category, middle>;

    outer ec;

    ec                                 = 1;
    ec.m_internalError                 = 2;
    ec.m_internalError.m_internalError = 3;

    auto& inner_value  = ec.as<inner>();
    auto& middle_value = ec.as<middle_cast>();
    auto& outer_value  = ec.as<outer_cast>();

#if 0
    cDebug("{0}: {1}", ec.category().name(), ec.message());
    cDebug("-{0}: {1}", inner_value.category().name(), inner_value.message());
    cDebug("-{0}: {1}", middle_value.category().name(), middle_value.message());
    cDebug("-{0}: {1}", outer_value.category().name(), outer_value.message());
#endif

    return inner_value == 3 && middle_value == 2 && outer_value == 1;
}

COFFEE_TEST_SUITE(2) = {
    {simple_error_casting, "Error casting", "1 level of nesting"},
    {deep_error_casting, "Deep error casting", "2 levels of nesting"}};

COFFEE_EXEC_TESTS()
