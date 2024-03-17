#pragma once

#include "caching_item.h"
#include "data.h"
#include "data_cache.h"
#include "graphics_api.h"

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

        template<typename T>
        auto& texture_as()
        {
            return *std::dynamic_pointer_cast<T>(surface);
        }
    };

    BitmapCache(gfx::api* allocator, RenderingParameters const* params)
        : allocator(allocator)
        , params(params)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        index = blam::tag_index_view(map);
        magic = map.magic;
        if(map.map->version == blam::version_t::xbox)
        {
            bitm_magic              = map.magic;
            bitm_magic.magic_offset = 0;
        }
        evict_all();
    }

    /*! Should not be called if we're parsing Xbox maps! */
    inline void load_bitmaps_from(blam::magic_data_t const& bitmap_magic)
    {
        bitm_header =
            blam::bitm::bitmap_atlas_view::from_data(bitmap_magic.data());
        bitm_magic              = bitmap_magic;
        bitm_magic.magic_offset = 0;
    }

    blam::tag_index_view<V>    index;
    gfx::api*                  allocator;
    RenderingParameters const* params;

    blam::magic_data_t            magic;
    blam::magic_data_t            bitm_magic;
    blam::bitm::bitmap_atlas_view bitm_header;

    std::map<bitm_format_hash, TextureBucket> tex_buckets;

    u32 max_mipmap{3};

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

#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION >= 0x320
    template<typename T>
    requires std::is_same_v<T, gfx::texture_cube_array_t>
    auto bucket_to_type()
    {
        return gfx::textures::cube_array;
    }
#endif

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

        bool no_mipmap = fmt.pixfmt == PixFmt::RGB565;

        bucket.mip_bias = no_mipmap ? 0 : params->mipmap_bias;

#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION >= 0x320
        if(std::is_same_v<T, gfx::texture_cube_array_t>)
            bucket.surface = allocator->alloc_texture(
                gfx::textures::cube_array, fmt, max_mipmap);
        else
#endif
            bucket.surface = std::make_shared<gfx::compat::texture_2da_t>(
                allocator, fmt, no_mipmap ? 1 : (max_mipmap - bucket.mip_bias));
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
        TextureBucket&            bucket,
        BitmapItem const&         img,
        blam::magic_data_t const& magic,
        u16                       mipmap)
    {
        u32 mips = bucket.surface->m_mipmaps;

        if(mipmap >= mips || mipmap > img.image.mip->mipmaps)
            return;

        auto size = img.image.mip->isize;
        size.x >>= img.mipmaps.base + mipmap;
        size.y >>= img.mipmaps.base + mipmap;

        if(size.x % 4 != 0 || size.y % 4 != 0)
            return;

        i32 mip_pad = 0; // bucket.surface->m_format.pixfmt != PixFmt::RGB565
                         //? 2 << (mips - mipmap)
                         //: 0;

        Veci2 pool_offset = Veci2(img.image.offset[0], img.image.offset[1]);
        Veci2 tex_offset  = {
            (pool_offset[0] >> mipmap) - mip_pad,
            (pool_offset[1] >> mipmap) - mip_pad};

        (tex_offset[0] >>= 2) <<= 2;
        (tex_offset[1] >>= 2) <<= 2;

        mipmap += img.mipmaps.base;

#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION >= 0x320
        if(bucket.type == blam::bitm::type_t::tex_cube)
        {
            gfx::texture_cube_array_t& texture =
                bucket.template texture_as<gfx::texture_cube_array_t>();
            auto img_data  = img.image.mip->data(magic, mipmap);
            auto face_size = img_data.size_bytes() / 6;
            std::array<semantic::Span<const u8>, 6> faces = {{
                img_data.subspan(face_size * 0, face_size),
                img_data.subspan(face_size * 2, face_size),
                img_data.subspan(face_size * 1, face_size),
                img_data.subspan(face_size * 3, face_size),
                img_data.subspan(face_size * 4, face_size),
                img_data.subspan(face_size * 5, face_size),
            }};
            texture.upload(
                faces,
                Veci3{0, 0, img.image.layer},
                Veci3{size.x, size.y, 1},
                mipmap - img.mipmaps.base);
        } else
#endif
        {
            gfx::compat::texture_2da_t& texture =
                bucket.template texture_as<gfx::compat::texture_2da_t>();

            texture.upload(
                img.image.mip->data(magic, mipmap),
                Veci3{
                    tex_offset.x + mip_pad,
                    tex_offset.y + mip_pad,
                    static_cast<i32>(img.image.layer)},
                Veci3{size.x, size.y, 1},
                mipmap - img.mipmaps.base);
        }
    }

    template<typename BucketType>
    void commit_bitmap(BitmapItem& img)
    {
        auto _ = allocator->debug().scope(img.tag->to_name().to_string(magic));

        auto& bucket =
            get_bucket<BucketType>(img.image.fmt, img.image.mip->type);

        C_UNUSED(auto name) = img.tag->to_name().to_string(magic);

        auto bmagic = img.image.mip->shared() ? bitm_magic : magic.no_magic();

        for(auto i : range<u16>(img.mipmaps.last - img.mipmaps.base))
        {
            auto offset = Veci2(img.image.offset[0], img.image.offset[1]);
            if(offset.x % 4 != 0 || offset.y % 4 != 0)
                break;

            upload_mipmap<BucketType>(bucket, img, bmagic, i);
        }

        img.image.bias = 10;
        //        img.image.bias = std::max<f32>(

        //            static_cast<f32>(bucket.surface->m_mipmaps)
        //                - static_cast<f32>(img.image.mip->mipmaps),
        //            0);

        /* Lightmaps do not use mipmapping */
        if(img.image.mip->mipmaps == 0)
            upload_mipmap<BucketType>(bucket, img, bmagic, 0);
    }

    void allocate_storage();

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

    generation_idx_t resolve_all(blam::tagref_t const& array, i16 idx = 0)
    {
        return predict(array, idx);
    }

    u32 type_mask(BitmapItem const& bitm)
    {
        using typing::pixels::PixFmt;
        switch(bitm.image.fmt.pixfmt)
        {
        case PixFmt::BCn: {
            switch(bitm.image.fmt.cmpflg)
            {
            case typing::pixels::CompFlags::BC1:
                return 0x01000000;
            case typing::pixels::CompFlags::BC2:
                return 0x02000000;
            case typing::pixels::CompFlags::BC3:
                return 0x03000000;
            default:
                break;
            }
            break;
        }
        case PixFmt::RGB565:
            return 0x04000000;
        case PixFmt::R8:
            return 0x05000000;
        case PixFmt::RG8:
            return 0x06000000;
        case PixFmt::RGBA4:
            return 0x07000000;
        case PixFmt::RGBA8:
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
        return type_mask(bitmap) | bitmap.image.layer;
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

    void start_restricted(Proxy&, time_point const&)
    {
    }

    void end_restricted(Proxy&, time_point const&)
    {
    }
};
