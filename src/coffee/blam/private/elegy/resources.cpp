#include <blam/elegy/resources.h>

#include <zlib.h>

namespace blam::elegy {

bool inflate_page(Span<byte_t const> compressed, Span<byte_t> out)
{
    z_stream strm{};
    /* Raw deflate, no zlib header */
    if(inflateInit2(&strm, -MAX_WBITS) != Z_OK)
        return false;
    strm.next_in   = const_cast<Bytef*>(compressed.data());
    strm.avail_in  = static_cast<uInt>(compressed.size());
    strm.next_out  = out.data();
    strm.avail_out = static_cast<uInt>(out.size());
    int  rc        = inflate(&strm, Z_FINISH);
    bool ok        = (rc == Z_STREAM_END || rc == Z_OK) && strm.avail_out == 0;
    inflateEnd(&strm);
    return ok;
}

} // namespace blam::elegy
