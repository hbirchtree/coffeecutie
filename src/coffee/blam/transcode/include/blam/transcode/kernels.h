#pragma once

#include <blam/volta/blam_bitm.h>
#include <gsl/span>

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace mtx {

using libc_types::u16;
using libc_types::u32;
using libc_types::u8;

/* --- transcode kernel interface --------------------------------------------
 * A "kernel" is one target's whole encode step, in a shape that's the same
 * for every target: given a source image's format/pixels/dims/mip count,
 * either produce the bytes to write in its place (+ the on-disk format code
 * to patch into image_t::format, + GX maxlod / mip levels for targets that
 * have mips) or return nullopt to leave the source untouched. Adding a new
 * target (another game console, another GPU) means writing one function
 * matching `kernel_fn` -- transcode_map()'s per-image loop doesn't change.
 * `format` is the RAW on-disk value already computed for the target (e.g.
 * gexxo::native::to_blam(gx_format) for Gekko) -- kernels own their own
 * format-marker scheme; the caller just writes whatever value comes back.
 * Kernels keep no state, so several threads can share one. */
struct transcode_result
{
    blam::bitm::format_t format; // patched into image_t::format verbatim
    /* Patched into image_t::mipmaps verbatim: GX max LOD for Gekko, which
     * gx-bsp reads that way, else the level count BlamGraphics uploads */
    u16             mipmaps;
    std::vector<u8> data;
};

using kernel_fn = std::function<std::optional<transcode_result>(
    blam::bitm::format_t src_fmt,
    gsl::span<u8 const>  src_px,
    u32                  src_size,
    u16                  w,
    u16                  h,
    u16                  src_mip_count)>;

namespace gekko {
/* `lightmap_format` "cmpr"/"compressed" sends uncompressed colour (lightmaps)
 * through CMPR instead of native RGB565 */
kernel_fn make_kernel(std::optional<std::string> const& lightmap_format);
} // namespace gekko

namespace powervr {
kernel_fn make_kernel();
} // namespace powervr

/* etc2comp effort, 0-100. 40 is ETCCOMP_DEFAULT_EFFORT_LEVEL; lower is
 * faster and coarser */
constexpr float default_etc_effort = 40.f;

namespace es2 {
kernel_fn make_kernel(float effort = default_etc_effort);
} // namespace es2

namespace es3 {
kernel_fn make_kernel(float effort = default_etc_effort);
} // namespace es3

/* Kernel by target name: Gekko, PowerVR, ES2, ES3. nullopt if unknown. */
std::optional<kernel_fn> make_kernel(
    std::string_view                  target,
    std::optional<std::string> const& lightmap_format = std::nullopt,
    float                             etc_effort      = default_etc_effort);

} // namespace mtx
