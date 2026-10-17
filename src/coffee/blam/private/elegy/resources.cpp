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

result<resource_view, resource_error> resource_loader::load(datum_index id)
{
    using stl_types::failure;
    auto const* res = resource(id);
    if(!res)
        return failure(resource_error::no_resource);
    auto magic    = map->magic();
    auto segments = layout->segments.data(magic).value_or(Span<raw_segment_t const>());
    auto const* segment = block_index<raw_segment_t>{res->segment_index}
                              .resolve(segments);
    if(!segment)
        return failure(resource_error::bad_segment);
    auto sizes = layout->sizes.data(magic).value_or(Span<raw_size_t const>());
    /* A segment's bytes in one page, trimmed to its size entry */
    auto part = [&](block_index<raw_page_t> index,
                    i32                     at,
                    block_index<raw_size_t> size,
                    Span<byte_t const>&     out,
                    std::shared_ptr<void const>& keep)
        -> optional<resource_error> {
        if(!index.valid())
            return std::nullopt;
        auto page = this->page(index);
        if(!page.has_value())
            return page.error();
        auto const& bytes = *page.value();
        if(at < 0 || static_cast<u32>(at) > bytes.size())
            return resource_error::page_out_of_bounds;
        out = Span<byte_t const>(bytes.data(), bytes.size()).subspan(at);
        if(auto const* entry = size.resolve(sizes);
           entry && entry->size.value() >= 0 &&
           static_cast<size_t>(entry->size.value()) < out.size())
            out = out.subspan(0, static_cast<size_t>(entry->size.value()));
        keep = page.value();
        return std::nullopt;
    };

    resource_view out;
    if(auto error = part(
           segment->primary_page,
           segment->primary_offset,
           segment->primary_size,
           out.data,
           out.keep))
        return failure(*error);
    auto info = gestalt->info_buffer.data(magic);
    i32  info_offset = res->info_offset, info_size = res->info_size;
    if(info_size > 0 && static_cast<u32>(info_offset) <= info.size() &&
       static_cast<u32>(info_size) <= info.size() - info_offset)
        out.definition = info.subspan(info_offset, info_size);
    /* Optional detail; the primary page is usable without it */
    part(
        segment->secondary_page,
        segment->secondary_offset,
        segment->secondary_size,
        out.secondary,
        out.keep_secondary);
    out.root = res->base_definition_address.value() & 0x0FFFFFFF;
    for(auto const& fix : res->resource_fixups.data(magic).value_or(Span<resource_fixup_t const>()))
        out.fixups[static_cast<u32>(fix.offset.value())] = fix.address;
    return out;
}

result<std::shared_ptr<std::vector<byte_t> const>, resource_error>
resource_loader::page(block_index<raw_page_t> index)
{
    using stl_types::failure;
    auto pages = layout->pages.data(map->magic()).value_or(Span<raw_page_t const>());
    auto const* page = index.resolve(pages);
    if(!page)
        return failure(resource_error::bad_segment);
    if(auto it = m_pages.find(index.value()); it != m_pages.end())
        return it->second;

    u64 csize = static_cast<u32>(page->compressed_size.value());
    u64 usize = static_cast<u32>(page->size.value());
    Span<byte_t const> compressed;
    std::vector<byte_t> read_buffer;
    if(page->cache.valid())
    {
        auto const* cache  = page->cache.resolve(external_caches());
        auto const* source = cache ? attached(cache->file_name()) : nullptr;
        if(!source)
            return failure(resource_error::missing_cache_file);
        read_buffer.resize(csize);
        if(!source->read(
               u64(source->resource_offset) + u32(page->offset.value()),
               Span<byte_t>(read_buffer.data(), read_buffer.size())))
            return failure(resource_error::page_out_of_bounds);
        compressed = Span<byte_t const>(read_buffer.data(), read_buffer.size());
    } else
    {
        u64 start = u64(map->resource_offset()) + u32(page->offset.value());
        if(start > map->data.size() || csize > map->data.size() - start)
            return failure(resource_error::page_out_of_bounds);
        compressed = map->data.subspan(start, csize);
    }
    auto out        = std::make_shared<std::vector<byte_t>>();
    if(page->codec.value() == page_codec_t::none)
        out->assign(compressed.begin(), compressed.end());
    else
    {
        out->resize(usize);
        if(page->codec.value() != page_codec_t::deflate ||
           !inflate_page(compressed, Span<byte_t>(out->data(), out->size())))
            return failure(resource_error::decompression_failed);
    }
    m_pages[index.value()] = out;
    return std::shared_ptr<std::vector<byte_t> const>(out);
}

} // namespace blam::elegy
