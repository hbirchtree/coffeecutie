#pragma once

#include "common.h"

namespace gl::group {

// InternalFormatPName
enum class internal_format_prop : ::libc_types::u32
{
#ifdef GL_AUTO_GENERATE_MIPMAP
    auto_generate_mipmap = GL_AUTO_GENERATE_MIPMAP,
#endif
#ifdef GL_CLEAR_BUFFER
    clear_buffer = GL_CLEAR_BUFFER,
#endif
#ifdef GL_CLEAR_TEXTURE
    clear_texture = GL_CLEAR_TEXTURE,
#endif
#ifdef GL_COLOR_COMPONENTS
    color_components = GL_COLOR_COMPONENTS,
#endif
#ifdef GL_COLOR_ENCODING
    color_encoding = GL_COLOR_ENCODING,
#endif
#ifdef GL_COLOR_RENDERABLE
    color_renderable = GL_COLOR_RENDERABLE,
#endif
#ifdef GL_COMPUTE_TEXTURE
    compute_texture = GL_COMPUTE_TEXTURE,
#endif
#ifdef GL_DEPTH_RENDERABLE
    depth_renderable = GL_DEPTH_RENDERABLE,
#endif
#ifdef GL_FILTER
    filter = GL_FILTER,
#endif
#ifdef GL_FRAGMENT_TEXTURE
    fragment_texture = GL_FRAGMENT_TEXTURE,
#endif
#ifdef GL_FRAMEBUFFER_BLEND
    framebuffer_blend = GL_FRAMEBUFFER_BLEND,
#endif
#ifdef GL_FRAMEBUFFER_RENDERABLE
    framebuffer_renderable = GL_FRAMEBUFFER_RENDERABLE,
#endif
#ifdef GL_FRAMEBUFFER_RENDERABLE_LAYERED
    framebuffer_renderable_layered = GL_FRAMEBUFFER_RENDERABLE_LAYERED,
#endif
#ifdef GL_GENERATE_MIPMAP
    generate_mipmap = GL_GENERATE_MIPMAP,
#endif
#ifdef GL_GEOMETRY_TEXTURE
    geometry_texture = GL_GEOMETRY_TEXTURE,
#endif
#ifdef GL_GET_TEXTURE_IMAGE_FORMAT
    get_texture_image_format = GL_GET_TEXTURE_IMAGE_FORMAT,
#endif
#ifdef GL_GET_TEXTURE_IMAGE_TYPE
    get_texture_image_type = GL_GET_TEXTURE_IMAGE_TYPE,
#endif
#ifdef GL_IMAGE_COMPATIBILITY_CLASS
    image_compatibility_class = GL_IMAGE_COMPATIBILITY_CLASS,
#endif
#ifdef GL_IMAGE_FORMAT_COMPATIBILITY_TYPE
    image_format_compatibility_type = GL_IMAGE_FORMAT_COMPATIBILITY_TYPE,
#endif
#ifdef GL_IMAGE_PIXEL_FORMAT
    image_pixel_format = GL_IMAGE_PIXEL_FORMAT,
#endif
#ifdef GL_IMAGE_PIXEL_TYPE
    image_pixel_type = GL_IMAGE_PIXEL_TYPE,
#endif
#ifdef GL_IMAGE_TEXEL_SIZE
    image_texel_size = GL_IMAGE_TEXEL_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_ALPHA_SIZE
    internalformat_alpha_size = GL_INTERNALFORMAT_ALPHA_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_ALPHA_TYPE
    internalformat_alpha_type = GL_INTERNALFORMAT_ALPHA_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_BLUE_SIZE
    internalformat_blue_size = GL_INTERNALFORMAT_BLUE_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_BLUE_TYPE
    internalformat_blue_type = GL_INTERNALFORMAT_BLUE_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_DEPTH_SIZE
    internalformat_depth_size = GL_INTERNALFORMAT_DEPTH_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_DEPTH_TYPE
    internalformat_depth_type = GL_INTERNALFORMAT_DEPTH_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_GREEN_SIZE
    internalformat_green_size = GL_INTERNALFORMAT_GREEN_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_GREEN_TYPE
    internalformat_green_type = GL_INTERNALFORMAT_GREEN_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_PREFERRED
    internalformat_preferred = GL_INTERNALFORMAT_PREFERRED,
#endif
#ifdef GL_INTERNALFORMAT_RED_SIZE
    internalformat_red_size = GL_INTERNALFORMAT_RED_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_RED_TYPE
    internalformat_red_type = GL_INTERNALFORMAT_RED_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_SHARED_SIZE
    internalformat_shared_size = GL_INTERNALFORMAT_SHARED_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_STENCIL_SIZE
    internalformat_stencil_size = GL_INTERNALFORMAT_STENCIL_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_STENCIL_TYPE
    internalformat_stencil_type = GL_INTERNALFORMAT_STENCIL_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_SUPPORTED
    internalformat_supported = GL_INTERNALFORMAT_SUPPORTED,
#endif
#ifdef GL_MAX_DEPTH
    max_depth = GL_MAX_DEPTH,
#endif
#ifdef GL_MAX_HEIGHT
    max_height = GL_MAX_HEIGHT,
#endif
#ifdef GL_MAX_LAYERS
    max_layers = GL_MAX_LAYERS,
#endif
#ifdef GL_MAX_WIDTH
    max_width = GL_MAX_WIDTH,
#endif
#ifdef GL_MIPMAP
    mipmap = GL_MIPMAP,
#endif
#ifdef GL_NUM_SAMPLE_COUNTS
    num_sample_counts = GL_NUM_SAMPLE_COUNTS,
#endif
#ifdef GL_READ_PIXELS
    read_pixels = GL_READ_PIXELS,
#endif
#ifdef GL_READ_PIXELS_FORMAT
    read_pixels_format = GL_READ_PIXELS_FORMAT,
#endif
#ifdef GL_READ_PIXELS_TYPE
    read_pixels_type = GL_READ_PIXELS_TYPE,
#endif
#ifdef GL_SAMPLES
    samples = GL_SAMPLES,
#endif
#ifdef GL_SHADER_IMAGE_ATOMIC
    shader_image_atomic = GL_SHADER_IMAGE_ATOMIC,
#endif
#ifdef GL_SHADER_IMAGE_LOAD
    shader_image_load = GL_SHADER_IMAGE_LOAD,
#endif
#ifdef GL_SHADER_IMAGE_STORE
    shader_image_store = GL_SHADER_IMAGE_STORE,
#endif
#ifdef GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST
    simultaneous_texture_and_depth_test =
        GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST,
#endif
#ifdef GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE
    simultaneous_texture_and_depth_write =
        GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE,
#endif
#ifdef GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST
    simultaneous_texture_and_stencil_test =
        GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST,
#endif
#ifdef GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE
    simultaneous_texture_and_stencil_write =
        GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE,
#endif
#ifdef GL_SRGB_READ
    srgb_read = GL_SRGB_READ,
#endif
#ifdef GL_SRGB_WRITE
    srgb_write = GL_SRGB_WRITE,
#endif
#ifdef GL_STENCIL_RENDERABLE
    stencil_renderable = GL_STENCIL_RENDERABLE,
#endif
#ifdef GL_TESS_CONTROL_TEXTURE
    tess_control_texture = GL_TESS_CONTROL_TEXTURE,
#endif
#ifdef GL_TESS_EVALUATION_TEXTURE
    tess_evaluation_texture = GL_TESS_EVALUATION_TEXTURE,
#endif
#ifdef GL_TEXTURE_COMPRESSED
    texture_compressed = GL_TEXTURE_COMPRESSED,
#endif
#ifdef GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT
    texture_compressed_block_height = GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT,
#endif
#ifdef GL_TEXTURE_COMPRESSED_BLOCK_SIZE
    texture_compressed_block_size = GL_TEXTURE_COMPRESSED_BLOCK_SIZE,
#endif
#ifdef GL_TEXTURE_COMPRESSED_BLOCK_WIDTH
    texture_compressed_block_width = GL_TEXTURE_COMPRESSED_BLOCK_WIDTH,
#endif
#ifdef GL_TEXTURE_GATHER
    texture_gather = GL_TEXTURE_GATHER,
#endif
#ifdef GL_TEXTURE_GATHER_SHADOW
    texture_gather_shadow = GL_TEXTURE_GATHER_SHADOW,
#endif
#ifdef GL_TEXTURE_IMAGE_FORMAT
    texture_image_format = GL_TEXTURE_IMAGE_FORMAT,
#endif
#ifdef GL_TEXTURE_IMAGE_TYPE
    texture_image_type = GL_TEXTURE_IMAGE_TYPE,
#endif
#ifdef GL_TEXTURE_SHADOW
    texture_shadow = GL_TEXTURE_SHADOW,
#endif
#ifdef GL_TEXTURE_VIEW
    texture_view = GL_TEXTURE_VIEW,
#endif
#ifdef GL_VERTEX_TEXTURE
    vertex_texture = GL_VERTEX_TEXTURE,
#endif
#ifdef GL_VIEW_COMPATIBILITY_CLASS
    view_compatibility_class = GL_VIEW_COMPATIBILITY_CLASS,
#endif
}; // enum class internal_format_prop

} // namespace gl::group
