#include <blam/volta/cblam_map.h>
#include <coffee/core/types/chunk.h>

#include <peripherals/stl/range_equal.h>

namespace blam {

using semantic::BytesConst;
using namespace semantic::chunk_ops;

stl_types::result<file_header_t const*, map_load_error> file_header_t::
    from_data(semantic::BytesConst const& data, pc_version_t)
{
    file_header_t const* fh = C_RCAST<file_header_t const*>(data.data);

    if(fh->version != version_t::pc)
        return map_load_error::incompatible_map_version_expected_pc;

    if(!stl_types::equal(header_head, fh->id) ||
       !stl_types::equal(header_foot, fh->footer))
        return map_load_error::incompatible_endianness;

    return fh;
}

stl_types::result<file_header_t const*, map_load_error> file_header_t::
    from_data(semantic::BytesConst const& data, custom_version_t)
{
    file_header_t const* fh = C_RCAST<file_header_t const*>(data.data);

    if(fh->version != version_t::custom_edition)
        return map_load_error::incompatible_map_version_expected_custom_edition;

    if(!stl_types::equal(header_head, fh->id) ||
       !stl_types::equal(header_foot, fh->footer))
        return map_load_error::incompatible_endianness;

    return fh;
}

stl_types::result<file_header_t const*, map_load_error> file_header_t::
    from_data(semantic::BytesConst const& data, xbox_version_t)
{
    file_header_t const* fh = C_RCAST<file_header_t const*>(data.data);

    if(fh->version != version_t::xbox)
        return map_load_error::incompatible_map_version_expected_xbox;

    if(!stl_types::equal(header_head, fh->id) ||
       !stl_types::equal(header_foot, fh->footer))
        return map_load_error::incompatible_endianness;

    return fh;
}

} // namespace blam
