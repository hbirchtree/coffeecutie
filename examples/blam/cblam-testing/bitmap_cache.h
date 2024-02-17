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

    inline void load_from(
        blam::map_container<V> const& map,
        blam::magic_data_t const&     bitmap_magic)
    {
        index = blam::tag_index_view(map);
        bitm_header =
            blam::bitm::bitmap_atlas_view::from_data(bitmap_magic.data());
        magic = map.magic;
        bitm_magic =
            map.map->version == blam::version_t::xbox ? magic : bitmap_magic;
        bitm_magic.magic_offset = 0;
        evict_all();
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

    void allocate_storage()
    {
        using size_bucket = std::tuple<u32, u32>;

        struct pool_size
        {
            u32   num    = 0;
            u32   layers = 0;
            Veci2 max    = {};

            std::map<cache_id_t, size_bucket> images;
        };

        ProfContext _("Building texture atlases");

        std::map<bitm_format_hash, pool_size> fmt_count;

        /* Find final pool sizes */
        for(auto& bitm : m_cache)
        {
            auto const& fmt  = bitm.second.image.fmt;
            auto        hash = std::make_tuple(
                bitm.second.image.mip->type,
                fmt.pixfmt,
                fmt.comp,
                fmt.bfmt,
                fmt.cmpflg);
            auto& pool   = fmt_count[hash];
            auto  imsize = bitm.second.image.mip->isize;
            if(bitm.second.image.mip->mipmaps > params->mipmap_bias)
                imsize >>= params->mipmap_bias;
            //            auto&       surface =
            //            tex_buckets[bitm.second.image.bucket].surface;

            //            u32 mipmaps = surface->m_mipmaps;
            u32 pad = 0;
            //                = surface->m_format.pixfmt != PixFmt::RGB565 ? 4
            //                << mipmaps : 0;

            pool.num++;
            pool.max.x = std::max<u32>(pool.max.x, imsize.x + pad);
            pool.max.y = std::max<u32>(pool.max.y, imsize.y + pad);
            pool.images.insert(
                {bitm.first, std::make_tuple(imsize.x + pad, imsize.y + pad)});
        }

        /*
         * Generate metadata for use in shaders, like layer, scale and offset
         * This requires knowledge of the size of the pool and number of layers
         * in array textures
         */
        for(auto& pool_ : fmt_count)
        {
            auto& pool = pool_.second;

            Veci2 offset = {0, 0};

            //            auto& surface = tex_buckets[pool_.first].surface;

            u32 layer = 0;
            //            u32 mipmaps = surface->m_mipmaps;
            u32 max_pad = 0; // surface->m_format.pixfmt != PixFmt::RGB565 ? 4
                             // << mipmaps : 0;

            for(auto [id, fmt] : pool.images)
            {
                BitmapItem* img    = &m_cache.find(id)->second;
                auto        imsize = img->image.mip->isize;

                if(params->mipmap_bias > 0 &&
                   img->image.mip->mipmaps > params->mipmap_bias)
                {
                    imsize >>= params->mipmap_bias;
                    img->mipmaps.base = params->mipmap_bias;
                    img->mipmaps.last =
                        params->mipmap_bias +
                        std::min<i32>(
                            5, img->image.mip->mipmaps - params->mipmap_bias);
                } else
                {
                    img->mipmaps.base = 0;
                    img->mipmaps.last = img->image.mip->mipmaps;
                }

                if(img->header->type == blam::bitm::bitmap_type_t::cube)
                {
                    img->image.layer = layer++;
                    continue;
                }

                imsize.x += max_pad;
                imsize.y += max_pad;

                auto img_offset = offset;
                //                auto img_layer  = layer;

                [[maybe_unused]] Veci2 slack = {
                    pool.max.x - offset.x,
                    pool.max.y - offset.y,
                };

                bool fits_width = (offset.x + imsize.x) <= pool.max.x;

                if(fits_width)
                {
                    img_offset.x = offset.x;
                    img_offset.y = offset.y /*- imsize.h*/;
                    offset.x += imsize.x;
                } else
                {
                    layer++;
                    offset.x   = imsize.x;
                    img_offset = {};
                    //                    img_layer  = layer;
                }

                img->image.layer  = layer;
                img->image.offset = {
                    C_CAST<f32>(img_offset.x + max_pad / 2),
                    C_CAST<f32>(img_offset.y + max_pad / 2),
                };
                img->image.scale = {
                    C_CAST<f32>(imsize.x - max_pad),
                    C_CAST<f32>(imsize.y - max_pad),
                };
            }

            pool.layers = layer + 1;
        }

        /* Allocate the surfaces */
        for(auto& bucket : tex_buckets)
        {
            auto& props  = bucket.second;
            auto& pool   = fmt_count[bucket.first];
            i32   layers = C_CAST<i32>(pool.layers);
            auto  size =
                size_3d<i32>{pool.max.x, pool.max.y, layers}.convert<u32>();
            props.surface->alloc(size);

            auto [type, fmt, _, __, comp] = bucket.first;
            std::string bucket_name       = fmt::format(
                "cache_{0}_{1}_{2}",
                magic_enum::enum_name(type),
                magic_enum::enum_name(fmt),
                magic_enum::enum_name(comp));
            allocator->debug().annotate(*props.surface, bucket_name.data());
        }

        /* Commit the textures */
        for(auto& bitm : m_cache)
            commit_bitmap<gfx::compat::texture_2da_t>(bitm.second);

        for(auto& [_, pool] : fmt_count)
            for(auto [image_id, fmt] : pool.images)
            {
                auto  image_it = m_cache.find(image_id);
                auto* image    = &image_it->second;
                image->image.offset[0] /= pool.max.x;
                image->image.offset[1] /= pool.max.y;
                image->image.scale[0] /= pool.max.x;
                image->image.scale[1] /= pool.max.y;
            }
    }

    virtual BitmapItem predict_impl(
        blam::tagref_t const& bitmap, i16 idx) override
    {
        using namespace typing::pixels;

        auto _ = allocator->debug().scope("BitmapCache");

        using blam::tag_class_t;

        if(idx == -1)
            return {};

        if(!bitmap.valid() || bitmap.tag_class != tag_class_t::bitm)
            Throw(std::runtime_error("non-bitm tag passed to BitmapCache"));

        auto it          = index.find(bitmap);
        auto shader_name = bitmap.to_name().to_string(magic);

        if(it == index.end())
        {
            cDebug("Failed to find bitmap: {0}", shader_name);
            return {};
        }

        blam::tag_t const* bitm_tag = &(*it);

        auto [bitm_ptr, curr_magic] =
            bitm_tag->image(magic, bitm_header).value();

        if(!bitm_ptr)
            Throw(undefined_behavior("failed to get bitmap header"));

        auto const& bitm = *bitm_ptr;

        BitmapItem out;
        out.header = &bitm;
        out.tag    = bitm_tag;

        auto sequences = bitm.sequences.data(curr_magic).value();
        for(auto const& sequence : sequences)
        {
            //            if(sequence.name.str().empty())
            continue;
            cDebug(
                "Sequence: {0} : {1} bitmaps",
                sequence.name.str(),
                sequence.bitmap_count);
            for(auto const& sprite : sequence.sprites.data(curr_magic).value())
            {
                cDebug(
                    " - Sprite {0}+{1}",
                    sequence.first_bitmap,
                    sprite.bitmap_index);
            }
        }

        if(auto image_ = bitm.images.data(curr_magic); image_.has_value())
        {
            auto& im = image_.value();
            if(static_cast<u16>(idx) >= im.size())
                return {};

            auto const& image = im[idx];

            auto& img = out.image;
            img.mip   = &image;
            img.layer = 0;

            PixDesc fmt;
            if(image.compressed())
            {
                std::tie(fmt.pixfmt, fmt.cmpflg) = image.to_compressed_fmt();

                fmt.comp = convert::to<PixCmp>(fmt.c);
            } else
            {
                fmt.pixfmt                   = image.to_pixfmt();
                std::tie(fmt.bfmt, fmt.comp) = image.to_fmt();
            }

            img.bucket = create_hash(fmt, img.mip->type);
            img.fmt    = fmt;

            switch(im[0].type)
            {
            case blam::bitm::type_t::tex_2d: {
                auto& bucket =
                    get_bucket<gfx::compat::texture_2da_t>(fmt, img.mip->type);
                img.layer = bucket.ptr++;
                break;
            }
#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION >= 0x320
            case blam::bitm::type_t::tex_cube: {
                if(!allocator->feature_info().texture.cube_array)
                    return {};
                auto& bucket =
                    get_bucket<gfx::texture_cube_array_t>(fmt, img.mip->type);
                img.layer = bucket.ptr++;
                break;
            }
#endif
            default:
                cDebug(
                    "unimplemented texture type: {0}",
                    magic_enum::enum_name(im[0].type));
                return {};
            }

        } else
            return {};

        return out;
    }

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
