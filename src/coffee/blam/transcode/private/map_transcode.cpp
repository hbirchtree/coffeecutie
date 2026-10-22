#include <blam/transcode/map_transcode.h>

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_bsp_structures.h>
#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_scenario.h>
#include <blam/volta/blam_shaders.h>
#include <blam/common/endian.h>
#include <blam/common/vertex_pack.h>
#include <blam/gx/quantize.h>
#include <blam/volta/blam_file_header.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_swizzle.h>
#include <blam/volta/blam_tag_classes.h>

#include <magic_enum/magic_enum.hpp>

#include <atomic>
#include <cstring>
#include <exception>
#include <mutex>
#include <set>
#include <thread>

namespace mtx {
namespace {

using libc_types::f32;
using libc_types::i16;
using libc_types::i32;
using libc_types::i8;

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

/* --- GX vertex arrays (gx-bsp) ---------------------------------------------
 * BSP: pos, diffuse uv (f32), lightmap uv (s16, 15 fraction bits), detail
 * uv (f32) = 32 bytes, over the material's vertices. Models: pos (f32), normal
 * (s8, 6 fraction bits), pad, uv (f32) = 24 bytes over the part's vertices.
 * Both big-endian, smaller than what they replace, and the indices are swapped
 * in place. The vertex type is patched so the reader knows. */

template<typename T>
T read_le(u8 const* p)
{
    T v;
    std::memcpy(&v, p, sizeof(T));
    return blam::from_le(v);
}

void put_be32(u8* p, u32 v)
{
    v = __builtin_bswap32(v); // host LE -> GX BE
    std::memcpy(p, &v, 4);
}

void put_bef(u8* p, f32 v)
{
    put_be32(p, std::bit_cast<u32>(v));
}

void put_be16(u8* p, u16 v)
{
    v = __builtin_bswap16(v);
    std::memcpy(p, &v, 2);
}

template<typename Ver>
f32 detail_scale(blam::map_container<Ver> const& c, blam::tagref_t const& ref)
{
    using blam::from_le;
    u32 const idx = ref.tag_id.index();
    if(!ref.tag_id.valid() || idx >= from_le(c.tags->tag_count))
        return 1.f;
    blam::tag_t const& t     = c.tags->tags(c.map)[idx];
    f32                scale = 1.f;
    if(t.matches(blam::tag_class_t::shader_environment))
    {
        if(auto sh = t.data<blam::shader::shader_env>(c.magic); sh.has_value())
            scale = from_le(sh.value()->diffuse.primary.scale);
    } else if(t.matches(blam::tag_class_t::shader_model))
    {
        if(auto sh = t.data<blam::shader::shader_model>(c.magic);
           sh.has_value())
            scale = from_le(sh.value()->maps.detail.scale);
    }
    return scale == 0.f ? 1.f : scale;
}

template<typename Ver>
u32 transcode_gx_vertices(
    blam::map_container<Ver> const& c, u8 const* base, gsl::span<u8> out_map)
{
    using blam::from_le;
    namespace vert = blam::vert;
    auto foff_of   = [&](void const* p) -> size_t {
        return reinterpret_cast<u8 const*>(p) - base;
    };
    auto swap_indices = [&](size_t off, size_t count) {
        for(size_t i = 0; i < count; i++)
        {
            u16 v = read_le<u16>(base + off + i * 2);
            v     = __builtin_bswap16(v);
            std::memcpy(out_map.data() + off + i * 2, &v, 2);
        }
    };
    std::set<size_t> done; // vertex/index ranges shared by several meshes
    u32              meshes = 0;

    /* Models */
    auto      vmag      = c.tags->vertex_magic(c.magic);
    u32 const tag_count = from_le(c.tags->tag_count);
    for(u32 ti = 0; ti < tag_count; ti++)
    {
        blam::tag_t const& t = c.tags->tags(c.map)[ti];
        if(!t.matches(blam::tag_class_t::mod2) &&
           !t.matches(blam::tag_class_t::mode))
            continue;
        auto hdr = t.data<blam::mod2::header<Ver>>(c.magic);
        if(hdr.has_error())
            continue;
        f32 const us    = from_le(hdr.value()->uvscale.x);
        f32 const vs    = from_le(hdr.value()->uvscale.y);
        auto      geoms = hdr.value()->geometries.data(c.magic);
        if(geoms.has_error())
            continue;
        for(auto const& geom : geoms.value())
            for(auto const& wrap : geom.meshes(c.magic))
            {
                blam::mod2::part const& p  = wrap.data;
                auto const              vt = from_le(p.vertex_type);
                if(vt != vert::vertex_type_t::mod2_compressed_vertex &&
                   vt != vert::vertex_type_t::mod2_uncompressed_vertex)
                    continue;
                try
                {
                    auto vspan = p.vertex_segment(*c.tags, c.magic).data(vmag);
                    auto ispan = p.index_segment(*c.tags).data(vmag);
                    if(vspan.has_error() || ispan.has_error() ||
                       vspan.value().empty())
                        continue;
                    u8 const* src =
                        reinterpret_cast<u8 const*>(vspan.value().data());
                    size_t const voff = foff_of(src);
                    size_t const vsize =
                        vspan.value().size_bytes() / vspan.value().size();
                    if(done.insert(voff).second)
                        for(size_t i = 0; i < vspan.value().size(); i++)
                        {
                            u8 const* v   = src + i * vsize;
                            u8*       dst = out_map.data() + voff + i * 24;
                            std::array<f32, 3> n;
                            f32                u, w;
                            if(vt ==
                               vert::vertex_type_t::mod2_compressed_vertex)
                            {
                                auto const p = blam::vertex_pack::unpack_11_11_10(read_le<u32>(v + 12));
                                n            = {p.x, p.y, p.z};
                                u = read_le<i16>(v + 24) / 32767.f;
                                w = read_le<i16>(v + 26) / 32767.f;
                            } else
                            {
                                n = {
                                    read_le<f32>(v + 12),
                                    read_le<f32>(v + 16),
                                    read_le<f32>(v + 20)};
                                u = read_le<f32>(v + 48);
                                w = read_le<f32>(v + 52);
                            }
                            for(u32 k = 0; k < 3; k++)
                            {
                                put_bef(dst + k * 4, read_le<f32>(v + k * 4));
                                dst[12 + k] =
                                    static_cast<u8>(blam::gx::to_s8_frac6(n[k]));
                            }
                            dst[15] = 0;
                            put_bef(dst + 16, u * us);
                            put_bef(dst + 20, w * vs);
                        }
                    size_t const ioff = foff_of(ispan.value().data());
                    if(done.insert(ioff).second)
                        swap_indices(ioff, ispan.value().size());
                } catch(std::exception const&)
                {
                    continue;
                }
                patch_u16(
                    out_map,
                    foff_of(&p.vertex_type),
                    static_cast<u16>(vert::vertex_type_t::mod2_gx_vertex));
                meshes++;
            }
    }

    /* Structure BSPs */
    auto scn = c.tags->scenario(c.map, c.magic);
    if(!scn)
        return meshes;
    auto bsps = (*scn)->bsp_info.data(c.magic);
    if(bsps.has_error())
        return meshes;
    for(auto const& bi : bsps.value())
    {
        auto const bm  = bi.bsp_magic(c.magic);
        auto       hdr = bi.to_bsp(bm).to_header().data(bm, blam::single_value);
        if(hdr.has_error())
            continue;
        auto lms = hdr.value()->lightmaps.data(bm);
        if(lms.has_error())
            continue;
        for(auto const& lm : lms.value())
        {
            auto mats = lm.materials.data(bm);
            if(mats.has_error())
                continue;
            for(blam::bsp::material const& mat : mats.value())
            {
                auto const type = from_le(mat.pc.type);
                if(type != vert::vertex_type_t::sbsp_uncompressed_vertex &&
                   type != vert::vertex_type_t::sbsp_compressed_vertex)
                    continue;
                bool const compressed =
                    type == vert::vertex_type_t::sbsp_compressed_vertex;
                auto const vref  = mat.vertices();
                auto       verts = vref.data(bm);
                auto       light = mat.light_verts().data(bm);
                auto       faces = mat.indices(*hdr.value()).data(bm);
                if(verts.has_error() || light.has_error() || faces.has_error())
                    continue;
                u32 const vsz    = compressed ? 32 : 56;
                u32 const lsz    = compressed ? 8 : 20;
                u32 const uv_off = compressed ? 24 : 48;
                u32 const count  = from_le(mat.pc.count);
                if(verts.value().size() < count * vsz ||
                   light.value().size() < count * lsz)
                    continue;
                f32 const    scale = detail_scale(c, mat.shader);
                u8 const*    src   = verts.value().data();
                u8 const*    lsrc  = light.value().data();
                size_t const voff  = foff_of(src);
                if(!done.insert(voff).second)
                    continue;
                for(u32 i = 0; i < count; i++)
                {
                    u8 const* v   = src + i * vsz;
                    u8 const* l   = lsrc + i * lsz;
                    u8*       dst = out_map.data() + voff + i * 32;
                    f32 const du  = read_le<f32>(v + uv_off);
                    f32 const dv  = read_le<f32>(v + uv_off + 4);
                    for(u32 k = 0; k < 3; k++)
                        put_bef(dst + k * 4, read_le<f32>(v + k * 4));
                    put_bef(dst + 12, du);
                    put_bef(dst + 16, dv);
                    for(u32 k = 0; k < 2; k++)
                        put_be16(
                            dst + 20 + k * 2,
                            static_cast<u16>(
                                compressed
                                    ? read_le<i16>(l + 4 + k * 2)
                                    : blam::gx::to_s16_frac15(
                                          read_le<f32>(l + 12 + k * 4))));
                    put_bef(dst + 24, du * scale);
                    put_bef(dst + 28, dv * scale);
                }
                size_t const ioff = foff_of(faces.value().data());
                if(done.insert(ioff).second)
                    swap_indices(ioff, faces.value().size() * 3);
                // The arrays start where the vertices did; keep that
                // reachable through the compressed reference
                patch_u32(
                    out_map,
                    foff_of(&mat.pc.compressed_vertices.offset),
                    from_le(vref.offset));
                patch_u16(
                    out_map,
                    foff_of(&mat.pc.type),
                    static_cast<u16>(vert::vertex_type_t::sbsp_gx_vertex));
                meshes++;
            }
        }
    }
    return meshes;
}

/* Each face of each level goes through the kernel as a 2D image and back to
 * where it was read, so the faces stay where the runtime looks for them */
template<typename Ver>
std::optional<transcode_result> encode_cube(
    kernel_fn const&             kernel,
    blam::bitm::image_t const&   img,
    gsl::span<u8 const>          src_px)
{
    using blam::from_le;
    using blam::bitm::format_t;

    u16 const levels = std::max<u16>(from_le(img.mipmaps), 1);
    i16 const w      = from_le(img.isize.x);
    i16 const h      = from_le(img.isize.y);

    transcode_result out{
        .format  = from_le(img.format),
        .mipmaps = from_le(img.mipmaps),
        .data    = {src_px.begin(), src_px.end()},
    };
    std::optional<format_t> format;
    for(u16 level = 0; level < levels; level++)
    {
        u32 const size = img.layer_mip_bytes(level);
        for(u32 face = 0; face < 6; face++)
        {
            u32 const off = img.template cube_face_offset<Ver>(face, level);
            if(!size || off + size > src_px.size())
                return std::nullopt;
            auto piece = kernel(
                from_le(img.format),
                src_px.subspan(off, size),
                size,
                static_cast<u16>(std::max(w >> level, 1)),
                static_cast<u16>(std::max(h >> level, 1)),
                1);
            if(!piece || piece->data.size() != size ||
               (format && piece->format != *format))
                return std::nullopt;
            format = piece->format;
            std::memcpy(out.data.data() + off, piece->data.data(), size);
        }
    }
    out.format = *format;
    return out;
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
        bool                            cube{false};
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
                    blam::reference<u8>{img.size, img.offset}
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

            // Kernels encode one 2D mip chain; cube faces go one at a time,
            // volume slices are left alone
            auto const type = from_le(img.type);
            bool const cube = type == blam::bitm::type_t::tex_cube;
            if(!kernel || type == blam::bitm::type_t::tex_3d ||
               (cube && !options.cube_faces))
            {
                st.skipped++;
                continue;
            }
            if(img.shared())
                shared_jobs[from_le(img.offset)] = jobs.size();
            jobs.push_back(
                {&img, src_px, dst, std::nullopt, std::move(linear), cube});
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
                if(job.cube)
                    job.result = encode_cube<Ver>(kernel, *job.img, job.src_px);
                else
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

    if(options.gx_vertices)
        st.gx_meshes = transcode_gx_vertices(c, base, out_map);

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
