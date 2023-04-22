#if defined(GL_RGBA4) && defined(GL_RGBA) && defined(GL_UNSIGNED_SHORT_4_4_4_4)
{
    .type = format_t::rgba4,
    .vk_type = vk_format_t::r4g4b4a4_unorm_pack16,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 4, .g = 4, .b = 4, .a = 4, },
        .type            = texture_type_t::unsigned_short_4_4_4_4,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 2,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = 0x100,
        .web = 0x100,
    },
    .extensions = {
        .core = "GL_EXT_packed_pixels"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA4) && defined(GL_BGRA) && defined(GL_UNSIGNED_SHORT_4_4_4_4)
{
    .type = format_t::rgba4,
    .vk_type = vk_format_t::b4g4r4a4_unorm_pack16,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 4, .g = 4, .b = 4, .a = 4, },
        .type            = texture_type_t::unsigned_short_4_4_4_4,
        .format          = texture_layout_t::bgra,
        .pixel_size      = 2,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_bgra GL_EXT_packed_pixels"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB565) && defined(GL_RGB) && defined(GL_UNSIGNED_SHORT_5_6_5)
{
    .type = format_t::rgb565,
    .vk_type = vk_format_t::r5g6b5_unorm_pack16,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 5, .g = 6, .b = 5, },
        .type            = texture_type_t::unsigned_short_5_6_5,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 2,
        .component_count = 3,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = 0x100,
        .web = 0x100,
    },
    .extensions = {
        .core = {},
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB565) && defined(GL_RGB) && defined(GL_UNSIGNED_SHORT_5_6_5_REV)
{
    .type = format_t::rgb565,
    .vk_type = vk_format_t::b5g6r5_unorm_pack16,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 5, .g = 6, .b = 5, },
        .type            = texture_type_t::unsigned_short_5_6_5_rev,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 2,
        .component_count = 3,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB5_A1) && defined(GL_RGBA) && defined(GL_UNSIGNED_SHORT_5_5_5_1)
{
    .type = format_t::rgb5_a1,
    .vk_type = vk_format_t::r5g5b5a1_unorm_pack16,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 5, .g = 5, .b = 5, .a = 1},
        .type            = texture_type_t::unsigned_short_5_5_5_1,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 2,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = 0x100,
        .web = 0x100,
    },
    .extensions = {
        .core = "GL_EXT_packed_pixels"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB5_A1) && defined(GL_BGRA) && defined(GL_UNSIGNED_SHORT_5_5_5_1)
{
    .type = format_t::rgb5_a1,
    .vk_type = vk_format_t::b5g5r5a1_unorm_pack16,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 5, .g = 5, .b = 5, .a = 1},
        .type            = texture_type_t::unsigned_short_5_5_5_1,
        .format          = texture_layout_t::bgra,
        .pixel_size      = 2,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_bgra GL_EXT_packed_pixels"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB5_A1) && defined(GL_BGRA) && defined(GL_UNSIGNED_SHORT_1_5_5_5_REV)
{
    .type = format_t::rgb5_a1,
    .vk_type = vk_format_t::a1r5g5b5_unorm_pack16,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 5, .g = 5, .b = 5, .a = 1},
        .type            = texture_type_t::unsigned_short_1_5_5_5_rev,
        .format          = texture_layout_t::bgra,
        .pixel_size      = 2,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_R8) && defined(GL_RED) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::r8,
    .vk_type = vk_format_t::r8_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::red,
        .pixel_size      = 1,
        .component_count = 1,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg"sv,
        .es = "GL_EXT_texture_rg"sv,
        .web = {},
    },
},
#endif
#if defined(GL_R8_SNORM) && defined(GL_RED) && defined(GL_BYTE)
{
    .type = format_t::r8_snorm,
    .vk_type = vk_format_t::r8_snorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::red,
        .pixel_size      = 1,
        .component_count = 1,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x310,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_snorm"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_R8UI) && defined(GL_RED_INTEGER) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::r8ui,
    .vk_type = vk_format_t::r8_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::red_integer,
        .pixel_size      = 1,
        .component_count = 1,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_R8I) && defined(GL_RED_INTEGER) && defined(GL_BYTE)
{
    .type = format_t::r8i,
    .vk_type = vk_format_t::r8_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::red_integer,
        .pixel_size      = 1,
        .component_count = 1,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_SR8_EXT) && defined(GL_RED) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::sr8_ext,
    .vk_type = vk_format_t::r8_srgb,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {},
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::red,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_sRGB_R8"sv,
        .es = "GL_EXT_texture_sRGB_R8"sv,
        .web = {},
    },
},
#endif
#if defined(GL_RG8) && defined(GL_RG) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::rg8,
    .vk_type = vk_format_t::r8g8_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::rg,
        .pixel_size      = 2,
        .component_count = 2,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg"sv,
        .es = "GL_EXT_texture_rg"sv,
        .web = {},
    },
},
#endif
#if defined(GL_RG8_SNORM) && defined(GL_RG) && defined(GL_BYTE)
{
    .type = format_t::rg8_snorm,
    .vk_type = vk_format_t::r8g8_snorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::rg,
        .pixel_size      = 2,
        .component_count = 2,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x310,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_snorm"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RG8UI) && defined(GL_RG_INTEGER) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::rg8ui,
    .vk_type = vk_format_t::r8g8_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::rg_integer,
        .pixel_size      = 2,
        .component_count = 2,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RG8I) && defined(GL_RG_INTEGER) && defined(GL_BYTE)
{
    .type = format_t::rg8i,
    .vk_type = vk_format_t::r8g8_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::rg_integer,
        .pixel_size      = 2,
        .component_count = 2,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_SRG8_EXT) && defined(GL_RG) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::srg8_ext,
    .vk_type = vk_format_t::r8g8_srgb,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {},
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::rg,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_sRGB_RG8"sv,
        .es = "GL_EXT_texture_sRGB_RG8"sv,
        .web = {},
    },
},
#endif
#if defined(GL_RGB8) && defined(GL_RGB) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::rgb8,
    .vk_type = vk_format_t::r8g8b8_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x100,
        .es = 0x100,
        .web = 0x100,
    },
    .extensions = {
        .core = {},
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB8_SNORM) && defined(GL_RGB) && defined(GL_BYTE)
{
    .type = format_t::rgb8_snorm,
    .vk_type = vk_format_t::r8g8b8_snorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x310,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_snorm"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB8UI) && defined(GL_RGB_INTEGER) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::rgb8ui,
    .vk_type = vk_format_t::r8g8b8_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::rgb_integer,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB8I) && defined(GL_RGB_INTEGER) && defined(GL_BYTE)
{
    .type = format_t::rgb8i,
    .vk_type = vk_format_t::r8g8b8_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::rgb_integer,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_SRGB8) && defined(GL_RGB) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::srgb8,
    .vk_type = vk_format_t::r8g8b8_srgb,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = true,
        .floating_point  = false,
    },
    .version = {
        .core = 0x210,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_sRGB"sv,
        .es = "GL_EXT_sRGB"sv,
        .web = "EXT_sRGB"sv,
    },
},
#endif
#if defined(GL_RGB8) && defined(GL_BGR) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::rgb8,
    .vk_type = vk_format_t::b8g8r8_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::bgr,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_bgra"sv,
        .es = "GL_EXT_bgra"sv,
        .web = {},
    },
},
#endif
#if defined(GL_RGB8_SNORM) && defined(GL_BGR) && defined(GL_BYTE)
{
    .type = format_t::rgb8_snorm,
    .vk_type = vk_format_t::b8g8r8_snorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::bgr,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x310,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_snorm"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB8UI) && defined(GL_BGR_INTEGER) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::rgb8ui,
    .vk_type = vk_format_t::b8g8r8_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::bgr_integer,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB8I) && defined(GL_BGR_INTEGER) && defined(GL_BYTE)
{
    .type = format_t::rgb8i,
    .vk_type = vk_format_t::b8g8r8_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::bgr_integer,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_SRGB8) && defined(GL_BGR) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::srgb8,
    .vk_type = vk_format_t::b8g8r8_srgb,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::bgr,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = true,
        .floating_point  = false,
    },
    .version = {
        .core = 0x210,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_sRGB"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA8) && defined(GL_RGBA) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::rgba8,
    .vk_type = vk_format_t::r8g8b8a8_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, .a = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x100,
        .es = 0x100,
        .web = 0x100,
    },
    .extensions = {
        .core = {},
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA8_SNORM) && defined(GL_RGBA) && defined(GL_BYTE)
{
    .type = format_t::rgba8_snorm,
    .vk_type = vk_format_t::r8g8b8a8_snorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, .a = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x310,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_snorm"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA8UI) && defined(GL_RGBA_INTEGER) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::rgba8ui,
    .vk_type = vk_format_t::r8g8b8a8_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, .a = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::rgba_integer,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA8I) && defined(GL_RGBA_INTEGER) && defined(GL_BYTE)
{
    .type = format_t::rgba8i,
    .vk_type = vk_format_t::r8g8b8a8_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, .a = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::rgba_integer,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_SRGB8_ALPHA8) && defined(GL_RGBA) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::srgb8_alpha8,
    .vk_type = vk_format_t::r8g8b8a8_srgb,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = true,
        .floating_point  = false,
    },
    .version = {
        .core = 0x210,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_sRGB"sv,
        .es = "GL_EXT_sRGB"sv,
        .web = "EXT_sRGB"sv,
    },
},
#endif
#if defined(GL_RGBA8) && defined(GL_BGRA) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::rgba8,
    .vk_type = vk_format_t::b8g8r8a8_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, .a = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::bgra,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_bgra"sv,
        .es = "GL_APPLE_texture_format_BGRA8888"sv,
        .web = {},
    },
},
#endif
#if defined(GL_RGBA8_SNORM) && defined(GL_BGRA) && defined(GL_BYTE)
{
    .type = format_t::rgba8_snorm,
    .vk_type = vk_format_t::b8g8r8a8_snorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, .a = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::bgra,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x310,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_snorm"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA8UI) && defined(GL_BGRA_INTEGER) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::rgba8ui,
    .vk_type = vk_format_t::b8g8r8a8_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, .a = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::bgra_integer,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA8I) && defined(GL_BGRA_INTEGER) && defined(GL_BYTE)
{
    .type = format_t::rgba8i,
    .vk_type = vk_format_t::b8g8r8a8_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, .a = 8, },
        .type            = texture_type_t::byte_,
        .format          = texture_layout_t::bgra_integer,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_SRGB8_ALPHA8) && defined(GL_BGRA) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::srgb8_alpha8,
    .vk_type = vk_format_t::b8g8r8a8_srgb,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 8, .g = 8, .b = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::bgra,
        .pixel_size      = 3,
        .component_count = 3,
        .component_size  = 1,
        .srgb            = true,
        .floating_point  = false,
    },
    .version = {
        .core = 0x210,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_sRGB"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB10_A2) && defined(GL_BGRA) && defined(GL_UNSIGNED_INT_2_10_10_10_REV)
{
    .type = format_t::rgb10_a2,
    .vk_type = vk_format_t::a2r10g10b10_unorm_pack32,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 10, .g = 10, .b = 10, },
        .type            = texture_type_t::unsigned_int_2_10_10_10_rev,
        .format          = texture_layout_t::bgra,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB10_A2UI) && defined(GL_BGRA_INTEGER) && defined(GL_UNSIGNED_INT_2_10_10_10_REV)
{
    .type = format_t::rgb10_a2ui,
    .vk_type = vk_format_t::a2r10g10b10_uint_pack32,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 10, .g = 10, .b = 10, },
        .type            = texture_type_t::unsigned_int_2_10_10_10_rev,
        .format          = texture_layout_t::bgra_integer,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x330,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_rgb10_a2ui"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB10_A2) && defined(GL_RGBA) && defined(GL_UNSIGNED_INT_2_10_10_10_REV)
{
    .type = format_t::rgb10_a2,
    .vk_type = vk_format_t::a2b10g10r10_unorm_pack32,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 10, .g = 10, .b = 10, },
        .type            = texture_type_t::unsigned_int_2_10_10_10_rev,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = {},
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB10_A2UI) && defined(GL_RGBA_INTEGER) && defined(GL_UNSIGNED_INT_2_10_10_10_REV)
{
    .type = format_t::rgb10_a2ui,
    .vk_type = vk_format_t::a2b10g10r10_uint_pack32,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 10, .g = 10, .b = 10, },
        .type            = texture_type_t::unsigned_int_2_10_10_10_rev,
        .format          = texture_layout_t::rgba_integer,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x330,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rgb10_a2ui"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_R16) && defined(GL_RED) && defined(GL_UNSIGNED_SHORT)
{
    .type = format_t::r16,
    .vk_type = vk_format_t::r16_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, },
        .type            = texture_type_t::unsigned_short,
        .format          = texture_layout_t::red,
        .pixel_size      = 2,
        .component_count = 1,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg"sv,
        .es = "GL_EXT_texture_norm16"sv,
        .web = "EXT_texture_norm16"sv,
    },
},
#endif
#if defined(GL_R16_SNORM) && defined(GL_RED) && defined(GL_SHORT)
{
    .type = format_t::r16_snorm,
    .vk_type = vk_format_t::r16_snorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, },
        .type            = texture_type_t::short_,
        .format          = texture_layout_t::red,
        .pixel_size      = 2,
        .component_count = 1,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x310,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_snorm"sv,
        .es = "GL_EXT_texture_norm16"sv,
        .web = "EXT_texture_norm16"sv,
    },
},
#endif
#if defined(GL_R16UI) && defined(GL_RED_INTEGER) && defined(GL_UNSIGNED_SHORT)
{
    .type = format_t::r16ui,
    .vk_type = vk_format_t::r16_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, },
        .type            = texture_type_t::unsigned_short,
        .format          = texture_layout_t::red_integer,
        .pixel_size      = 2,
        .component_count = 1,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_R16I) && defined(GL_RED_INTEGER) && defined(GL_SHORT)
{
    .type = format_t::r16i,
    .vk_type = vk_format_t::r16_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, },
        .type            = texture_type_t::short_,
        .format          = texture_layout_t::red_integer,
        .pixel_size      = 2,
        .component_count = 1,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_R16F) && defined(GL_RED) && defined(GL_HALF_FLOAT)
{
    .type = format_t::r16f,
    .vk_type = vk_format_t::r16_sfloat,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, },
        .type            = texture_type_t::half_float,
        .format          = texture_layout_t::red,
        .pixel_size      = 2,
        .component_count = 1,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = true,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_half_float_pixel GL_ARB_texture_float GL_ARB_texture_rg"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RG16) && defined(GL_RG) && defined(GL_UNSIGNED_SHORT)
{
    .type = format_t::rg16,
    .vk_type = vk_format_t::r16g16_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, },
        .type            = texture_type_t::unsigned_short,
        .format          = texture_layout_t::rg,
        .pixel_size      = 4,
        .component_count = 2,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg"sv,
        .es = "GL_EXT_texture_norm16"sv,
        .web = "EXT_texture_norm16"sv,
    },
},
#endif
#if defined(GL_RG16_SNORM) && defined(GL_RG) && defined(GL_SHORT)
{
    .type = format_t::rg16_snorm,
    .vk_type = vk_format_t::r16g16_snorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, },
        .type            = texture_type_t::short_,
        .format          = texture_layout_t::rg,
        .pixel_size      = 4,
        .component_count = 2,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x310,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_snorm"sv,
        .es = "GL_EXT_texture_norm16"sv,
        .web = "EXT_texture_norm16"sv,
    },
},
#endif
#if defined(GL_RG16UI) && defined(GL_RG_INTEGER) && defined(GL_UNSIGNED_SHORT)
{
    .type = format_t::rg16ui,
    .vk_type = vk_format_t::r16g16_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, },
        .type            = texture_type_t::unsigned_short,
        .format          = texture_layout_t::rg_integer,
        .pixel_size      = 4,
        .component_count = 2,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RG16I) && defined(GL_RG_INTEGER) && defined(GL_SHORT)
{
    .type = format_t::rg16i,
    .vk_type = vk_format_t::r16g16_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, },
        .type            = texture_type_t::short_,
        .format          = texture_layout_t::rg_integer,
        .pixel_size      = 4,
        .component_count = 2,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RG16F) && defined(GL_RG) && defined(GL_HALF_FLOAT)
{
    .type = format_t::rg16f,
    .vk_type = vk_format_t::r16g16_sfloat,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, },
        .type            = texture_type_t::half_float,
        .format          = texture_layout_t::rg,
        .pixel_size      = 4,
        .component_count = 2,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = true,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_half_float_pixel GL_ARB_texture_float GL_ARB_texture_rg"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB16) && defined(GL_RGB) && defined(GL_UNSIGNED_SHORT)
{
    .type = format_t::rgb16,
    .vk_type = vk_format_t::r16g16b16_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, .b = 16, },
        .type            = texture_type_t::unsigned_short,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 6,
        .component_count = 3,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x100,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_EXT_texture_norm16"sv,
        .web = "EXT_texture_norm16"sv,
    },
},
#endif
#if defined(GL_RGB16_SNORM) && defined(GL_RGB) && defined(GL_SHORT)
{
    .type = format_t::rgb16_snorm,
    .vk_type = vk_format_t::r16g16b16_snorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, .b = 16, },
        .type            = texture_type_t::short_,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 6,
        .component_count = 3,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x310,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_snorm"sv,
        .es = "GL_EXT_texture_norm16"sv,
        .web = "EXT_texture_norm16"sv,
    },
},
#endif
#if defined(GL_RGB16UI) && defined(GL_RGB_INTEGER) && defined(GL_UNSIGNED_SHORT)
{
    .type = format_t::rgb16ui,
    .vk_type = vk_format_t::r16g16b16_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, .b = 16, },
        .type            = texture_type_t::unsigned_short,
        .format          = texture_layout_t::rgb_integer,
        .pixel_size      = 6,
        .component_count = 3,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB16I) && defined(GL_RGB_INTEGER) && defined(GL_SHORT)
{
    .type = format_t::rgb16i,
    .vk_type = vk_format_t::r16g16b16_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, .b = 16, },
        .type            = texture_type_t::short_,
        .format          = texture_layout_t::rgb_integer,
        .pixel_size      = 6,
        .component_count = 3,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB16F) && defined(GL_RGB) && defined(GL_HALF_FLOAT)
{
    .type = format_t::rgb16f,
    .vk_type = vk_format_t::r16g16b16_sfloat,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, .b = 16, },
        .type            = texture_type_t::half_float,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 6,
        .component_count = 3,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = true,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_APPLE_float_pixels"sv,
        .es = "GL_OES_texture_half_float"sv,
        .web = "OES_texture_half_float"sv,
    },
},
#endif
#if defined(GL_RGBA16) && defined(GL_RGBA) && defined(GL_UNSIGNED_SHORT)
{
    .type = format_t::rgba16,
    .vk_type = vk_format_t::r16g16b16a16_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, .b = 16, .a = 16, },
        .type            = texture_type_t::unsigned_short,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 8,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x100,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_EXT_texture_norm16"sv,
        .web = "EXT_texture_norm16"sv,
    },
},
#endif
#if defined(GL_RGBA16_SNORM) && defined(GL_RGBA) && defined(GL_SHORT)
{
    .type = format_t::rgba16_snorm,
    .vk_type = vk_format_t::r16g16b16a16_snorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, .b = 16, .a = 16, },
        .type            = texture_type_t::short_,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 8,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x310,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_snorm"sv,
        .es = "GL_EXT_texture_norm16"sv,
        .web = "EXT_texture_norm16"sv,
    },
},
#endif
#if defined(GL_RGBA16UI) && defined(GL_RGBA_INTEGER) && defined(GL_UNSIGNED_SHORT)
{
    .type = format_t::rgba16ui,
    .vk_type = vk_format_t::r16g16b16a16_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, .b = 16, .a = 16, },
        .type            = texture_type_t::unsigned_short,
        .format          = texture_layout_t::rgba_integer,
        .pixel_size      = 8,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA16I) && defined(GL_RGBA_INTEGER) && defined(GL_SHORT)
{
    .type = format_t::rgba16i,
    .vk_type = vk_format_t::r16g16b16a16_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, .b = 16, .a = 16, },
        .type            = texture_type_t::short_,
        .format          = texture_layout_t::rgba_integer,
        .pixel_size      = 8,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA16F) && defined(GL_RGBA) && defined(GL_HALF_FLOAT)
{
    .type = format_t::rgba16f,
    .vk_type = vk_format_t::r16g16b16a16_sfloat,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 16, .g = 16, .b = 16, .a = 16, },
        .type            = texture_type_t::half_float,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 8,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = true,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_APPLE_float_pixels"sv,
        .es = "GL_OES_texture_half_float"sv,
        .web = "OES_texture_half_float"sv,
    },
},
#endif
#if defined(GL_R32UI) && defined(GL_RED_INTEGER) && defined(GL_UNSIGNED_INT)
{
    .type = format_t::r32ui,
    .vk_type = vk_format_t::r32_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, },
        .type            = texture_type_t::unsigned_int,
        .format          = texture_layout_t::red_integer,
        .pixel_size      = 4,
        .component_count = 1,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_R32I) && defined(GL_RED_INTEGER) && defined(GL_INT)
{
    .type = format_t::r32i,
    .vk_type = vk_format_t::r32_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, },
        .type            = texture_type_t::int_,
        .format          = texture_layout_t::red_integer,
        .pixel_size      = 4,
        .component_count = 1,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_R32F) && defined(GL_RED) && defined(GL_FLOAT)
{
    .type = format_t::r32f,
    .vk_type = vk_format_t::r32_sfloat,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, },
        .type            = texture_type_t::float_,
        .format          = texture_layout_t::red,
        .pixel_size      = 4,
        .component_count = 1,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = true,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_float GL_ARB_texture_rg"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RG32UI) && defined(GL_RG_INTEGER) && defined(GL_UNSIGNED_INT)
{
    .type = format_t::rg32ui,
    .vk_type = vk_format_t::r32g32_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, .g = 32, },
        .type            = texture_type_t::unsigned_int,
        .format          = texture_layout_t::rg_integer,
        .pixel_size      = 8,
        .component_count = 2,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RG32I) && defined(GL_RG_INTEGER) && defined(GL_INT)
{
    .type = format_t::rg32i,
    .vk_type = vk_format_t::r32g32_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, .g = 32, },
        .type            = texture_type_t::int_,
        .format          = texture_layout_t::rg_integer,
        .pixel_size      = 8,
        .component_count = 2,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_rg GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RG32F) && defined(GL_RG) && defined(GL_FLOAT)
{
    .type = format_t::rg32f,
    .vk_type = vk_format_t::r32g32_sfloat,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, .g = 32, },
        .type            = texture_type_t::float_,
        .format          = texture_layout_t::rg,
        .pixel_size      = 8,
        .component_count = 2,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = true,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_texture_float GL_ARB_texture_rg"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB32UI) && defined(GL_RGB_INTEGER) && defined(GL_UNSIGNED_INT)
{
    .type = format_t::rgb32ui,
    .vk_type = vk_format_t::r32g32b32_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, .g = 32, .b = 32, },
        .type            = texture_type_t::unsigned_int,
        .format          = texture_layout_t::rgb_integer,
        .pixel_size      = 12,
        .component_count = 3,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB32I) && defined(GL_RGB_INTEGER) && defined(GL_INT)
{
    .type = format_t::rgb32i,
    .vk_type = vk_format_t::r32g32b32_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, .g = 32, .b = 32, },
        .type            = texture_type_t::int_,
        .format          = texture_layout_t::rgb_integer,
        .pixel_size      = 12,
        .component_count = 3,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGB32F) && defined(GL_RGB) && defined(GL_FLOAT)
{
    .type = format_t::rgb32f,
    .vk_type = vk_format_t::r32g32b32_sfloat,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, .g = 32, .b = 32, },
        .type            = texture_type_t::float_,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 12,
        .component_count = 3,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = true,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_APPLE_float_pixels"sv,
        .es = "GL_OES_texture_float"sv,
        .web = "OES_texture_float"sv,
    },
},
#endif
#if defined(GL_RGBA32UI) && defined(GL_RGBA_INTEGER) && defined(GL_UNSIGNED_INT)
{
    .type = format_t::rgba32ui,
    .vk_type = vk_format_t::r32g32b32a32_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, .g = 32, .b = 32, .a = 32, },
        .type            = texture_type_t::unsigned_int,
        .format          = texture_layout_t::rgba_integer,
        .pixel_size      = 16,
        .component_count = 4,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA32I) && defined(GL_RGBA_INTEGER) && defined(GL_INT)
{
    .type = format_t::rgba32i,
    .vk_type = vk_format_t::r32g32b32a32_sint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, .g = 32, .b = 32, .a = 32, },
        .type            = texture_type_t::int_,
        .format          = texture_layout_t::rgba_integer,
        .pixel_size      = 16,
        .component_count = 4,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_integer"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA32F) && defined(GL_RGBA) && defined(GL_FLOAT)
{
    .type = format_t::rgba32f,
    .vk_type = vk_format_t::r32g32b32a32_sfloat,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 32, .g = 32, .b = 32, .a = 32, },
        .type            = texture_type_t::float_,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 16,
        .component_count = 4,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = true,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_APPLE_float_pixels"sv,
        .es = "GL_OES_texture_float"sv,
        .web = "OES_texture_float"sv,
    },
},
#endif
#if defined(GL_R11F_G11F_B10F) && defined(GL_RGB) && defined(GL_UNSIGNED_INT_10F_11F_11F_REV)
{
    .type = format_t::r11f_g11f_b10f,
    .vk_type = vk_format_t::b10g11r11_ufloat_pack32,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 11, },
        .type            = texture_type_t::unsigned_int_10f_11f_11f_rev,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 4,
        .component_count = 3,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_packed_float"sv,
        .es = "GL_APPLE_texture_packed_float"sv,
        .web = {},
    },
},
#endif
#if defined(GL_RGB9_E5) && defined(GL_RGB) && defined(GL_UNSIGNED_INT_5_9_9_9_REV)
{
    .type = format_t::rgb9_e5,
    .vk_type = vk_format_t::e5b9g9r9_ufloat_pack32,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 9, .g = 9, .b = 9, },
        .type            = texture_type_t::unsigned_int_5_9_9_9_rev,
        .format          = texture_layout_t::rgb,
        .pixel_size      = 4,
        .component_count = 4,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_EXT_texture_shared_exponent"sv,
        .es = "GL_APPLE_texture_packed_float"sv,
        .web = {},
    },
},
#endif
#if defined(GL_DEPTH_COMPONENT16) && defined(GL_DEPTH_COMPONENT) && defined(GL_UNSIGNED_SHORT)
{
    .type = format_t::depth_component16,
    .vk_type = vk_format_t::d16_unorm,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.depth = 16, },
        .type            = texture_type_t::unsigned_short,
        .format          = texture_layout_t::depth_component,
        .pixel_size      = 2,
        .component_count = 1,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x140,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_depth_texture"sv,
        .es = "GL_OES_depth_texture"sv,
        .web = "WEBGL_depth_texture"sv,
    },
},
#endif
#if defined(GL_DEPTH_COMPONENT32F) && defined(GL_DEPTH_COMPONENT) && defined(GL_FLOAT)
{
    .type = format_t::depth_component32f,
    .vk_type = vk_format_t::d32_sfloat,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.depth = 32, },
        .type            = texture_type_t::float_,
        .format          = texture_layout_t::depth_component,
        .pixel_size      = 4,
        .component_count = 1,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = true,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_depth_buffer_float"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_STENCIL_INDEX8) && defined(GL_STENCIL_INDEX) && defined(GL_UNSIGNED_BYTE)
{
    .type = format_t::stencil_index8,
    .vk_type = vk_format_t::s8_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.stencil = 8, },
        .type            = texture_type_t::unsigned_byte,
        .format          = texture_layout_t::stencil_index,
        .pixel_size      = 1,
        .component_count = 1,
        .component_size  = 1,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x440,
        .es = 0x320,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_stencil8"sv,
        .es = "GL_OES_texture_stencil8"sv,
        .web = {},
    },
},
#endif
#if defined(GL_DEPTH24_STENCIL8) && defined(GL_DEPTH_STENCIL) && defined(GL_UNSIGNED_INT_24_8)
{
    .type = format_t::depth24_stencil8,
    .vk_type = vk_format_t::d24_unorm_s8_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.depth = 24, },
        .type            = texture_type_t::unsigned_int_24_8,
        .format          = texture_layout_t::depth_stencil,
        .pixel_size      = 4,
        .component_count = 2,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_framebuffer_object"sv,
        .es = "GL_OES_packed_depth_stencil"sv,
        .web = {},
    },
},
#endif
#if defined(GL_DEPTH32F_STENCIL8) && defined(GL_DEPTH_STENCIL) && defined(GL_FLOAT_32_UNSIGNED_INT_24_8_REV)
{
    .type = format_t::depth32f_stencil8,
    .vk_type = vk_format_t::d32_sfloat_s8_uint,
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.depth = 32, },
        .type            = texture_type_t::float_32_unsigned_int_24_8_rev,
        .format          = texture_layout_t::depth_stencil,
        .pixel_size      = 4,
        .component_count = 2,
        .component_size  = 4,
        .srgb            = false,
        .floating_point  = true,
    },
    .version = {
        .core = 0x300,
        .es = 0x300,
        .web = 0x200,
    },
    .extensions = {
        .core = "GL_ARB_depth_buffer_float"sv,
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGB_S3TC_DXT1_EXT)
{
    .type = format_t::compressed_rgb_s3tc_dxt1_ext,
    .vk_type = vk_format_t::bc1_rgb_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_compression_s3tc"sv,
        .es = "GL_ANGLE_texture_compression_dxt1"sv,
        .web = "WEBGL_compressed_texture_s3tc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB_S3TC_DXT1_EXT)
{
    .type = format_t::compressed_srgb_s3tc_dxt1_ext,
    .vk_type = vk_format_t::bc1_rgb_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 4,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_compression_s3tc GL_EXT_texture_sRGB"sv,
        .es = "GL_EXT_texture_compression_s3tc_srgb"sv,
        .web = "WEBGL_compressed_texture_s3tc_srgb"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
{
    .type = format_t::compressed_rgba_s3tc_dxt1_ext,
    .vk_type = vk_format_t::bc1_rgba_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_compression_s3tc"sv,
        .es = "GL_ANGLE_texture_compression_dxt1"sv,
        .web = "WEBGL_compressed_texture_s3tc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT)
{
    .type = format_t::compressed_srgb_alpha_s3tc_dxt1_ext,
    .vk_type = vk_format_t::bc1_rgba_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 4,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_compression_s3tc GL_EXT_texture_sRGB"sv,
        .es = "GL_EXT_texture_compression_s3tc_srgb"sv,
        .web = "WEBGL_compressed_texture_s3tc_srgb"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
{
    .type = format_t::compressed_rgba_s3tc_dxt3_ext,
    .vk_type = vk_format_t::bc2_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_compression_s3tc"sv,
        .es = "GL_ANGLE_texture_compression_dxt3"sv,
        .web = "WEBGL_compressed_texture_s3tc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT)
{
    .type = format_t::compressed_srgb_alpha_s3tc_dxt3_ext,
    .vk_type = vk_format_t::bc2_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_compression_s3tc GL_EXT_texture_sRGB"sv,
        .es = "GL_EXT_texture_compression_s3tc_srgb"sv,
        .web = "WEBGL_compressed_texture_s3tc_srgb"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
{
    .type = format_t::compressed_rgba_s3tc_dxt5_ext,
    .vk_type = vk_format_t::bc3_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_compression_s3tc"sv,
        .es = "GL_ANGLE_texture_compression_dxt3"sv,
        .web = "WEBGL_compressed_texture_s3tc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT)
{
    .type = format_t::compressed_srgb_alpha_s3tc_dxt5_ext,
    .vk_type = vk_format_t::bc3_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_EXT_texture_compression_s3tc GL_EXT_texture_sRGB"sv,
        .es = "GL_EXT_texture_compression_s3tc_srgb"sv,
        .web = "WEBGL_compressed_texture_s3tc_srgb"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RED_RGTC1_EXT)
{
    .type = format_t::compressed_red_rgtc1_ext,
    .vk_type = vk_format_t::bc4_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x300,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_compression_rgtc"sv,
        .es = "GL_EXT_texture_compression_rgtc"sv,
        .web = "EXT_texture_compression_rgtc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SIGNED_RED_RGTC1_EXT)
{
    .type = format_t::compressed_signed_red_rgtc1_ext,
    .vk_type = vk_format_t::bc4_snorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x300,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_compression_rgtc"sv,
        .es = "GL_EXT_texture_compression_rgtc"sv,
        .web = "EXT_texture_compression_rgtc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RED_GREEN_RGTC2_EXT)
{
    .type = format_t::compressed_red_green_rgtc2_ext,
    .vk_type = vk_format_t::bc5_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x300,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_compression_rgtc"sv,
        .es = "GL_EXT_texture_compression_rgtc"sv,
        .web = "EXT_texture_compression_rgtc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT)
{
    .type = format_t::compressed_signed_red_green_rgtc2_ext,
    .vk_type = vk_format_t::bc5_snorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x300,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_compression_rgtc"sv,
        .es = "GL_EXT_texture_compression_rgtc"sv,
        .web = "EXT_texture_compression_rgtc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB)
{
    .type = format_t::compressed_rgb_bptc_unsigned_float_arb,
    .vk_type = vk_format_t::bc6h_ufloat_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x420,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_compression_bptc"sv,
        .es = "GL_EXT_texture_compression_bptc"sv,
        .web = "EXT_texture_compression_bptc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB)
{
    .type = format_t::compressed_rgb_bptc_signed_float_arb,
    .vk_type = vk_format_t::bc6h_sfloat_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x420,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_compression_bptc"sv,
        .es = "GL_EXT_texture_compression_bptc"sv,
        .web = "EXT_texture_compression_bptc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_BPTC_UNORM_ARB)
{
    .type = format_t::compressed_rgba_bptc_unorm_arb,
    .vk_type = vk_format_t::bc7_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x420,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_compression_bptc"sv,
        .es = "GL_EXT_texture_compression_bptc"sv,
        .web = "EXT_texture_compression_bptc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB)
{
    .type = format_t::compressed_srgb_alpha_bptc_unorm_arb,
    .vk_type = vk_format_t::bc7_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x420,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_texture_compression_bptc"sv,
        .es = "GL_EXT_texture_compression_bptc"sv,
        .web = "EXT_texture_compression_bptc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGB8_ETC2)
{
    .type = format_t::compressed_rgb8_etc2,
    .vk_type = vk_format_t::etc2_r8g8b8_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 3,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x430,
        .es = 0x300,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_ES3_compatibility"sv,
        .es = {},
        .web = "WEBGL_compressed_texture_etc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ETC2)
{
    .type = format_t::compressed_srgb8_etc2,
    .vk_type = vk_format_t::etc2_r8g8b8_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x430,
        .es = 0x300,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_ES3_compatibility"sv,
        .es = {},
        .web = "WEBGL_compressed_texture_etc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2)
{
    .type = format_t::compressed_rgb8_punchthrough_alpha1_etc2,
    .vk_type = vk_format_t::etc2_r8g8b8a1_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 3,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x430,
        .es = 0x300,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_ES3_compatibility"sv,
        .es = {},
        .web = "WEBGL_compressed_texture_etc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2)
{
    .type = format_t::compressed_srgb8_punchthrough_alpha1_etc2,
    .vk_type = vk_format_t::etc2_r8g8b8a1_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x430,
        .es = 0x300,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_ES3_compatibility"sv,
        .es = {},
        .web = "WEBGL_compressed_texture_etc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA8_ETC2_EAC)
{
    .type = format_t::compressed_rgba8_etc2_eac,
    .vk_type = vk_format_t::etc2_r8g8b8a8_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x430,
        .es = 0x300,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_ES3_compatibility"sv,
        .es = {},
        .web = "WEBGL_compressed_texture_etc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC)
{
    .type = format_t::compressed_srgb8_alpha8_etc2_eac,
    .vk_type = vk_format_t::etc2_r8g8b8a8_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x430,
        .es = 0x300,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_ES3_compatibility"sv,
        .es = {},
        .web = "WEBGL_compressed_texture_etc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_R11_EAC)
{
    .type = format_t::compressed_r11_eac,
    .vk_type = vk_format_t::eac_r11_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::red,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 1,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x430,
        .es = 0x300,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_ES3_compatibility"sv,
        .es = {},
        .web = "WEBGL_compressed_texture_etc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SIGNED_R11_EAC)
{
    .type = format_t::compressed_signed_r11_eac,
    .vk_type = vk_format_t::eac_r11_snorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::red,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 1,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x430,
        .es = 0x300,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_ES3_compatibility"sv,
        .es = {},
        .web = "WEBGL_compressed_texture_etc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RG11_EAC)
{
    .type = format_t::compressed_rg11_eac,
    .vk_type = vk_format_t::eac_r11g11_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rg,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 2,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x430,
        .es = 0x300,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_ES3_compatibility"sv,
        .es = {},
        .web = "WEBGL_compressed_texture_etc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SIGNED_RG11_EAC)
{
    .type = format_t::compressed_signed_rg11_eac,
    .vk_type = vk_format_t::eac_r11g11_snorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rg,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 2,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = 0x430,
        .es = 0x300,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_ARB_ES3_compatibility"sv,
        .es = {},
        .web = "WEBGL_compressed_texture_etc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_4x4_KHR)
{
    .type = format_t::compressed_rgba_astc_4x4_khr,
    .vk_type = vk_format_t::astc_4x4_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_4x4_khr,
    .vk_type = vk_format_t::astc_4x4_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_4x4_KHR)
{
    .type = format_t::compressed_rgba_astc_4x4_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_5x4_KHR)
{
    .type = format_t::compressed_rgba_astc_5x4_khr,
    .vk_type = vk_format_t::astc_5x4_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 5,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_5x4_khr,
    .vk_type = vk_format_t::astc_5x4_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 5,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_5x4_KHR)
{
    .type = format_t::compressed_rgba_astc_5x4_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 5,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_5x5_KHR)
{
    .type = format_t::compressed_rgba_astc_5x5_khr,
    .vk_type = vk_format_t::astc_5x5_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 5,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_5x5_khr,
    .vk_type = vk_format_t::astc_5x5_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 5,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_5x5_KHR)
{
    .type = format_t::compressed_rgba_astc_5x5_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 5,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_6x5_KHR)
{
    .type = format_t::compressed_rgba_astc_6x5_khr,
    .vk_type = vk_format_t::astc_6x5_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 6,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_6x5_khr,
    .vk_type = vk_format_t::astc_6x5_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 6,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_6x5_KHR)
{
    .type = format_t::compressed_rgba_astc_6x5_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 6,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_6x6_KHR)
{
    .type = format_t::compressed_rgba_astc_6x6_khr,
    .vk_type = vk_format_t::astc_6x6_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 6,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_6x6_khr,
    .vk_type = vk_format_t::astc_6x6_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 6,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_6x6_KHR)
{
    .type = format_t::compressed_rgba_astc_6x6_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 6,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_8x5_KHR)
{
    .type = format_t::compressed_rgba_astc_8x5_khr,
    .vk_type = vk_format_t::astc_8x5_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 8,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_8x5_khr,
    .vk_type = vk_format_t::astc_8x5_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 8,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_8x5_KHR)
{
    .type = format_t::compressed_rgba_astc_8x5_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 8,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_8x6_KHR)
{
    .type = format_t::compressed_rgba_astc_8x6_khr,
    .vk_type = vk_format_t::astc_8x6_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 8,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_8x6_khr,
    .vk_type = vk_format_t::astc_8x6_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 8,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_8x6_KHR)
{
    .type = format_t::compressed_rgba_astc_8x6_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 8,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_8x8_KHR)
{
    .type = format_t::compressed_rgba_astc_8x8_khr,
    .vk_type = vk_format_t::astc_8x8_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 8,
        .block_height    = 8,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_8x8_khr,
    .vk_type = vk_format_t::astc_8x8_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 8,
        .block_height    = 8,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_8x8_KHR)
{
    .type = format_t::compressed_rgba_astc_8x8_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 8,
        .block_height    = 8,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_10x5_KHR)
{
    .type = format_t::compressed_rgba_astc_10x5_khr,
    .vk_type = vk_format_t::astc_10x5_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 10,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_10x5_khr,
    .vk_type = vk_format_t::astc_10x5_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 10,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_10x5_KHR)
{
    .type = format_t::compressed_rgba_astc_10x5_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 10,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_10x6_KHR)
{
    .type = format_t::compressed_rgba_astc_10x6_khr,
    .vk_type = vk_format_t::astc_10x6_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 10,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_10x6_khr,
    .vk_type = vk_format_t::astc_10x6_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 10,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_10x6_KHR)
{
    .type = format_t::compressed_rgba_astc_10x6_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 10,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_10x8_KHR)
{
    .type = format_t::compressed_rgba_astc_10x8_khr,
    .vk_type = vk_format_t::astc_10x8_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 10,
        .block_height    = 8,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_10x8_khr,
    .vk_type = vk_format_t::astc_10x8_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 10,
        .block_height    = 8,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_10x8_KHR)
{
    .type = format_t::compressed_rgba_astc_10x8_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 10,
        .block_height    = 8,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_10x10_KHR)
{
    .type = format_t::compressed_rgba_astc_10x10_khr,
    .vk_type = vk_format_t::astc_10x10_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 10,
        .block_height    = 10,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_10x10_khr,
    .vk_type = vk_format_t::astc_10x10_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 10,
        .block_height    = 10,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_10x10_KHR)
{
    .type = format_t::compressed_rgba_astc_10x10_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 10,
        .block_height    = 10,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_12x10_KHR)
{
    .type = format_t::compressed_rgba_astc_12x10_khr,
    .vk_type = vk_format_t::astc_12x10_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 12,
        .block_height    = 10,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_12x10_khr,
    .vk_type = vk_format_t::astc_12x10_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 12,
        .block_height    = 10,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_12x10_KHR)
{
    .type = format_t::compressed_rgba_astc_12x10_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 12,
        .block_height    = 10,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_12x12_KHR)
{
    .type = format_t::compressed_rgba_astc_12x12_khr,
    .vk_type = vk_format_t::astc_12x12_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 12,
        .block_height    = 12,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR)
{
    .type = format_t::compressed_srgb8_alpha8_astc_12x12_khr,
    .vk_type = vk_format_t::astc_12x12_srgb_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 12,
        .block_height    = 12,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_ldr"sv,
        .es = "GL_KHR_texture_compression_astc_ldr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_12x12_KHR)
{
    .type = format_t::compressed_rgba_astc_12x12_khr,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 12,
        .block_height    = 12,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = "GL_KHR_texture_compression_astc_hdr"sv,
        .es = "GL_KHR_texture_compression_astc_hdr"sv,
        .web = "WEBGL_compressed_texture_astc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_3x3x3_OES)
{
    .type = format_t::compressed_rgba_astc_3x3x3_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 3,
        .block_height    = 3,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES)
{
    .type = format_t::compressed_srgb8_alpha8_astc_3x3x3_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 3,
        .block_height    = 3,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_3x3x3_OES)
{
    .type = format_t::compressed_rgba_astc_3x3x3_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 3,
        .block_height    = 3,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_4x3x3_OES)
{
    .type = format_t::compressed_rgba_astc_4x3x3_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 3,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES)
{
    .type = format_t::compressed_srgb8_alpha8_astc_4x3x3_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 3,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_4x3x3_OES)
{
    .type = format_t::compressed_rgba_astc_4x3x3_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 3,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_4x4x3_OES)
{
    .type = format_t::compressed_rgba_astc_4x4x3_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES)
{
    .type = format_t::compressed_srgb8_alpha8_astc_4x4x3_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_4x4x3_OES)
{
    .type = format_t::compressed_rgba_astc_4x4x3_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_4x4x4_OES)
{
    .type = format_t::compressed_rgba_astc_4x4x4_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES)
{
    .type = format_t::compressed_srgb8_alpha8_astc_4x4x4_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_4x4x4_OES)
{
    .type = format_t::compressed_rgba_astc_4x4x4_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_5x4x4_OES)
{
    .type = format_t::compressed_rgba_astc_5x4x4_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 5,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES)
{
    .type = format_t::compressed_srgb8_alpha8_astc_5x4x4_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 5,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_5x4x4_OES)
{
    .type = format_t::compressed_rgba_astc_5x4x4_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 5,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_5x5x4_OES)
{
    .type = format_t::compressed_rgba_astc_5x5x4_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 5,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES)
{
    .type = format_t::compressed_srgb8_alpha8_astc_5x5x4_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 5,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_5x5x4_OES)
{
    .type = format_t::compressed_rgba_astc_5x5x4_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 5,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_5x5x5_OES)
{
    .type = format_t::compressed_rgba_astc_5x5x5_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 5,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES)
{
    .type = format_t::compressed_srgb8_alpha8_astc_5x5x5_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 5,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_5x5x5_OES)
{
    .type = format_t::compressed_rgba_astc_5x5x5_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 5,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_6x5x5_OES)
{
    .type = format_t::compressed_rgba_astc_6x5x5_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 6,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES)
{
    .type = format_t::compressed_srgb8_alpha8_astc_6x5x5_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 6,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_6x5x5_OES)
{
    .type = format_t::compressed_rgba_astc_6x5x5_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 6,
        .block_height    = 5,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_6x6x5_OES)
{
    .type = format_t::compressed_rgba_astc_6x6x5_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 6,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES)
{
    .type = format_t::compressed_srgb8_alpha8_astc_6x6x5_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 6,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_6x6x5_OES)
{
    .type = format_t::compressed_rgba_astc_6x6x5_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 6,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_6x6x6_OES)
{
    .type = format_t::compressed_rgba_astc_6x6x6_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 6,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES)
{
    .type = format_t::compressed_srgb8_alpha8_astc_6x6x6_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 6,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_ASTC_6x6x6_OES)
{
    .type = format_t::compressed_rgba_astc_6x6x6_oes,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 6,
        .block_height    = 6,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_texture_compression_astc"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG)
{
    .type = format_t::compressed_rgba_pvrtc_2bppv1_img,
    .vk_type = vk_format_t::pvrtc1_2bpp_unorm_block_img,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 8,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_IMG_texture_compression_pvrtc"sv,
        .web = "WEBGL_compressed_texture_pvrtc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG)
{
    .type = format_t::compressed_rgba_pvrtc_4bppv1_img,
    .vk_type = vk_format_t::pvrtc1_4bpp_unorm_block_img,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_IMG_texture_compression_pvrtc"sv,
        .web = "WEBGL_compressed_texture_pvrtc"sv,
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG)
{
    .type = format_t::compressed_rgba_pvrtc_2bppv2_img,
    .vk_type = vk_format_t::pvrtc2_2bpp_unorm_block_img,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 8,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_IMG_texture_compression_pvrtc2"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG)
{
    .type = format_t::compressed_rgba_pvrtc_4bppv2_img,
    .vk_type = vk_format_t::pvrtc2_4bpp_unorm_block_img,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_IMG_texture_compression_pvrtc2"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT)
{
    .type = format_t::compressed_srgb_alpha_pvrtc_2bppv1_ext,
    .vk_type = vk_format_t::pvrtc1_2bpp_srgb_block_img,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 8,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 4,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_EXT_pvrtc_sRGB"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT)
{
    .type = format_t::compressed_srgb_alpha_pvrtc_4bppv1_ext,
    .vk_type = vk_format_t::pvrtc1_4bpp_srgb_block_img,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgba,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 4,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_EXT_pvrtc_sRGB"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG)
{
    .type = format_t::compressed_srgb_alpha_pvrtc_2bppv2_img,
    .vk_type = vk_format_t::pvrtc2_2bpp_srgb_block_img,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 8,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_EXT_pvrtc_sRGB GL_IMG_texture_compression_pvrtc2"sv,
        .web = {},
    },
},
#endif
#if defined(GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG)
{
    .type = format_t::compressed_srgb_alpha_pvrtc_4bppv2_img,
    .vk_type = vk_format_t::pvrtc2_4bpp_srgb_block_img,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 16,
        .bpp             = 128,
        .component_count = 3,
        .srgb            = true,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_EXT_pvrtc_sRGB GL_IMG_texture_compression_pvrtc2"sv,
        .web = {},
    },
},
#endif
#if defined(GL_RGBA4) && defined(GL_BGRA) && defined(GL_UNSIGNED_SHORT_4_4_4_4_REV)
{
    .type = format_t::rgba4,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 4, .g = 4, .b = 4, .a = 4, },
        .type            = texture_type_t::unsigned_short_4_4_4_4_rev,
        .format          = texture_layout_t::bgra,
        .pixel_size      = 2,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_RGBA4) && defined(GL_RGBA) && defined(GL_UNSIGNED_SHORT_4_4_4_4_REV)
{
    .type = format_t::rgba4,
    .vk_type = static_cast<vk_format_t>(0),
    .compression = std::nullopt,
    .raw_format = RAW_FORMAT_TYPE{
        .bit_layout      = {.r = 4, .g = 4, .b = 4, .a = 4, },
        .type            = texture_type_t::unsigned_short_4_4_4_4_rev,
        .format          = texture_layout_t::rgba,
        .pixel_size      = 2,
        .component_count = 4,
        .component_size  = 2,
        .srgb            = false,
        .floating_point  = false,
    },
    .version = {
        .core = 0x120,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = {},
        .web = {},
    },
},
#endif
#if defined(GL_ETC1_RGB8_OES)
{
    .type = format_t::etc1_rgb8_oes,
    .vk_type = vk_format_t::etc2_r8g8b8_unorm_block,
    .compression = COMPRESSED_FORMAT_TYPE{
        .format          = texture_layout_t::rgb,
        .block_width     = 4,
        .block_height    = 4,
        .block_size      = 8,
        .bpp             = 64,
        .component_count = 3,
        .srgb            = false,
    },
    .raw_format = std::nullopt,
    .version = {
        .core = std::nullopt,
        .es = std::nullopt,
        .web = std::nullopt,
    },
    .extensions = {
        .core = {},
        .es = "GL_OES_compressed_ETC1_RGB8_texture"sv,
        .web = "WEBGL_compressed_texture_etc1 WEBGL_compressed_texture_etc"sv,
    },
},
#endif
