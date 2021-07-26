#pragma once

#include "common.h"

namespace gl::group {

// PixelStoreParameter
enum class pixel_store_parameter : ::libc_types::u32
{
#ifdef GL_PACK_ALIGNMENT
    pack_alignment = GL_PACK_ALIGNMENT,
#endif
#ifdef GL_PACK_IMAGE_DEPTH_SGIS
    pack_image_depth_sgis = GL_PACK_IMAGE_DEPTH_SGIS,
#endif
#ifdef GL_PACK_IMAGE_HEIGHT
    pack_image_height = GL_PACK_IMAGE_HEIGHT,
#endif
#ifdef GL_PACK_IMAGE_HEIGHT_EXT
    pack_image_height_ext = GL_PACK_IMAGE_HEIGHT_EXT,
#endif
#ifdef GL_PACK_LSB_FIRST
    pack_lsb_first = GL_PACK_LSB_FIRST,
#endif
#ifdef GL_PACK_RESAMPLE_OML
    pack_resample_oml = GL_PACK_RESAMPLE_OML,
#endif
#ifdef GL_PACK_RESAMPLE_SGIX
    pack_resample_sgix = GL_PACK_RESAMPLE_SGIX,
#endif
#ifdef GL_PACK_ROW_LENGTH
    pack_row_length = GL_PACK_ROW_LENGTH,
#endif
#ifdef GL_PACK_SKIP_IMAGES
    pack_skip_images = GL_PACK_SKIP_IMAGES,
#endif
#ifdef GL_PACK_SKIP_IMAGES_EXT
    pack_skip_images_ext = GL_PACK_SKIP_IMAGES_EXT,
#endif
#ifdef GL_PACK_SKIP_PIXELS
    pack_skip_pixels = GL_PACK_SKIP_PIXELS,
#endif
#ifdef GL_PACK_SKIP_ROWS
    pack_skip_rows = GL_PACK_SKIP_ROWS,
#endif
#ifdef GL_PACK_SKIP_VOLUMES_SGIS
    pack_skip_volumes_sgis = GL_PACK_SKIP_VOLUMES_SGIS,
#endif
#ifdef GL_PACK_SUBSAMPLE_RATE_SGIX
    pack_subsample_rate_sgix = GL_PACK_SUBSAMPLE_RATE_SGIX,
#endif
#ifdef GL_PACK_SWAP_BYTES
    pack_swap_bytes = GL_PACK_SWAP_BYTES,
#endif
#ifdef GL_PIXEL_TILE_CACHE_SIZE_SGIX
    pixel_tile_cache_size_sgix = GL_PIXEL_TILE_CACHE_SIZE_SGIX,
#endif
#ifdef GL_PIXEL_TILE_GRID_DEPTH_SGIX
    pixel_tile_grid_depth_sgix = GL_PIXEL_TILE_GRID_DEPTH_SGIX,
#endif
#ifdef GL_PIXEL_TILE_GRID_HEIGHT_SGIX
    pixel_tile_grid_height_sgix = GL_PIXEL_TILE_GRID_HEIGHT_SGIX,
#endif
#ifdef GL_PIXEL_TILE_GRID_WIDTH_SGIX
    pixel_tile_grid_width_sgix = GL_PIXEL_TILE_GRID_WIDTH_SGIX,
#endif
#ifdef GL_PIXEL_TILE_HEIGHT_SGIX
    pixel_tile_height_sgix = GL_PIXEL_TILE_HEIGHT_SGIX,
#endif
#ifdef GL_PIXEL_TILE_WIDTH_SGIX
    pixel_tile_width_sgix = GL_PIXEL_TILE_WIDTH_SGIX,
#endif
#ifdef GL_UNPACK_ALIGNMENT
    unpack_alignment = GL_UNPACK_ALIGNMENT,
#endif
#ifdef GL_UNPACK_IMAGE_DEPTH_SGIS
    unpack_image_depth_sgis = GL_UNPACK_IMAGE_DEPTH_SGIS,
#endif
#ifdef GL_UNPACK_IMAGE_HEIGHT
    unpack_image_height = GL_UNPACK_IMAGE_HEIGHT,
#endif
#ifdef GL_UNPACK_IMAGE_HEIGHT_EXT
    unpack_image_height_ext = GL_UNPACK_IMAGE_HEIGHT_EXT,
#endif
#ifdef GL_UNPACK_LSB_FIRST
    unpack_lsb_first = GL_UNPACK_LSB_FIRST,
#endif
#ifdef GL_UNPACK_RESAMPLE_OML
    unpack_resample_oml = GL_UNPACK_RESAMPLE_OML,
#endif
#ifdef GL_UNPACK_RESAMPLE_SGIX
    unpack_resample_sgix = GL_UNPACK_RESAMPLE_SGIX,
#endif
#ifdef GL_UNPACK_ROW_LENGTH
    unpack_row_length = GL_UNPACK_ROW_LENGTH,
#endif
#ifdef GL_UNPACK_ROW_LENGTH_EXT
    unpack_row_length_ext = GL_UNPACK_ROW_LENGTH_EXT,
#endif
#ifdef GL_UNPACK_SKIP_IMAGES
    unpack_skip_images = GL_UNPACK_SKIP_IMAGES,
#endif
#ifdef GL_UNPACK_SKIP_IMAGES_EXT
    unpack_skip_images_ext = GL_UNPACK_SKIP_IMAGES_EXT,
#endif
#ifdef GL_UNPACK_SKIP_PIXELS
    unpack_skip_pixels = GL_UNPACK_SKIP_PIXELS,
#endif
#ifdef GL_UNPACK_SKIP_PIXELS_EXT
    unpack_skip_pixels_ext = GL_UNPACK_SKIP_PIXELS_EXT,
#endif
#ifdef GL_UNPACK_SKIP_ROWS
    unpack_skip_rows = GL_UNPACK_SKIP_ROWS,
#endif
#ifdef GL_UNPACK_SKIP_ROWS_EXT
    unpack_skip_rows_ext = GL_UNPACK_SKIP_ROWS_EXT,
#endif
#ifdef GL_UNPACK_SKIP_VOLUMES_SGIS
    unpack_skip_volumes_sgis = GL_UNPACK_SKIP_VOLUMES_SGIS,
#endif
#ifdef GL_UNPACK_SUBSAMPLE_RATE_SGIX
    unpack_subsample_rate_sgix = GL_UNPACK_SUBSAMPLE_RATE_SGIX,
#endif
#ifdef GL_UNPACK_SWAP_BYTES
    unpack_swap_bytes = GL_UNPACK_SWAP_BYTES,
#endif
}; // enum class pixel_store_parameter

} // namespace gl::group
