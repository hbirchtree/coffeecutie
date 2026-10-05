#pragma once

#include <blam/transcode/kernels.h>

#include <string>
#include <unordered_map>

namespace mtx {

struct map_stats
{
    u32 converted{0}, skipped{0}, skipped_read{0};
    u32 saved_bytes{0};
    u32 reused{0};     /*!< converted, found in the shared_cache */
    u32 deswizzled{0}; /*!< Xbox images written linear */
};

/* Images in bitmaps.map already transcoded into out_bitmaps, by offset, so
 * the level maps sharing them encode each one once */
struct shared_cache
{
    struct entry
    {
        bool                 converted;
        blam::bitm::format_t source;
        blam::bitm::format_t format;
        u16                  mipmaps;
        u32                  size;
    };

    std::unordered_map<u32, entry> images;
};

/* Images encoded so far, of those to encode. Called from the encoding
 * threads. */
using progress_fn = std::function<void(u32 done, u32 total)>;

struct transcode_options
{
    /* Threads to encode images on, the output is the same for any count */
    unsigned threads{1};
    /* Write swizzled Xbox images linear, see transcode_map() */
    bool deswizzle{true};
    /* Kept for the same out_bitmaps, see shared_cache */
    shared_cache* cache{nullptr};
    progress_fn   progress;
};

struct map_transcode_result
{
    bool        ok{false};
    std::string error;
    map_stats   stats;
};

/*!
 * \brief Run `kernel` over every bitmap image of an uncompressed Halo PC,
 * Custom Edition or Xbox level map, in place.
 *
 * Pixels are read from `map` and `bitmaps` (the originals) and written at the
 * same offsets into `out_map` and `out_bitmaps`, which must start out as
 * copies of them. Each transcoded image's format/size/mip count is patched in
 * `out_map`. Several level maps can share one `bitmaps`/`out_bitmaps` pair:
 * an image they share comes out the same each time, as long as `bitmaps`
 * stays the untouched original. Images that don't fit their slot, or are in a
 * format the kernel doesn't handle, are left as they are. `bitmaps` may be
 * empty, then shared images count as skipped_read.
 *
 * With options.deswizzle, Xbox images stored swizzled are written linear,
 * with their swizzled flag cleared, before the kernel sees them (volume
 * textures excepted), so the runtime never deswizzles them. `kernel` may be
 * empty to only do that. Without it, swizzled images are left alone. Kernels
 * only get 2D images; cube maps and volume textures are not laid out as one
 * mip chain.
 */
map_transcode_result transcode_map(
    gsl::span<u8 const> map,
    gsl::span<u8 const> bitmaps,
    gsl::span<u8>       out_map,
    gsl::span<u8>       out_bitmaps,
    kernel_fn const&        kernel,
    transcode_options const& options = {});

} // namespace mtx
