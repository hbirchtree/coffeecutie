#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/any_of.h>

#include "../common/block_layout.h"
#include "blam_base_types.h"

#include <bit>

namespace blam {
namespace hsc::bc {

enum class v1 : libc_types::i16;
enum class v2 : libc_types::i16;
enum class v4 : libc_types::i16;

} // namespace hsc::bc

namespace hsc {

template<typename BC>
struct bytecode_pointer;

}

struct xbox_version_t
{
    using bytecode_type      = hsc::bc::v1;
    using bytecode_pointer_t = hsc::bytecode_pointer<bytecode_type>;

    static constexpr auto version_v = version_t::xbox;
    static constexpr auto endian    = std::endian::little;
    using block_layout              = layout::volta_xbox;
};

struct pc_version_t
{
    using bytecode_type      = hsc::bc::v2;
    using bytecode_pointer_t = hsc::bytecode_pointer<bytecode_type>;

    static constexpr auto version_v = version_t::pc;
    static constexpr auto endian    = std::endian::little;
    using block_layout              = layout::volta_pc;
};

struct custom_version_t
{
    using bytecode_type      = hsc::bc::v2;
    using bytecode_pointer_t = hsc::bytecode_pointer<bytecode_type>;

    static constexpr auto version_v = version_t::custom_edition;
    static constexpr auto endian    = std::endian::little;
    using block_layout              = layout::volta_pc;
};

struct mcc_version_t
{
    using bytecode_type      = hsc::bc::v4;
    using bytecode_pointer_t = hsc::bytecode_pointer<bytecode_type>;

    static constexpr auto version_v = version_t::mcc;
    static constexpr auto endian    = std::endian::little;
    using block_layout              = layout::volta_pc;
};

struct trial_version_t
{
    using bytecode_type      = hsc::bc::v2;
    using bytecode_pointer_t = hsc::bytecode_pointer<bytecode_type>;

    static constexpr auto version_v = version_t::trial;
    static constexpr auto endian    = std::endian::little;
    using block_layout              = layout::volta_pc;
};

template<typename T>
concept is_game_version = stl_types::is_any_of<
    T,
    xbox_version_t,
    pc_version_t,
    custom_version_t,
    mcc_version_t,
    trial_version_t>;

using xbox_t = xbox_version_t;
using grbx_t = pc_version_t;

constexpr xbox_version_t   xbox_version;
constexpr pc_version_t     pc_version;
constexpr custom_version_t custom_version;
constexpr mcc_version_t    mcc_version;
constexpr trial_version_t  trial_version;

} // namespace blam
