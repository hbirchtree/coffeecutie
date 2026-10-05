#include <blam/transcode/map_transcode.h>

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_endian.h>
#include <blam/volta/blam_file_header.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_swizzle.h>
#include <blam/volta/blam_tag_classes.h>

#include <magic_enum/magic_enum.hpp>

#include <atomic>
#include <cstring>
#include <exception>
#include <mutex>
#include <thread>

namespace mtx {
namespace {

using libc_types::i16;

void patch_u16(gsl::span<u8> buf, size_t off, u16 v)
{
    std::memcpy(buf.data() + off, &v, sizeof(v)); // host LE == map LE
}

void patch_u32(gsl::span<u8> buf, size_t off, u32 v)
{
    std::memcpy(buf.data() + off, &v, sizeof(v));
}

/* Bytes per pixel of the formats the Xbox swizzles, 0 for the rest */
u32 swizzled_bpp(blam::bitm::format_t format)
{
    using blam::bitm::format_t;
    switch(format)
    {
    case format_t::A8:
    case format_t::Y8:
    case format_t::AY8:
    case format_t::P8:
        return 1;
    case format_t::A8Y8:
    case format_t::R5G6B5:
    case format_t::A1RGB5:
    case format_t::ARGB4:
        return 2;
    case format_t::XRGB8:
    case format_t::ARGB8:
        return 4;
    default:
        return 0;
    }
}

/* Linear copy of an Xbox Morton-swizzled image, laid out the way
 * image_t::data() and cube_face<xbox_version_t>() read it: each level after
 * the last, and for cube maps each face's chain padded to 128 bytes. Levels
 * that go below one pixel on an axis are left as they are, a swizzle of a
 * single row or column is the identity. Volume textures are not handled. */
std::optional<std::vector<u8>> deswizzle_image(
    blam::bitm::image_t const& img, gsl::span<u8 const> px)
{
    using blam::from_le;
    using blam::bitm::type_t;

    auto const type = from_le(img.type);
    u32 const  bpp  = swizzled_bpp(from_le(img.format));
    if(bpp == 0 || type == type_t::tex_3d)
        return std::nullopt;

    u32 const w      = static_cast<u32>(from_le(img.isize.x));
    u32 const h      = static_cast<u32>(from_le(img.isize.y));
    u16 const levels = std::max<u16>(from_le(img.mipmaps), 1);
    auto level_bytes = [&](u16 i) { return (w >> i) * (h >> i) * bpp; };

    u32 chain = 0;
    for(u16 i = 0; i < levels; i++)
        chain += level_bytes(i);
    u32 const faces  = type == type_t::tex_cube ? 6 : 1;
    u32 const stride = (chain + 127u) & ~127u;

    std::vector<u8> out(px.begin(), px.end());
    for(u32 face = 0; face < faces; face++)
    {
        u32 offset = face * stride;
        for(u16 i = 0; i < levels; i++)
        {
            u32 const wl = w >> i, hl = h >> i, size = level_bytes(i);
            if(wl == 0 || hl == 0)
                break;
            if(static_cast<size_t>(offset) + size > px.size())
                return std::nullopt;
            if(!blam::swizzle::deswizzle_bytes(
                   semantic::Span<u8 const>(px.data() + offset, size),
                   semantic::Span<u8>(out.data() + offset, size),
                   wl,
                   hl,
                   bpp))
                return std::nullopt;
            offset += size;
        }
    }
    return out;
}

shared_cache::entry const* find_cached(
    shared_cache const& cache, u32 offset, blam::bitm::format_t source)
{
    auto it = cache.images.find(offset);
    if(it == cache.images.end() || it->second.source != source)
        return nullptr;
    return &it->second;
}

template<typename Ver>
map_transcode_result transcode(
    gsl::span<u8 const> map_span,
    gsl::span<u8 const> bitmaps_span,
    gsl::span<u8>       out_map,
    gsl::span<u8>       out_bitmaps,
    kernel_fn const&         kernel,
    transcode_options const& options)
{
    using blam::from_le;
    using blam::bitm::image_t;

    auto* const        cache    = options.cache;
    auto const&        progress = options.progress;
    map_transcode_result out;
    u8 const*            base = map_span.data();

    auto parsed = blam::map_container<Ver>::from_bytes(
        semantic::BytesConst::ofBytes(map_span.data(), map_span.size()),
        Ver{},
        [](std::string_view, libc_types::i16) {});
    if(parsed.has_error())
    {
        out.error = magic_enum::enum_name(parsed.error());
        return out;
    }
    auto const& c = parsed.value();
    if(!c.decompressed.empty())
    {
        out.error = "map is compressed";
        return out;
    }

    std::vector<gsl::span<image_t const>> bitmaps;
    u32 const tag_count = from_le(c.tags->tag_count);
    for(u32 i = 0; i < tag_count; i++)
    {
        blam::tag_t const& t = c.tags->tags(c.map)[i];
        if(!t.matches(blam::tag_class_t::bitm))
            continue;
        auto hr = t.data<blam::bitm::header_t>(c.magic);
        if(hr.has_error())
            continue;
        auto imgs = hr.value()->images.data(c.magic);
        if(imgs.has_error())
            continue;
        bitmaps.push_back(imgs.value());
    }

    // Member-address arithmetic (not offsetof) avoids -Winvalid-offsetof on
    // image_t's vector members.
    auto foff_of = [&](void const* p) -> size_t {
        return reinterpret_cast<u8 const*>(p) - base;
    };
    auto patch_header = [&](image_t const& img,
                            blam::bitm::format_t format,
                            u16                  mipmaps,
                            u32                  size) {
        patch_u16(out_map, foff_of(&img.format), static_cast<u16>(format));
        patch_u32(out_map, foff_of(&img.size), size);
        patch_u16(out_map, foff_of(&img.mipmaps), mipmaps);
    };

    // Gathered first, so the encoding can be spread over threads. Results
    // are applied in this order afterwards, the output does not depend on
    // the thread count.
    struct job_t
    {
        image_t const*                  img;
        gsl::span<u8 const>             src_px;
        u8*                             dst;
        std::optional<transcode_result> result;
        /* Deswizzled pixels src_px points into */
        std::vector<u8>                 linear;
    };
    /* A shared image listed again in this map, patched from its job */
    struct repeat_t
    {
        image_t const* img;
        size_t         job;
    };
    std::vector<job_t>              jobs;
    std::vector<repeat_t>           repeats;
    std::unordered_map<u32, size_t> shared_jobs;

    map_stats& st = out.stats;
    for(auto const& imgs : bitmaps)
    {
        for(image_t const& img : imgs)
        {
            auto const fmt = from_le(img.format);
            i16 const  w   = from_le(img.isize.x);
            i16 const  h   = from_le(img.isize.y);
            if(w <= 0 || h <= 0)
                continue;

            u32 const src_size = from_le(img.size);

            // Locate source pixels (as a span into the read-only input) +
            // their destination in the output copies.
            gsl::span<u8 const> src_px;
            u8*                 dst = nullptr;
            if(img.shared())
            {
                u32 const off = from_le(img.offset);
                if(static_cast<size_t>(off) + src_size > bitmaps_span.size() ||
                   static_cast<size_t>(off) + src_size > out_bitmaps.size())
                {
                    st.skipped_read++;
                    continue;
                }
                src_px = bitmaps_span.subspan(off, src_size);
                dst    = out_bitmaps.data() + off;

                // Already written to out_bitmaps for an earlier map
                auto const* known = cache ? find_cached(*cache, off, fmt) : nullptr;
                if(known && !known->converted)
                {
                    st.skipped++;
                    continue;
                }
                if(known)
                {
                    patch_header(img, known->format, known->mipmaps, known->size);
                    st.converted++;
                    st.reused++;
                    st.saved_bytes += src_size - known->size;
                    continue;
                }
                if(auto it = shared_jobs.find(off); it != shared_jobs.end())
                {
                    repeats.push_back({&img, it->second});
                    continue;
                }
            } else
            {
                // Pixels sit at a plain file offset, as BitmapCache reads them
                auto pix =
                    blam::reference<u8>{.count = img.size, .offset = img.offset}
                        .data(c.magic.ptr_only());
                if(pix.has_error())
                {
                    st.skipped_read++;
                    continue;
                }
                size_t const foff =
                    reinterpret_cast<u8 const*>(pix.value().data()) - base;
                if(foff + src_size > map_span.size())
                {
                    st.skipped_read++;
                    continue;
                }
                src_px = map_span.subspan(foff, src_size);
                dst    = out_map.data() + foff;
            }

            // Written linear right away, whether or not the kernel takes the
            // image after, and the flag cleared so the runtime leaves it be
            std::vector<u8> linear;
            u16 const flags    = static_cast<u16>(from_le(img.flags));
            u16 const swizzled = static_cast<u16>(blam::bitm::flags_t::swizzled);
            if((flags & swizzled) && !options.deswizzle)
            {
                st.skipped++;
                continue;
            }
            if(flags & swizzled)
            {
                if(auto deswizzled = deswizzle_image(img, src_px))
                {
                    linear = std::move(*deswizzled);
                    std::memcpy(dst, linear.data(), linear.size());
                    patch_u16(
                        out_map,
                        foff_of(&img.flags),
                        static_cast<u16>(flags & ~swizzled));
                    st.deswizzled++;
                    src_px = linear;
                } else
                {
                    // Kernels expect linear pixels
                    st.skipped++;
                    continue;
                }
            }

            // Kernels encode one 2D mip chain; cube faces and volume slices
            // are laid out differently
            auto const type = from_le(img.type);
            if(!kernel || type == blam::bitm::type_t::tex_cube ||
               type == blam::bitm::type_t::tex_3d)
            {
                st.skipped++;
                continue;
            }
            if(img.shared())
                shared_jobs[from_le(img.offset)] = jobs.size();
            jobs.push_back({&img, src_px, dst, std::nullopt, std::move(linear)});
        }
    }

    u32 const           total = static_cast<u32>(jobs.size());
    std::atomic<size_t> next{0};
    std::atomic<u32>    finished{0};
    std::exception_ptr  failure;
    std::mutex          failure_lock;
    auto                encode = [&] {
        for(size_t i; (i = next++) < jobs.size();)
        {
            auto& job = jobs[i];
            try
            {
                job.result = kernel(
                    from_le(job.img->format),
                    job.src_px,
                    static_cast<u32>(job.src_px.size()),
                    static_cast<u16>(from_le(job.img->isize.x)),
                    static_cast<u16>(from_le(job.img->isize.y)),
                    from_le(job.img->mipmaps));
            } catch(...)
            {
                std::lock_guard _(failure_lock);
                if(!failure)
                    failure = std::current_exception();
            }
            if(progress)
                progress(++finished, total);
        }
    };
    if(progress)
        progress(0, total);
    {
        std::vector<std::thread> workers;
        for(size_t i = 1; i < std::min<size_t>(options.threads, jobs.size());
            i++)
            workers.emplace_back(encode);
        encode();
        for(auto& worker : workers)
            worker.join();
    }
    if(failure)
        std::rethrow_exception(failure);

    for(auto& job : jobs)
    {
        image_t const& img      = *job.img;
        u32 const      src_size = static_cast<u32>(job.src_px.size());
        auto const&    result   = job.result;
        if(cache && img.shared())
            cache->images[from_le(img.offset)] = {
                .converted = result.has_value(),
                .source    = from_le(img.format),
                .format    = result ? result->format : from_le(img.format),
                .mipmaps   = result ? result->mipmaps : u16(0),
                .size      = result ? static_cast<u32>(result->data.size()) : 0,
            };
        if(!result)
        {
            st.skipped++;
            continue;
        }

        std::memcpy(job.dst, result->data.data(), result->data.size());

        // Patch the image_t in the output map: new format + size +
        // mip levels, verbatim from the kernel.
        patch_header(
            img,
            result->format,
            result->mipmaps,
            static_cast<u32>(result->data.size()));

        st.converted++;
        st.saved_bytes += src_size - static_cast<u32>(result->data.size());
    }
    for(auto const& repeat : repeats)
    {
        auto const& result = jobs[repeat.job].result;
        if(!result)
        {
            st.skipped++;
            continue;
        }
        patch_header(
            *repeat.img,
            result->format,
            result->mipmaps,
            static_cast<u32>(result->data.size()));
        st.converted++;
        st.saved_bytes += from_le(repeat.img->size) -
                          static_cast<u32>(result->data.size());
    }

    out.ok = true;
    return out;
}

} // namespace

map_transcode_result transcode_map(
    gsl::span<u8 const> map,
    gsl::span<u8 const> bitmaps,
    gsl::span<u8>       out_map,
    gsl::span<u8>       out_bitmaps,
    kernel_fn const&         kernel,
    transcode_options const& options)
{
    if(out_map.size() != map.size())
        return {.error = "output map is not a copy of the map"};
    if(map.size() < sizeof(blam::file_header_t))
        return {.error = "not a map"};

    auto const* header = reinterpret_cast<blam::file_header_t const*>(map.data());
    switch(blam::from_le(header->version))
    {
    case blam::version_t::pc:
        return transcode<blam::pc_version_t>(
            map, bitmaps, out_map, out_bitmaps, kernel, options);
    case blam::version_t::xbox:
        return transcode<blam::xbox_version_t>(
            map, bitmaps, out_map, out_bitmaps, kernel, options);
    case blam::version_t::custom_edition:
        return transcode<blam::custom_version_t>(
            map, bitmaps, out_map, out_bitmaps, kernel, options);
    default:
        return {.error = "only Halo PC, Custom Edition and Xbox maps are supported"};
    }
}

} // namespace mtx
