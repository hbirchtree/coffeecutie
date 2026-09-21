#pragma once

#include "common.h"

namespace gl::group {

// InternalFormatPName
enum class internal_format_prop : u32
{
    auto_generate_mipmap   = 0x8295, // GL_AUTO_GENERATE_MIPMAP
    clear_buffer           = 0x82B4, // GL_CLEAR_BUFFER
    clear_texture          = 0x9365, // GL_CLEAR_TEXTURE
    color_components       = 0x8283, // GL_COLOR_COMPONENTS
    color_encoding         = 0x8296, // GL_COLOR_ENCODING
    color_renderable       = 0x8286, // GL_COLOR_RENDERABLE
    compute_texture        = 0x82A0, // GL_COMPUTE_TEXTURE
    depth_renderable       = 0x8287, // GL_DEPTH_RENDERABLE
    filter                 = 0x829A, // GL_FILTER
    fragment_texture       = 0x829F, // GL_FRAGMENT_TEXTURE
    framebuffer_blend      = 0x828B, // GL_FRAMEBUFFER_BLEND
    framebuffer_renderable = 0x8289, // GL_FRAMEBUFFER_RENDERABLE
    framebuffer_renderable_layered =
        0x828A,                         // GL_FRAMEBUFFER_RENDERABLE_LAYERED
    generate_mipmap           = 0x8191, // GL_GENERATE_MIPMAP
    geometry_texture          = 0x829E, // GL_GEOMETRY_TEXTURE
    get_texture_image_format  = 0x8291, // GL_GET_TEXTURE_IMAGE_FORMAT
    get_texture_image_type    = 0x8292, // GL_GET_TEXTURE_IMAGE_TYPE
    image_compatibility_class = 0x82A8, // GL_IMAGE_COMPATIBILITY_CLASS
    image_format_compatibility_type =
        0x90C7,                           // GL_IMAGE_FORMAT_COMPATIBILITY_TYPE
    image_pixel_format          = 0x82A9, // GL_IMAGE_PIXEL_FORMAT
    image_pixel_type            = 0x82AA, // GL_IMAGE_PIXEL_TYPE
    image_texel_size            = 0x82A7, // GL_IMAGE_TEXEL_SIZE
    internalformat_alpha_size   = 0x8274, // GL_INTERNALFORMAT_ALPHA_SIZE
    internalformat_alpha_type   = 0x827B, // GL_INTERNALFORMAT_ALPHA_TYPE
    internalformat_blue_size    = 0x8273, // GL_INTERNALFORMAT_BLUE_SIZE
    internalformat_blue_type    = 0x827A, // GL_INTERNALFORMAT_BLUE_TYPE
    internalformat_depth_size   = 0x8275, // GL_INTERNALFORMAT_DEPTH_SIZE
    internalformat_depth_type   = 0x827C, // GL_INTERNALFORMAT_DEPTH_TYPE
    internalformat_green_size   = 0x8272, // GL_INTERNALFORMAT_GREEN_SIZE
    internalformat_green_type   = 0x8279, // GL_INTERNALFORMAT_GREEN_TYPE
    internalformat_preferred    = 0x8270, // GL_INTERNALFORMAT_PREFERRED
    internalformat_red_size     = 0x8271, // GL_INTERNALFORMAT_RED_SIZE
    internalformat_red_type     = 0x8278, // GL_INTERNALFORMAT_RED_TYPE
    internalformat_shared_size  = 0x8277, // GL_INTERNALFORMAT_SHARED_SIZE
    internalformat_stencil_size = 0x8276, // GL_INTERNALFORMAT_STENCIL_SIZE
    internalformat_stencil_type = 0x827D, // GL_INTERNALFORMAT_STENCIL_TYPE
    internalformat_supported    = 0x826F, // GL_INTERNALFORMAT_SUPPORTED
    max_depth                   = 0x8280, // GL_MAX_DEPTH
    max_height                  = 0x827F, // GL_MAX_HEIGHT
    max_layers                  = 0x8281, // GL_MAX_LAYERS
    max_width                   = 0x827E, // GL_MAX_WIDTH
    mipmap                      = 0x8293, // GL_MIPMAP
    num_sample_counts           = 0x9380, // GL_NUM_SAMPLE_COUNTS
    read_pixels                 = 0x828C, // GL_READ_PIXELS
    read_pixels_format          = 0x828D, // GL_READ_PIXELS_FORMAT
    read_pixels_type            = 0x828E, // GL_READ_PIXELS_TYPE
    samples                     = 0x80A9, // GL_SAMPLES
    shader_image_atomic         = 0x82A6, // GL_SHADER_IMAGE_ATOMIC
    shader_image_load           = 0x82A4, // GL_SHADER_IMAGE_LOAD
    shader_image_store          = 0x82A5, // GL_SHADER_IMAGE_STORE
    simultaneous_texture_and_depth_test =
        0x82AC, // GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST
    simultaneous_texture_and_depth_write =
        0x82AE, // GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE
    simultaneous_texture_and_stencil_test =
        0x82AD, // GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST
    simultaneous_texture_and_stencil_write =
        0x82AF, // GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE
    srgb_read               = 0x8297, // GL_SRGB_READ
    srgb_write              = 0x8298, // GL_SRGB_WRITE
    stencil_renderable      = 0x8288, // GL_STENCIL_RENDERABLE
    tess_control_texture    = 0x829C, // GL_TESS_CONTROL_TEXTURE
    tess_evaluation_texture = 0x829D, // GL_TESS_EVALUATION_TEXTURE
    texture_compressed      = 0x86A1, // GL_TEXTURE_COMPRESSED
    texture_compressed_block_height =
        0x82B2, // GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT
    texture_compressed_block_size = 0x82B3, // GL_TEXTURE_COMPRESSED_BLOCK_SIZE
    texture_compressed_block_width =
        0x82B1,                        // GL_TEXTURE_COMPRESSED_BLOCK_WIDTH
    texture_gather           = 0x82A2, // GL_TEXTURE_GATHER
    texture_gather_shadow    = 0x82A3, // GL_TEXTURE_GATHER_SHADOW
    texture_image_format     = 0x828F, // GL_TEXTURE_IMAGE_FORMAT
    texture_image_type       = 0x8290, // GL_TEXTURE_IMAGE_TYPE
    texture_shadow           = 0x82A1, // GL_TEXTURE_SHADOW
    texture_view             = 0x82B5, // GL_TEXTURE_VIEW
    vertex_texture           = 0x829B, // GL_VERTEX_TEXTURE
    view_compatibility_class = 0x82B6, // GL_VIEW_COMPATIBILITY_CLASS
    num_surface_compression_fixed_rates_ext =
        0x8F6E, // GL_NUM_SURFACE_COMPRESSION_FIXED_RATES_EXT
}; // enum class internal_format_prop

} // namespace gl::group
