#pragma once

#include "common.h"

namespace gl::group {

// PixelStoreParameter
enum class pixel_store_parameter : u32
{
    pack_alignment              = 0x0D05, // GL_PACK_ALIGNMENT
    pack_image_height           = 0x806C, // GL_PACK_IMAGE_HEIGHT
    pack_lsb_first              = 0x0D01, // GL_PACK_LSB_FIRST
    pack_resample_oml           = 0x8984, // GL_PACK_RESAMPLE_OML
    pack_row_length             = 0x0D02, // GL_PACK_ROW_LENGTH
    pack_skip_images            = 0x806B, // GL_PACK_SKIP_IMAGES
    pack_skip_pixels            = 0x0D04, // GL_PACK_SKIP_PIXELS
    pack_skip_rows              = 0x0D03, // GL_PACK_SKIP_ROWS
    pack_swap_bytes             = 0x0D00, // GL_PACK_SWAP_BYTES
    unpack_alignment            = 0x0CF5, // GL_UNPACK_ALIGNMENT
    unpack_image_height         = 0x806E, // GL_UNPACK_IMAGE_HEIGHT
    unpack_lsb_first            = 0x0CF1, // GL_UNPACK_LSB_FIRST
    unpack_resample_oml         = 0x8985, // GL_UNPACK_RESAMPLE_OML
    unpack_row_length           = 0x0CF2, // GL_UNPACK_ROW_LENGTH
    unpack_skip_images          = 0x806D, // GL_UNPACK_SKIP_IMAGES
    unpack_skip_pixels          = 0x0CF4, // GL_UNPACK_SKIP_PIXELS
    unpack_skip_rows            = 0x0CF3, // GL_UNPACK_SKIP_ROWS
    unpack_swap_bytes           = 0x0CF0, // GL_UNPACK_SWAP_BYTES
    pack_image_depth_sgis       = 0x8131, // GL_PACK_IMAGE_DEPTH_SGIS
    pack_image_height_ext       = 0x806C, // GL_PACK_IMAGE_HEIGHT_EXT
    pack_resample_sgix          = 0x842E, // GL_PACK_RESAMPLE_SGIX
    pack_row_length_nv          = 0x0D02, // GL_PACK_ROW_LENGTH_NV
    pack_skip_images_ext        = 0x806B, // GL_PACK_SKIP_IMAGES_EXT
    pack_skip_pixels_nv         = 0x0D04, // GL_PACK_SKIP_PIXELS_NV
    pack_skip_rows_nv           = 0x0D03, // GL_PACK_SKIP_ROWS_NV
    pack_skip_volumes_sgis      = 0x8130, // GL_PACK_SKIP_VOLUMES_SGIS
    pack_subsample_rate_sgix    = 0x85A0, // GL_PACK_SUBSAMPLE_RATE_SGIX
    pixel_tile_cache_size_sgix  = 0x8145, // GL_PIXEL_TILE_CACHE_SIZE_SGIX
    pixel_tile_grid_depth_sgix  = 0x8144, // GL_PIXEL_TILE_GRID_DEPTH_SGIX
    pixel_tile_grid_height_sgix = 0x8143, // GL_PIXEL_TILE_GRID_HEIGHT_SGIX
    pixel_tile_grid_width_sgix  = 0x8142, // GL_PIXEL_TILE_GRID_WIDTH_SGIX
    pixel_tile_height_sgix      = 0x8141, // GL_PIXEL_TILE_HEIGHT_SGIX
    pixel_tile_width_sgix       = 0x8140, // GL_PIXEL_TILE_WIDTH_SGIX
    unpack_image_depth_sgis     = 0x8133, // GL_UNPACK_IMAGE_DEPTH_SGIS
    unpack_image_height_ext     = 0x806E, // GL_UNPACK_IMAGE_HEIGHT_EXT
    unpack_resample_sgix        = 0x842F, // GL_UNPACK_RESAMPLE_SGIX
    unpack_row_length_ext       = 0x0CF2, // GL_UNPACK_ROW_LENGTH_EXT
    unpack_skip_images_ext      = 0x806D, // GL_UNPACK_SKIP_IMAGES_EXT
    unpack_skip_pixels_ext      = 0x0CF4, // GL_UNPACK_SKIP_PIXELS_EXT
    unpack_skip_rows_ext        = 0x0CF3, // GL_UNPACK_SKIP_ROWS_EXT
    unpack_skip_volumes_sgis    = 0x8132, // GL_UNPACK_SKIP_VOLUMES_SGIS
    unpack_subsample_rate_sgix  = 0x85A1, // GL_UNPACK_SUBSAMPLE_RATE_SGIX
}; // enum class pixel_store_parameter

} // namespace gl::group
