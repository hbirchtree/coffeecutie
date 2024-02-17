#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/error/result.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/types.h>
#include <url/url.h>

namespace platform::file {

using libc_types::ptroff;
using libc_types::szptr;
using semantic::mem_chunk;
using semantic::RSCA;
using stl_types::failure;
using stl_types::result;
using stl_types::success;
using url::Url;

enum class mode_t
{
    none,

    file,
    directory,

    link,

    character,
    block,
    socket,
    fifo,
};

namespace permission_t {
enum permission_t : libc_types::u16
{
    execute = 01,
    write   = 02,
    read    = 04,

    none = 00,
    all  = read | write | execute,
};
}

C_FLAGS(permission_t::permission_t, libc_types::u16);

struct permissions_t
{
    permission_t::permission_t owner{}, group{}, other{};
};

static constexpr permissions_t default_permissions = {
    permission_t::all, permission_t::all, permission_t::none};

struct file_info_t
{
    libc_types::szptr size;
    mode_t            mode;
    permissions_t     perms;
};

struct read_params_t
{
    szptr  size{0};
    ptroff offset{0};
};

struct write_params_t
{
    ptroff offset{0};
};

struct create_params_t
{
    mode_t        mode{mode_t::none};
    permissions_t perms{
        .owner = permission_t::read | permission_t::write,
        .group = permission_t::read | permission_t::write,
    };
    bool recursive{false};
};

struct file_entry_t
{
    mode_t      mode;
    std::string name;
};

} // namespace platform::file
