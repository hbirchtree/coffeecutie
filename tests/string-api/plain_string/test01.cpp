#include <coffee/core/CUnitTesting>
#include <coffee/core/libc_types.h>
#include <peripherals/libc/string_ops.h>

using namespace Coffee;
using namespace assertion;

static constexpr cstring  normal_str = "String with content\nHello Hello";
static constexpr cwstring wide_str   = L"String with content\nHello Hello";

static constexpr cstring low_normal_string = "string with content\nhello hello";
static constexpr cwstring low_wide_string = L"string with content\nhello hello";

static constexpr size_t str_len         = 31;
static constexpr size_t search_idx      = 20;
static constexpr size_t search_last_idx = 26;

bool plain_operations()
{
    /*
     *
     * String length
     *
     *
     */
    assertEquals(libc::str::len(normal_str), str_len);
    assertEquals(libc::str::len(wide_str), str_len);

    assertEquals(libc::str::len(""), 0UL);
    assertEquals(libc::str::len(L""), 0UL);

    assertEquals(libc::str::len(C_CAST<cstring>(nullptr)), 0UL);
    assertEquals(libc::str::len(C_CAST<cwstring>(nullptr)), 0UL);

    /* String comparisons */
    assertTrue(libc::str::cmp(normal_str, normal_str));
    assertTrue(libc::str::cmp(wide_str, wide_str));

    assertEquals(
        libc::str::cmp<libc::str::comp_idx>(normal_str, normal_str), 0UL);
    assertEquals(libc::str::cmp<libc::str::comp_idx>(wide_str, wide_str), 0UL);

    assertTrue(
        libc::str::cmp<libc::str::comp_nocase>(normal_str, low_normal_string));
    assertTrue(
        libc::str::cmp<libc::str::comp_nocase>(wide_str, low_wide_string));

    assertEquals(
        libc::str::cmp<libc::str::comp_nocase_idx>(
            normal_str, low_normal_string),
        0UL);
    assertEquals(
        libc::str::cmp<libc::str::comp_nocase_idx>(wide_str, low_wide_string),
        0UL);

    /*
     *
     * String searching
     *
     *
     */
    assert::Equals<cstring>(
        libc::str::find(normal_str, "Hello"), &normal_str[search_idx]);
    assert::Equals<cwstring>(
        libc::str::find(wide_str, L"Hello"), &wide_str[search_idx]);

    assert::Equals<cstring>(
        libc::str::find<libc::str::find_default>(normal_str, 'H'),
        &normal_str[search_idx]);
    assert::Equals<cwstring>(
        libc::str::find<libc::str::find_default>(wide_str, L'H'),
        &wide_str[search_idx]);

    assert::Equals<cstring>(
        libc::str::find<libc::str::find_reverse>(normal_str, 'H'),
        &normal_str[search_last_idx]);
    assert::Equals<cwstring>(
        libc::str::find<libc::str::find_reverse>(wide_str, L'H'),
        &wide_str[search_last_idx]);

    assert::Equals<cstring>(
        libc::str::find<libc::str::find_tokens>(normal_str, "xH"),
        &normal_str[search_idx]);
    assert::Equals<cwstring>(
        libc::str::find<libc::str::find_tokens>(wide_str, L"xH"),
        &wide_str[search_idx]);

    /*
     *
     * Conversions
     *
     *
     */
    assert::Equals<i8>(libc::str::from_string<i8>("10"), 10);
    assert::Equals<u8>(libc::str::from_string<u8>("10"), 10);
    assert::Equals<i16>(libc::str::from_string<i16>("10"), 10);
    assert::Equals<u16>(libc::str::from_string<u16>("10"), 10);
    assert::Equals<i32>(libc::str::from_string<i32>("10"), 10);
    assert::Equals<u32>(libc::str::from_string<u32>("10"), 10);
    assert::Equals<i64>(libc::str::from_string<i64>("10"), 10);
    assert::Equals<u64>(libc::str::from_string<u64>("10"), 10);

    assert::Equals<i8>(
        libc::str::from_string<i8, libc::str::convert_mode<2>>("10"), 2);

    assert::Equals<i8>(
        libc::str::from_string<i8, libc::str::convert_base_8>("10"), 8);
    assert::Equals<u8>(
        libc::str::from_string<u8, libc::str::convert_base_8>("10"), 8);
    assert::Equals<i16>(
        libc::str::from_string<i16, libc::str::convert_base_8>("10"), 8);
    assert::Equals<u16>(
        libc::str::from_string<u16, libc::str::convert_base_8>("10"), 8);
    assert::Equals<i32>(
        libc::str::from_string<i32, libc::str::convert_base_8>("10"), 8);
    assert::Equals<u32>(
        libc::str::from_string<u32, libc::str::convert_base_8>("10"), 8);
    assert::Equals<i64>(
        libc::str::from_string<i64, libc::str::convert_base_8>("10"), 8);
    assert::Equals<u64>(
        libc::str::from_string<u64, libc::str::convert_base_8>("10"), 8);

    assert::Equals<i8>(
        libc::str::from_string<i8, libc::str::convert_base_16>("10"), 16);
    assert::Equals<u8>(
        libc::str::from_string<u8, libc::str::convert_base_16>("10"), 16);
    assert::Equals<i16>(
        libc::str::from_string<i16, libc::str::convert_base_16>("10"), 16);
    assert::Equals<u16>(
        libc::str::from_string<u16, libc::str::convert_base_16>("10"), 16);
    assert::Equals<i32>(
        libc::str::from_string<i32, libc::str::convert_base_16>("10"), 16);
    assert::Equals<u32>(
        libc::str::from_string<u32, libc::str::convert_base_16>("10"), 16);
    assert::Equals<i64>(
        libc::str::from_string<i64, libc::str::convert_base_16>("10"), 16);
    assert::Equals<u64>(
        libc::str::from_string<u64, libc::str::convert_base_16>("10"), 16);

    assert::Equals<scalar>(libc::str::from_string<scalar>("3"), 3.f);
    assert::Equals<bigscalar>(libc::str::from_string<bigscalar>("3"), 3.0);
    assert::Equals<lscalar>(libc::str::from_string<lscalar>("3"), 3.0L);

    /*
     *
     * Boolean conversion
     *
     *
     */
    assertEquals(libc::str::from_string<bool>("true"), true);
    assertEquals(libc::str::from_string<bool>("false"), false);
    assertEquals(libc::str::from_string<bool>("TRUE"), true);
    assertEquals(libc::str::from_string<bool>("FALSE"), false);

    return true;
}

COFFEE_TEST_SUITE(1) = {{plain_operations, "Plain string operations"}};

COFFEE_EXEC_TESTS()
