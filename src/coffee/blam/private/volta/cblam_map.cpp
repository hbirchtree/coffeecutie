#include <blam/volta/cblam_map.h>
#include <coffee/core/types/chunk.h>

namespace blam {

using semantic::BytesConst;
using namespace semantic::chunk_ops;

file_header_t const* file_header_t::from_data(
    semantic::Bytes const& data, pc_version_t)
{
    file_header_t const* fh = C_RCAST<file_header_t const*>(data.data);

    if(fh->version != version_t::pc)
        Throw(map_load_error("invalid map version: not a pc map"));

    auto file_head = BytesConst::ofBytes(fh->id);
    auto targ_head = BytesConst::ofBytes(header_head, 4);

    auto file_foot = BytesConst::ofBytes(fh->footer);
    auto targ_foot = BytesConst::ofBytes(header_foot, 4);

    if(!MemCmp(file_head, targ_head) || !MemCmp(file_foot, targ_foot))
        Throw(map_load_error("map failed endian check"));

    return fh;
}

file_header_t const* file_header_t::from_data(
    semantic::Bytes const& data, custom_version_t)
{
    file_header_t const* fh = C_RCAST<file_header_t const*>(data.data);

    if(fh->version != version_t::custom_edition)
        Throw(map_load_error("invalid map version: not a custom edition map"));

    auto file_head = BytesConst::ofBytes(fh->id);
    auto targ_head = BytesConst::ofBytes(header_head, 4);

    auto file_foot = BytesConst::ofBytes(fh->footer);
    auto targ_foot = BytesConst::ofBytes(header_foot, 4);

    if(!MemCmp(file_head, targ_head) || !MemCmp(file_foot, targ_foot))
        Throw(map_load_error("map failed endian check"));

    return fh;
}

file_header_t const* file_header_t::from_data(
    const semantic::Bytes& data, xbox_version_t)
{
    file_header_t const* fh = C_RCAST<file_header_t const*>(data.data);

    if(fh->version != version_t::xbox)
        Throw(map_load_error("invalid map version: not an xbox map"));

    auto file_head = BytesConst::ofBytes(fh->id);
    auto targ_head = BytesConst::ofBytes(header_head, 4);

    auto file_foot = BytesConst::ofBytes(fh->footer);
    auto targ_foot = BytesConst::ofBytes(header_foot, 4);

    if(!MemCmp(file_head, targ_head) || !MemCmp(file_foot, targ_foot))
        Throw(map_load_error("map failed endian check"));

    return fh;
}

} // namespace blam
