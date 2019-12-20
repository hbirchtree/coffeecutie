#include <blam/volta/cblam_map.h>
#include <coffee/core/types/chunk.h>

namespace blam {
namespace detail {

c_cptr magic_ptr(c_cptr base, i32 magic, i32 offset)
{
    const byte_t* ptr = (C_FCAST<const byte_t*>(base)) + offset - magic;
    return (ptr < base) ? nullptr : ptr;
}

} // namespace detail

using semantic::Bytes;
using namespace semantic::chunk_ops;

file_header_t const* file_header_t::from_data(
    const semantic::Bytes& data, pc_version_t)
{
    file_header_t const* fh = C_RCAST<file_header_t const*>(data.data);

    if(fh->version != version_t::pc)
        return nullptr;

    auto file_head = Bytes::Create(fh->id);
    auto targ_head = Bytes::From(header_head, 4);

    auto file_foot = Bytes::Create(fh->footer);
    auto targ_foot = Bytes::From(header_foot, 4);

    if(!MemCmp(file_head, targ_head) || !MemCmp(file_foot, targ_foot))
        return nullptr;

    return fh;
}

file_header_t const* file_header_t::from_data(
    semantic::Bytes const& data, custom_version_t)
{
    file_header_t const* fh = C_RCAST<file_header_t const*>(data.data);

    if(fh->version != version_t::custom_edition)
        return nullptr;

    auto file_head = Bytes::Create(fh->id);
    auto targ_head = Bytes::From(header_head, 4);

    auto file_foot = Bytes::Create(fh->footer);
    auto targ_foot = Bytes::From(header_foot, 4);

    if(!MemCmp(file_head, targ_head) || !MemCmp(file_foot, targ_foot))
        return nullptr;

    return fh;
}

file_header_t const* file_header_t::from_data(
    const semantic::Bytes& data, xbox_version_t)
{
    file_header_t const* fh = C_RCAST<file_header_t const*>(data.data);

    if(fh->version != version_t::xbox)
        return nullptr;

    auto file_head = Bytes::Create(fh->id);
    auto targ_head = Bytes::From(header_head, 4);

    auto file_foot = Bytes::Create(fh->footer);
    auto targ_foot = Bytes::From(header_foot, 4);

    if(!MemCmp(file_head, targ_head) || !MemCmp(file_foot, targ_foot))
        return nullptr;

    return fh;
}

} // namespace blam
