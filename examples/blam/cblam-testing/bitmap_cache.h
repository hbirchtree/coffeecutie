#pragma once

#include "caching_item.h"
#include "data.h"
#include "data_cache.h"

#include <blam/volta/blam_swizzle.h>
#include <coffee/graphics/apis/gleam/rhi_texture_atlas.h>

using BitmapManifest =
    compo::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

using compo::time_point;
using libc_types::u8;

template<typename V>
struct BitmapCache
    : DataCache<BitmapItem, std::tuple<u32, i16>, blam::tagref_t const&, i16>
    , compo::RestrictedSubsystem<BitmapCache<V>, BitmapManifest>
{
    using type  = BitmapCache<V>;
    using Proxy = compo::proxy_of<BitmapManifest>;

    struct TextureBucket
    {
        std::shared_ptr<gfx::texture_t> surface;
        std::shared_ptr<gfx::sampler_t> sampler;

        u32                ptr;
        PixDesc            fmt;
        blam::bitm::type_t type;

        u32 mip_bias{0};

        Veci2 max_size{};
        u32   layers{0};

        template<typename T>
        auto& texture_as()
        {
            return *std::dynamic_pointer_cast<T>(surface);
        }
    };

    struct bitmap_slot_t
    {
        Veci2 max_size{}; /* bucket dimensions the UVs were normalized by */
        u32   layer{0};
        u32   array_level{0}; /* mip level a sub-sized cube sits at */
        Veci2 pixel_offset{}; /* where the upload writes */
        Veci2 gutter{};       /* wrap padding reserved around the tile */
        Vecf2 offset{};       /* normalized, what the shader samples with */
        Vecf2 scale{};
        u32   mip_base{0};
        u32   mip_last{0};
    };

    BitmapCache(gfx::api* allocator, RenderingParameters const* params)
        : allocator(allocator)
        , params(params)
    {
        supports_tex3d = allocator->feature_info().texture.texture_3d;
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        index = blam::tag_index_view(map);
        magic = map.magic;
        if(map.map->version == blam::version_t::xbox)
        {
            bitm_magic             = map.magic;
            bitm_magic.file_offset = 0;
        }
        evict_all();
    }

    /*! Should not be called if we're parsing Xbox maps! */
    inline void load_bitmaps_from(blam::map_ptr const& bitmap_magic)
    {
        index.add_atlas(blam::atlas_type_t::bitmaps, bitmap_magic.data());
        bitm_header = blam::atlas_view::from_data(bitmap_magic.data());
        bitm_magic  = bitmap_magic;
        bitm_magic.file_offset = 0;
    }

    blam::tag_index_view<V>    index;
    gfx::api*                  allocator;
    RenderingParameters const* params;

    blam::map_ptr    magic;
    blam::map_ptr    bitm_magic;
    blam::atlas_view bitm_header;

    struct storage_reservation_t
    {
        PixDesc            fmt;
        blam::bitm::type_t type;
        Veci2              max_size{};
        u32                layers{0};
    };

    std::map<bitm_format_hash, TextureBucket>          tex_buckets;
    std::map<bitm_format_hash, storage_reservation_t>  m_reservations;
    std::map<std::tuple<u32, i16>, bitmap_slot_t>      m_slots;

    u32  max_mipmap{3};
    bool supports_tex3d{true};

    static inline bitm_format_hash create_hash(
        PixDesc const& fmt, blam::bitm::type_t type)
    {
        return std::make_tuple(
            type, fmt.pixfmt, fmt.comp, fmt.bfmt, fmt.cmpflg);
    }

    template<typename T>
    requires std::is_same_v<T, gfx::compat::texture_2da_t>
    auto bucket_to_type()
    {
        return gfx::textures::d2_array;
    }

    template<typename T>
    requires std::is_same_v<T, gfx::compat::texture_cube_array_t>
    auto bucket_to_type()
    {
        return gfx::textures::cube_array;
    }

    template<typename T>
    TextureBucket& get_bucket(
        PixDesc const&     fmt,
        blam::bitm::type_t type = blam::bitm::type_t::tex_2d)
    {
        using typing::Filtering;

        auto hash = create_hash(fmt, type);

        auto it = tex_buckets.find(hash);

        if(it != tex_buckets.end())
            return it->second;

        auto& bucket = tex_buckets.insert({hash, {}}).first->second;

        bucket.ptr = 0;
        bucket.fmt = fmt;

        bool no_mipmap = fmt.pixfmt == pix_fmt::RGB565;

        bucket.mip_bias = no_mipmap ? 0 : params->mipmap_bias;

        if constexpr(std::is_same_v<T, gfx::compat::texture_cube_array_t>)
        {
            auto surface = std::make_shared<gfx::compat::texture_cube_array_t>(
                allocator, fmt, max_mipmap);
            surface->set_usage_hint(
                gfx::compat::texture_usage_hint_t::per_base_instance);
            bucket.surface = std::move(surface);
        } else
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            if constexpr(std::is_same_v<T, gfx::texture_3d_t>)
        {
            // TODO: Find fallback on GL ES 2.0
            // Problem here is that shader_plasma uses TWO 3D textures
            // So shimming it to single-texture is not an option
            // Special-case shader_plasma on GL ES 2.0?
            bucket.surface =
                allocator->alloc_texture(gfx::textures::d3, fmt, max_mipmap);
        } else
#endif
        {
            auto surface = std::make_shared<gfx::compat::texture_2da_t>(
                allocator, fmt, no_mipmap ? 1 : (max_mipmap - bucket.mip_bias));
            surface->set_usage_hint(
                gfx::compat::texture_usage_hint_t::sparse_atlas);
            bucket.surface = std::move(surface);
        }

        bucket.type    = type;
        bucket.sampler = bucket.surface->sampler();

        bucket.sampler->alloc();
        bucket.sampler->set_filtering(
            Filtering::Linear, Filtering::Linear, Filtering::Linear);
        bucket.sampler->set_anisotropic(4);

        return bucket;
    }

    template<typename BucketType>
    inline void upload_mipmap(
        TextureBucket&       bucket,
        BitmapItem const&    img,
        blam::map_ptr const& magic,
        u16                  mipmap)
    {
        u32 mips = bucket.surface->m_mipmaps;

        if(mipmap + img.image.array_level >= mips ||
           mipmap > img.image.mip->mipmaps)
            return;

        auto size = img.image.mip->isize;
        size.x >>= img.mipmaps.base + mipmap;
        size.y >>= img.mipmaps.base + mipmap;

        if(size.x % 4 != 0 || size.y % 4 != 0)
            return;

        Veci2 const pool_offset =
            Veci2(img.image.offset[0], img.image.offset[1]);
        u32 const dst_level = mipmap;

        mipmap += img.mipmaps.base;

        if(bucket.type == blam::bitm::type_t::tex_cube)
        {
            gfx::compat::texture_cube_array_t& texture =
                bucket.template texture_as<
                    gfx::compat::texture_cube_array_t>();
            auto face_size = img.image.mip->layer_mip_bytes(mipmap);

            /* Xbox swizzles each cube face like a 2D texture; deswizzle per
             * face into a linear buffer (compressed faces are never swizzled
             * and don't set the flag). */
            const bool swizzled =
                (static_cast<u16>(img.image.mip->flags) &
                 static_cast<u16>(blam::bitm::flags_t::swizzled)) &&
                size.x > 0 && size.y > 0;
            const u32 bpp =
                swizzled
                    ? static_cast<u32>(
                          face_size / (static_cast<size_t>(size.x) * size.y))
                    : 0u;
            std::array<std::vector<u8>, 6> linear_faces;
            auto face = [&](u32 idx) -> semantic::Span<const u8> {
                semantic::Span<const u8> raw =
                    img.image.mip->template cube_face<V>(magic, idx, mipmap);
                if(swizzled && bpp != 0)
                {
                    linear_faces[idx].resize(face_size);
                    if(blam::swizzle::deswizzle_bytes(
                           semantic::Span<const u8>(
                               raw.data(), raw.size_bytes()),
                           semantic::Span<u8>(
                               linear_faces[idx].data(),
                               linear_faces[idx].size()),
                           static_cast<u32>(size.x),
                           static_cast<u32>(size.y),
                           bpp))
                        return semantic::Span<const u8>(
                            linear_faces[idx].data(),
                            linear_faces[idx].size());
                }
                return raw;
            };
            std::array<semantic::Span<const u8>, 6> faces = {{
                face(0),
                face(1),
                face(2),
                face(3),
                face(4),
                face(5),
            }};
            texture.upload(
                faces,
                Veci3{0, 0, img.image.layer},
                Veci3{size.x, size.y, 1},
                mipmap - img.mipmaps.base + img.image.array_level);
        } else
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
            if(bucket.type == blam::bitm::type_t::tex_3d)
        {
            gfx::texture_3d_t& texture =
                bucket.template texture_as<gfx::texture_3d_t>();
            auto img_data = img.image.mip->data(magic, mipmap);
            cDebug(
                "3D texture data: {}x{}x{} {}+{}",
                img.image.mip->isize.x,
                img.image.mip->isize.y,
                img.image.mip->depth,
                static_cast<const void*>(img_data.data()),
                img_data.size_bytes());
        } else
#endif
        {
            gfx::compat::texture_2da_t& texture =
                bucket.template texture_as<gfx::compat::texture_2da_t>();

            auto mip_data = img.image.mip->data(magic, mipmap);

            /* Xbox stores uncompressed textures Morton-swizzled; deswizzle into
             * a linear buffer before upload (compressed are never swizzled). */
            std::vector<u8> linear;
            if((static_cast<u16>(img.image.mip->flags) &
                static_cast<u16>(blam::bitm::flags_t::swizzled)) &&
               size.x > 0 && size.y > 0)
            {
                size_t pixels = static_cast<size_t>(size.x) * size.y;
                u32    bpp = static_cast<u32>(mip_data.size_bytes() / pixels);
                linear.resize(mip_data.size_bytes());
                if(bpp != 0 &&
                   blam::swizzle::deswizzle_bytes(
                       semantic::Span<const u8>(
                           mip_data.data(), mip_data.size_bytes()),
                       semantic::Span<u8>(linear.data(), linear.size()),
                       static_cast<u32>(size.x),
                       static_cast<u32>(size.y),
                       bpp))
                    mip_data =
                        semantic::Span<const u8>(linear.data(), linear.size());
            }

            /* The gutter around the tile is filled with copies of its own
             * opposite edges, so a filter tap leaving the tile lands on the
             * texels a repeat wrap would have given it. */
            std::vector<u8> padded;
            auto            tile = gfx::pad_tile(
                padded,
                gl::tex::format_of(bucket.fmt),
                pool_offset,
                img.image.gutter,
                dst_level,
                Veci2{size.x, size.y},
                mip_data);

            texture.upload(
                tile.data,
                Veci3{
                    tile.offset[0],
                    tile.offset[1],
                    static_cast<i32>(img.image.layer)},
                Veci3{tile.size[0], tile.size[1], 1},
                static_cast<i32>(dst_level));
        }
    }

    template<typename BucketType>
    void commit_bitmap(BitmapItem& img)
    {
        auto _ = allocator->debug().scope(img.tag->to_name().to_string(magic));

        auto& bucket =
            get_bucket<BucketType>(img.image.fmt, img.image.mip->type);

        C_UNUSED(auto name) = img.tag->to_name().to_string(magic);

        auto bmagic = img.image.mip->shared() ? bitm_magic : magic.ptr_only();

        for(auto i : range<u16>(img.mipmaps.last - img.mipmaps.base))
        {
            auto offset = Veci2(img.image.offset[0], img.image.offset[1]);
            if(offset.x % 4 != 0 || offset.y % 4 != 0)
                break;

            upload_mipmap<BucketType>(bucket, img, bmagic, i);
        }

        img.image.bias = -std::max<f32>(
            static_cast<f32>(bucket.surface->m_mipmaps) -
                static_cast<f32>(img.image.mip->mipmaps),
            0.f);

        /* Lightmaps do not use mipmapping */
        if(img.image.mip->mipmaps == 0)
            upload_mipmap<BucketType>(bucket, img, bmagic, 0);
    }

    u32 bias_of(blam::bitm::image_t const& img) const;

    void calculate_storage();

    void reserve_storage();

    void begin_map()
    {
        calculate_storage();
        reserve_storage();
    }

    virtual BitmapItem predict_impl(
        blam::tagref_t const& bitmap, i16 idx) override;

    virtual void evict_impl() override
    {
        for(auto& bucket : tex_buckets)
        {
            bucket.second.sampler->dealloc();
            bucket.second.surface->dealloc();
        }

        tex_buckets.clear();
        m_reservations.clear();
        m_slots.clear();
    }

    virtual std::tuple<u32, i16> get_id(
        blam::tagref_t const& tag, i16 idx) override
    {
        return std::make_tuple(tag.tag_id, idx);
    }

    generation_idx_t resolve(blam::tagref_t const& array, i16 idx = 0)
    {
        return predict(array, idx);
    }

    std::vector<generation_idx_t> resolve_all(blam::tagref_t const& array)
    {
        auto it = index.find(array);
        if(it == index.end())
            return {};
        blam::tag_t const& tag     = *it;
        auto               img_opt = tag.image(magic, bitm_header);
        if(img_opt.has_error())
            return {};
        std::vector<generation_idx_t> out;
        blam::bitm::header_t const*   img = img_opt.value().first;
        for(auto i : range<u32>(img->images.count))
            out.push_back(predict(array, i));
        return out;
    }

    u32 type_mask(BitmapItem const& bitm)
    {
        using typing::pixels::pix_fmt;
        switch(bitm.image.fmt.pixfmt)
        {
        case pix_fmt::BCn: {
            switch(bitm.image.fmt.cmpflg)
            {
            case typing::pixels::comp_flags::BC1:
                return 0x01000000;
            case typing::pixels::comp_flags::BC2:
                return 0x02000000;
            case typing::pixels::comp_flags::BC3:
                return 0x03000000;
            default:
                break;
            }
            break;
        }
        case pix_fmt::RGB565:
            return 0x04000000;
        case pix_fmt::R8:
            /* Xbox luminance/alpha formats all upload as R8; the shader
             * needs the semantic to reconstruct rgb/alpha (A8=000A,
             * Y8=LLL1, AY8=LLLL). P8 (palettized bump) stays raw. */
            switch(bitm.image.mip->format)
            {
            case blam::bitm::format_t::A8:
                return 0x09000000;
            case blam::bitm::format_t::Y8:
                return 0x0A000000;
            case blam::bitm::format_t::AY8:
                return 0x0B000000;
            case blam::bitm::format_t::P8:
                return 0x0D000000;
            default:
                return 0x05000000;
            }
        case pix_fmt::RG8:
            /* A8Y8 = luminance + alpha in r/g */
            return bitm.image.mip->format == blam::bitm::format_t::A8Y8
                       ? 0x0C000000
                       : 0x06000000;
        case pix_fmt::RGBA4:
            return 0x07000000;
        case pix_fmt::RGBA8:
            return 0x08000000;
        default:
            break;
        }
        return 0x0;
    }

    u32 get_atlas_layer(generation_idx_t bitm)
    {
        if(!bitm.valid())
            return 0;
        auto bitmit = find(bitm);
        if(bitmit == m_cache.end())
            return 0;
        BitmapItem const& bitmap = bitmit->second;
        return type_mask(bitmap) | (bitmap.image.array_level << 16) |
               bitmap.image.layer;
    }

    std::shared_ptr<gfx::sampler_t> cube_sampler()
    {
        for(auto& [hash, bucket] : tex_buckets)
            if(bucket.type == blam::bitm::type_t::tex_cube)
                return bucket.sampler;
        return nullptr;
    }

    std::shared_ptr<gfx::texture_t> cube_texture(generation_idx_t bitm)
    {
        if(!bitm.valid())
            return nullptr;
        auto bitmit = find(bitm);
        if(bitmit == m_cache.end())
            return nullptr;
        BitmapItem const& bitmap = bitmit->second;
        if(bitmap.image.mip->type != blam::bitm::type_t::tex_cube)
            return nullptr;
        auto bucket = tex_buckets.find(
            create_hash(bitmap.image.fmt, bitmap.image.mip->type));
        if(bucket == tex_buckets.end())
            return nullptr;
        auto cube =
            std::dynamic_pointer_cast<gfx::compat::texture_cube_array_t>(
                bucket->second.surface);
        if(!cube)
            return nullptr;
        return cube->subtexture(bitmap.image.layer);
    }

    template<typename T>
    BitmapItem const* assign_atlas_data(T& out, generation_idx_t bitm)
    {
        if(!bitm.valid())
            return nullptr;
        auto bitmit = find(bitm);
        if(bitmit == m_cache.end())
            return nullptr;
        BitmapItem const& bitmap = bitmit->second;
        out.atlas_offset         = bitmap.image.offset;
        out.atlas_scale          = bitmap.image.scale;
        /* We encode the type of texture here. This lets the shader
         * pick out BC1, BC2, BC3 and etc. by itself.
         * Massive benefits to batching without doing bindless textures! */
        out.layer = type_mask(bitmap) | bitmap.image.layer;
        return &bitmap;
    }
};
