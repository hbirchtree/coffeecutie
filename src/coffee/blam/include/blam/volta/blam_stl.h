#pragma once

#include "blam_scenario.h"
#include "blam_structures.h"

#include <algorithm>
#include <coffee/core/task_queue/task.h>
#include <corez/zlib.h>
#include <functional>
#include <iterator>
#include <peripherals/semantic/chunk.h>

namespace blam {

template<typename Ver>
struct map_container
{
    using result_type = result<map_container, map_load_error>;

    static void null_progress(std::string_view /*label*/, i16 /*progress*/)
    {
    }

    STATICINLINE auto from_bytes_async(
        rq::runtime_queue*                           queue,
        semantic::BytesConst const&                  map,
        Ver                                          ver,
        std::function<void(std::string_view, i16)>&& progress_cb
        = null_progress)
    {
        auto task = rq::dependent_task<void, result_type>::CreateSource(
            [map = map.view, ver, progress = std::move(progress_cb)]() mutable {
                return from_bytes(
                    semantic::BytesConst::ofContainer(map),
                    ver,
                    std::move(progress));
            });
        auto fut = task->output.get_future();
        auto res = rq::runtime_queue::Queue(queue, std::move(task));
        if(res.has_error())
        {
            decltype(task->output) error;
            error.set_value(
                stl_types::failure(map_load_error::failed_async_launch));
            return error.get_future();
        }
        return fut;
    }

    STATICINLINE result<map_container, map_load_error> from_bytes(
        semantic::BytesConst const&                  map,
        Ver                                          ver,
        std::function<void(std::string_view, i16)>&& progress = null_progress)
    {
        if(map.size < sizeof(file_header_t))
            return map_load_error::map_file_too_small;

        progress("Reading map header", 0);

        file_header_t const* header
            = file_header_t::from_data(map, ver).value();

        if(header->version != version_t::xbox)
        {
            if(!header)
                return map_load_error::not_a_map;
            progress("Reading tag index", 100);
            auto const* tags_index = &tag_index_t<Ver>::from_header(header);
            progress("Complete!", -1);
            return map_container{
                .map   = header,
                .tags  = tags_index,
                .magic = tags_index->get_magic(header, map.size),
                .store = map,
            };
        }

        progress("Preparing map decompression", 10);
        semantic::mem_chunk<char> decompressed;
        using namespace libc_types::size_literals;
        using zlib = ::zlib::codec;

        auto compressed_segment = *map.at(
            sizeof(file_header_t),
            (map.size - sizeof(file_header_t) - header->trailing_space));

        progress("Copying file header to memory", 15);
        /* Time to decompress! */
        decompressed
            = semantic::mem_chunk<char>::withSize(sizeof(file_header_t));
        header->copy_to(decompressed.as<file_header_t>()[0]);
        auto map_data = std::move(decompressed.allocation);
        map_data.reserve(header->decomp_len);

        progress("Decompressing map data", 25);
        auto err = zlib::decompress(
            compressed_segment.view,
            map_data,
            zlib::options_t{.chunk_size = 10_MB});
        if(err)
            return map_load_error::decompression_error;
        decompressed = semantic::mem_chunk<char>::ofContainer(map_data);
        header       = file_header_t::from_data(decompressed, ver).value();

        if(!header)
            return map_load_error::not_a_map;

        progress("Reading tag index", 100);
        auto const* tags_index = &tag_index_t<Ver>::from_header(header);

        progress("Complete!", -1);

        return map_container{
            .map          = header,
            .tags         = tags_index,
            .magic        = tags_index->get_magic(header, map.size),
            .store        = decompressed,
            .decompressed = std::move(map_data),
        };
    }

    file_header_t const*    map{nullptr};
    tag_index_t<Ver> const* tags{nullptr};
    magic_data_t            magic;

    semantic::mem_chunk<const char> store;
    std::vector<char>               decompressed{};

    inline std::string_view name() const
    {
        return map->full_mapname();
    }

    inline std::string_view internal_name() const
    {
        return map->name;
    }

    template<typename T>
    inline std::string_view get_name(T const* ref) const
    {
        return ref->to_name().to_string(magic);
    }
};

template<typename Ver>
class tag_index_view
{
    tag_index_t<Ver> const* m_idx;
    file_header_t const*    m_file;
    magic_data_t            m_magic;

  public:
    using span_type = semantic::Span<const tag_t>;
    using iterator  = span_type::iterator;

    tag_index_view() : m_idx(nullptr), m_file(nullptr)
    {
    }

    tag_index_view(map_container<Ver> const& map) :
        m_idx(map.tags), m_file(map.map), m_magic(map.magic)
    {
    }

    file_header_t const* file() const
    {
        return m_file;
    }
    tag_t const* tags() const
    {
        return m_idx;
    }
    span_type span() const
    {
        if(!m_idx)
            return span_type();
        return span_type(m_idx->tags(m_file), m_idx->tag_count);
    }

    iterator begin() const
    {
        return span().begin();
    }
    iterator end() const
    {
        return span().end();
    }

    template<tag_class_t... Class>
    /*! Efficient binary search through the tags */
    iterator find(tagref_typed_t<Class...> const& tag) const
    {
        /* TODO: Add tag validation */

        auto tag_unwrapped = *C_RCAST<tagref_t const*>(&tag);
        return find(tag_unwrapped);
    }

    /*! Efficient binary search through the tags */
    iterator find(tagref_t const& tag) const
    {
        if(tag.unknown != 0)
            Throw(undefined_behavior("invalid tagref_t"));

        return find(tag.tag_id);
    }

    /*! Efficient binary search through the tags */
    iterator find(u32 tag_id) const
    {
        /* Tag index has sequential tag IDs, allowing binary search */
        i32  size    = m_idx->tag_count;
        auto end_tag = (*(--end())).tag_id;
        auto it      = begin();

        if(tag_id > end_tag)
            return end();

        while(size > 1)
        {
            if(size % 2 == 0)
                size /= 2;
            else
                size = size / 2 + 1;
            auto remaining = std::distance(it, end()) - 1;
            size           = std::min<i32>(size, remaining);

            auto mid     = it + size;
            auto mid_tag = (*mid).tag_id;

            if(mid_tag == tag_id)
                return mid;

            if(tag_id > mid_tag)
                it = it + size;
            if(it == end())
                break;
        }

        return end();
    }

    /*! Sequential search of tags based on tag name */
    iterator find(std::string_view name) const
    {
        for(iterator it = begin(); it != end(); ++it)
        {
            auto it_name = (*it).to_name().to_string(m_magic);
            if(it_name == name)
                return it;
        }

        return end();
    }

    template<typename T, typename Key>
    std::optional<T const*> data(Key const& key) const
    {
        auto it = find(key);
        if(it == end())
            return std::nullopt;
        auto out = (*it).template data<T>(m_magic);
        if(!out.has_value())
            return std::nullopt;
        return out.value();
    }
};

} // namespace blam
