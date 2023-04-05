#pragma once

#include <blam/cblam.h>
#include <coffee/comp_app/AppWrap>
#include <coffee/comp_app/gl_config.h>
#include <coffee/core/CApplication>
#include <coffee/core/CArgParser>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/EventHandlers>
#include <coffee/core/Scene>

#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_compat.h>

#include <coffee/strings/libc_types.h>
#include <coffee/strings/vector_types.h>

#include <coffee/core/CDebug>

#include "blam_files.h"
#include "materials.h"
#include "selected_version.h"

using gfx_api = gleam::api;
using typing::vector_types::Matf3;
using typing::vector_types::Matf4;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;
using typing::vector_types::Veci2;
using typing::vector_types::Veci3;

namespace gfx {

using namespace gleam;

}

using namespace Coffee;

using cache_id_t = u64;

struct generation_idx_t
{
    cache_id_t i{0};
    u32        gen{0};
    u32        _pad{};

    inline bool valid() const
    {
        return i != 0 && gen != 0;
    }
};

using bitm_format_hash
    = Tup<blam::bitm::type_t, PixFmt, PixCmp, BitFmt, CompFlags>;

using Components::time_point;

struct BSPItem
{
    struct Mesh
    {
        blam::bsp::submesh_header const* mesh{nullptr};
        gleam::draw_command::data_t      draw;
        generation_idx_t                 light_bitm;
        generation_idx_t                 shader;
    };
    struct Group
    {
        blam::bsp::submesh_group const* group{nullptr};
        Vector<Mesh>                    meshes;
    };

    blam::bsp::header const*            mesh{nullptr};
    blam::tag_t const*                  tag{nullptr};
    Vector<Group>                       groups;
    Vector<gleam::draw_command::data_t> portals;

    inline bool valid() const
    {
        return mesh;
    }
};

template<typename V>
struct ModelItem
{
    struct SubModel
    {
        blam::mod2::submesh_header const* header{nullptr};
        gleam::draw_command::data_t       draw;
        generation_idx_t                  shader;
    };
    struct LOD
    {
        blam::mod2::geometry_header<V> const* header{nullptr};
        Vector<SubModel>                      sub;
    };

    blam::mod2::header<V> const* header{nullptr};
    blam::tag_t const*           tag{nullptr};
    LOD                          mesh;
    Vecf2                        uvscale;

    inline bool valid() const
    {
        return header;
    }
};

struct ModelAssembly
{
    Span<blam::mod2::region const>     regions;
    Vector<Array<generation_idx_t, 5>> models;
};

struct BitmapItem
{
    blam::bitm::header_t const* header{nullptr};
    blam::tag_t const*          shader_tag{nullptr};
    blam::tag_t const*          tag{nullptr};
    struct
    {
        blam::bitm::image_t const* mip{nullptr};
        PixDesc                    fmt;
        bitm_format_hash           bucket;

        u32   layer{0};
        Vecf2 offset{};
        Vecf2 scale{};
        f32   bias{0.f};
    } image;

    inline bool valid() const
    {
        return header;
    }
};

struct ShaderItem
{
    blam::shader::radiosity_properties const* header{nullptr};
    blam::tag_t const*                        tag{nullptr};

    inline blam::tag_class_t type()
    {
        if(!tag)
            Throw(undefined_behavior("invalid shader item"));

        return tag->tagclass_e[0];
    }

    struct soso_t
    {
        generation_idx_t base_bitm;
        generation_idx_t multi_bitm;
        generation_idx_t detail_bitm;
    };
    struct senv_t
    {
        generation_idx_t base_bitm;
        generation_idx_t primary_bitm;
        generation_idx_t secondary_bitm;
        generation_idx_t micro_bitm;

        generation_idx_t bump;

        generation_idx_t self_illum;

        generation_idx_t reflection_bitm;
    };
    struct schi_t
    {
        std::array<generation_idx_t, 4> maps;
        std::array<generation_idx_t, 4> layers;
    };
    struct scex_t
    {
        std::array<generation_idx_t, 4> maps;
        std::array<generation_idx_t, 4> layers;
        generation_idx_t                lens_flare;
    };
    struct swat_t
    {
        generation_idx_t base;
        generation_idx_t reflection;
    };

    union
    {
        generation_idx_t color_bitm;

        soso_t soso;
        senv_t senv;
        schi_t schi;
        scex_t scex;
        swat_t swat;

        struct
        {
        } smet;
        struct
        {
        } sgla;
        struct
        {
        } spla;
    };

    inline bool valid() const
    {
        return header;
    }
};

template<typename T, typename IdType, typename... IType>
struct DataCache : non_copy
{
    static constexpr cache_id_t invalid_id = 0;

    inline generation_idx_t predict(IType... param)
    {
        IdType item_id  = get_id(std::forward<IType>(param)...);
        auto   cache_it = m_cache_key.find(item_id);

        if(cache_it != m_cache_key.end())
        {
            auto cached_it = m_cache.find(cache_it->second);

            if(cached_it != m_cache.end())
                return {cached_it->first, generation};
            else
                Throw(undefined_behavior("corrupt key cache"));
        }

        cache_id_t out  = ++counter;
        T          item = predict_impl(param...);

        if(!item.valid())
            return {invalid_id, 0};

        m_cache.insert({out, std::move(item)});
        m_cache_key.emplace(item_id, out);

        return {out, generation};
    }

    auto find(generation_idx_t id)
    {
        if(id.gen < generation)
            Throw(undefined_behavior("stale reference"));

        return m_cache.find(id.i);
    }

    void evict_all()
    {
        evict_impl();

        m_cache.clear();
        m_cache_key.clear();
        counter = 0;
        generation++;
    }

    cache_id_t              counter{0};
    Map<cache_id_t, T>      m_cache;
    Map<IdType, cache_id_t> m_cache_key;
    u32                     generation{1};

    virtual T      predict_impl(IType... param) = 0;
    virtual IdType get_id(IType... args)        = 0;
    virtual void   evict_impl()
    {
    }
};

using BitmapManifest
    = Components::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename V>
struct BitmapCache
    : DataCache<BitmapItem, Tup<u32, i16>, blam::tagref_t const&, i16>,
      Components::RestrictedSubsystem<BitmapCache<V>, BitmapManifest>
{
    using type  = BitmapCache;
    using Proxy = Components::proxy_of<BitmapManifest>;

    struct TextureBucket
    {
        ShPtr<gfx::texture_t> surface;
        ShPtr<gfx::sampler_t> sampler;

        u32                ptr;
        PixDesc            fmt;
        blam::bitm::type_t type;

        template<typename T>
        auto& texture_as()
        {
            return *std::dynamic_pointer_cast<T>(surface);
        }
    };

    BitmapCache(BlamFiles const* files, gfx_api* allocator) :
        files(files), allocator(allocator)
    {
    }

    inline void load_from(
        blam::map_container<V> const& map,
        blam::magic_data_t const&     bitmap_magic)
    {
        index       = blam::tag_index_view(map);
        bitm_header = blam::bitm::bitmap_header_t::from_data(
            BytesConst::ofBytes(bitmap_magic.base_ptr, bitmap_magic.max_size));
        magic = map.magic;
        bitm_magic
            = map.map->version == blam::version_t::xbox ? magic : bitmap_magic;
        bitm_magic.magic_offset = 0;
        evict_all();
    }

    blam::tag_index_view<V> index;
    BlamFiles const*        files;
    gfx_api*                allocator;

    blam::magic_data_t                 magic;
    blam::magic_data_t                 bitm_magic;
    blam::bitm::bitmap_header_t const* bitm_header{nullptr};

    Map<bitm_format_hash, TextureBucket> tex_buckets;

    static inline bitm_format_hash create_hash(
        PixDesc const& fmt, blam::bitm::type_t type)
    {
        return std::make_tuple(
            type, fmt.pixfmt, fmt.comp, fmt.bfmt, fmt.cmpflg);
    }

    template<typename T>
    requires std::is_same_v<T, gfx::texture_2da_t>
    auto bucket_to_type()
    {
        return gfx::textures::d2_array;
    }
    template<typename T>
    requires std::is_same_v<T, gfx::compat::texture_2da_t>
    auto bucket_to_type()
    {
        return gfx::textures::d2_array;
    }
    template<typename T>
    requires std::is_same_v<T, gfx::texture_3d_t>
    auto bucket_to_type()
    {
        return gfx::textures::d3;
    }
    template<typename T>
    requires std::is_same_v<T, gfx::texture_cube_array_t>
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
        if(std::is_same_v<T, gfx::texture_cube_array_t>)
            bucket.surface
                = allocator->alloc_texture(gfx::textures::cube_array, fmt, 5);
        else
            bucket.surface = std::make_shared<gfx::compat::texture_2da_t>(
                allocator, fmt, fmt.pixfmt == PixFmt::RGB565 ? 2 : 5);
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

        if(mipmap >= mips)
            return;

        auto size = img.image.mip->isize.convert<i32>();
        size.w >>= mipmap;
        size.h >>= mipmap;

        if(size.w % 4 != 0 || size.h % 4 != 0)
            return;

        i32 mip_pad = 0; // bucket.surface->m_format.pixfmt != PixFmt::RGB565
                         //? 2 << (mips - mipmap)
                         //: 0;

        Veci2 pool_offset = Veci2(img.image.offset.x(), img.image.offset.y());
        Veci2 tex_offset
            = {(pool_offset.x() >> mipmap) - mip_pad,
               (pool_offset.y() >> mipmap) - mip_pad};

        (tex_offset.x() >>= 2) <<= 2;
        (tex_offset.y() >>= 2) <<= 2;

        if(bucket.type == blam::bitm::type_t::tex_cube)
        {
            gfx::texture_cube_array_t& texture
                = bucket.template texture_as<gfx::texture_cube_array_t>();
            auto img_data = img.image.mip->data(magic, mipmap);
            cDebug(
                "Cube map: {0}x{1}x6 -> {2} bytes",
                img.image.mip->isize.w,
                img.image.mip->isize.h,
                img_data.size_bytes());
            //            texture.upload();
        } else
        {
            gfx::compat::texture_2da_t& texture
                = bucket.template texture_as<gfx::compat::texture_2da_t>();

            texture.upload(
                img.image.mip->data(magic, mipmap),
                Veci3{
                    tex_offset.x() + mip_pad,
                    tex_offset.y() + mip_pad,
                    static_cast<i32>(img.image.layer)},
                size_3d<i32>{size.w, size.h, 1},
                mipmap);
        }

        //        bucket.surface->upload(
        //            img.image.fmt,
        //            size_3d<i32>{size.w, size.h, 1},
        //            img.image.mip->data(magic, mipmap),
        //            ec,
        //            Point3(
        //                tex_offset.x() + mip_pad,
        //                tex_offset.y() + mip_pad,
        //                img.image.layer),
        //            mipmap);

        if(mip_pad == 0)
            return;

        /*
         * Image padding:
         *
         * We insert the pixels marked as X from the other side of the image.
         * 4x4 blocks are the basis for padding because DXT* uses it
         *
         *        XXXXXXXXXXXXXXXXXX
         *       X|----------------|X
         *       X|                |X
         *       X|                |X
         *       X|                |X
         *       X|                |X
         *       X|                |X
         *       X|                |X
         *       X|                |X
         *       X|----------------|X
         *        XXXXXXXXXXXXXXXXXX
         *
         */

        //        texture.upload(
        //            img.image.mip->data(magic, mipmap),
        //            point_3d<i32>(
        //                tex_offset.x() + mip_pad,
        //                tex_offset.x() + mip_pad,
        //                img.image.layer),
        //            size_3d<u32>{size.w, 4, 1},
        //            mipmap);
        //        bucket.surface->upload(
        //            img.image.fmt,
        //            size_3d<i32>{size.w, 4, 1},
        //            *img.image.mip->data(magic, mipmap)
        //                 .at(PixDescSize(img.image.fmt, {size.w, size.h - 4}),
        //                     PixDescSize(img.image.fmt, {size.w, 4})),
        //            ec,
        //            Point3(
        //                tex_offset.x() + mip_pad,
        //                tex_offset.y() + mip_pad - 4,
        //                img.image.layer),
        //            mipmap);

        //        bucket.surface->upload(
        //            img.image.fmt,
        //            size_3d<i32>{size.w, 4, 1},
        //            *img.image.mip->data(magic, mipmap)
        //                 .at(0, PixDescSize(img.image.fmt, {size.w, 4})),
        //            ec,
        //            Point3(
        //                tex_offset.x() + mip_pad,
        //                tex_offset.y() + size.h + mip_pad,
        //                img.image.layer),
        //            mipmap);

        /* TODO: Fix vertical padding */

        //        bucket.surface->upload(
        //            img.image.fmt,
        //            size_3d<i32>(4, size.h, 1),
        //            *img.image.mip->data(magic, mipmap)
        //                 .at_lazy(
        //                     PixDescSize(img.image.fmt, {size.w - 4, 1}),
        //                     PixDescSize(img.image.fmt, size)),
        //            ec,
        //            Point3(
        //                tex_offset.x() + mip_pad - 4,
        //                tex_offset.y() + mip_pad,
        //                img.image.layer),
        //            mipmap,
        //            size.w);

        //        bucket.surface->upload(
        //            img.image.fmt,
        //            size_3d<i32>(4, size.h, 1),
        //            *img.image.mip->data(magic, mipmap)
        //                 .at_lazy(0, PixDescSize(img.image.fmt, size)),
        //            ec,
        //            Point3(
        //                tex_offset.x() + size.w + mip_pad,
        //                tex_offset.y() + mip_pad,
        //                img.image.layer),
        //            mipmap,
        //            size.w);

        /* TODO: Add corner padding */
    }

    template<typename BucketType>
    void commit_bitmap(BitmapItem& img)
    {
        auto _ = allocator->debug().scope(img.tag->to_name().to_string(magic));

        auto& bucket
            = get_bucket<BucketType>(img.image.fmt, img.image.mip->type);

        C_UNUSED(auto name) = img.tag->to_name().to_string(magic);

        auto bmagic = img.image.mip->shared() ? bitm_magic : magic.no_magic();

        for(auto i : Range<u16>(img.image.mip->mipmaps))
        {
            if((img.image.mip->isize.w >> i) < 4)
                break;
            auto offset
                = point_2d<i32>(img.image.offset.x(), img.image.offset.y());
            if(offset.x % 4 != 0 || offset.y % 4 != 0)
                break;

            upload_mipmap<BucketType>(bucket, img, bmagic, i);
        }

        img.image.bias = math::min<f32>(
            C_CAST<f32>(img.image.mip->mipmaps) - bucket.surface->m_mipmaps, 0);

        /* Lightmaps do not use mipmapping */
        if(img.image.mip->mipmaps == 0)
            upload_mipmap<BucketType>(bucket, img, bmagic, 0);
    }

    void allocate_storage()
    {
        using size_bucket = Tup<u32, u32>;
        struct pool_size
        {
            u32          num    = 0;
            u32          layers = 0;
            size_2d<i32> max    = {};

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
            auto&       pool    = fmt_count[hash];
            auto const& imsize  = bitm.second.image.mip->isize;
            auto&       surface = tex_buckets[bitm.second.image.bucket].surface;

            u32 mipmaps = surface->m_mipmaps;
            u32 pad     = 0;
            //                = surface->m_format.pixfmt != PixFmt::RGB565 ? 4
            //                << mipmaps : 0;

            pool.num++;
            pool.max.w = std::max<u32>(pool.max.w, imsize.w + pad);
            pool.max.h = std::max<u32>(pool.max.h, imsize.h + pad);
            pool.images.insert(
                {bitm.first, std::make_tuple(imsize.w + pad, imsize.h + pad)});
        }

        /*
         * Generate metadata for use in shaders, like layer, scale and offset
         * This requires knowledge of the size of the pool and number of layers
         * in array textures
         */
        for(auto& pool_ : fmt_count)
        {
            auto& pool = pool_.second;

            size_2d<i32> offset = {0, 0};

            auto& surface = tex_buckets[pool_.first].surface;

            u32 layer   = 0;
            u32 mipmaps = surface->m_mipmaps;
            u32 max_pad = 0; // surface->m_format.pixfmt != PixFmt::RGB565 ? 4
                             // << mipmaps : 0;

            for(auto [id, fmt] : pool.images)
            {
                auto* img    = &m_cache.find(id)->second;
                auto  imsize = img->image.mip->isize;

                imsize.w += max_pad;
                imsize.h += max_pad;

                auto img_offset = offset;
                auto img_layer  = layer;

                [[maybe_unused]] size_2d<i32> slack = {
                    pool.max.w - offset.w,
                    pool.max.h - offset.h,
                };

                bool fits_width = (offset.w + imsize.w) <= pool.max.w;
                //                i32  height_diff = offset.h - imsize.h;

                //                if(height_diff < 0)
                //                    offset.h += -height_diff;

                if(fits_width)
                {
                    img_offset.w = offset.w;
                    img_offset.h = offset.h /*- imsize.h*/;
                    offset.w += imsize.w;
                } else
                {
                    layer++;
                    offset.w   = imsize.w;
                    img_offset = {};
                    img_layer  = layer;
                }

                img->image.layer  = layer;
                img->image.offset = {
                    C_CAST<f32>(img_offset.w + max_pad / 2),
                    C_CAST<f32>(img_offset.h + max_pad / 2),
                };
                img->image.scale = {
                    C_CAST<f32>(imsize.w - max_pad),
                    C_CAST<f32>(imsize.h - max_pad),
                };

                //                img->image.scale.x() /= pool.max.w;
                //                img->image.scale.y() /= pool.max.h;
                //                img->image.offset.x() /= pool.max.w;
                //                img->image.offset.y() /= pool.max.h;

                cDebug(
                    "{0}x{1} - {2}x{3} @ {4} ({5},{6}) padding of {7}",
                    img_offset.w,
                    img_offset.h,
                    offset.w,
                    offset.h,
                    img_layer,
                    imsize.w,
                    imsize.h,
                    max_pad);
            }

            pool.layers = layer + 1;
        }

        /* Allocate the surfaces */
        for(auto& bucket : tex_buckets)
        {
            auto&        props  = bucket.second;
            auto&        pool   = fmt_count[bucket.first];
            i32          layers = C_CAST<i32>(pool.layers);
            size_3d<i32> size{pool.max.w, pool.max.h, layers};
            props.surface->alloc(size.convert<u32>());
        }

        /* Commit the textures */
        for(auto& bitm : m_cache)
            commit_bitmap<gfx::texture_2da_t>(bitm.second);

        for(auto& [_, pool] : fmt_count)
            for(auto [image_id, fmt] : pool.images)
            {
                auto  image_it = m_cache.find(image_id);
                auto* image    = &image_it->second;
                image->image.offset.x() /= pool.max.w;
                image->image.offset.y() /= pool.max.h;
                image->image.scale.x() /= pool.max.w;
                image->image.scale.y() /= pool.max.h;
            }
    }

    virtual BitmapItem predict_impl(
        blam::tagref_t const& shader, i16 idx) override
    {
        auto _ = allocator->debug().scope("BitmapCache");

        using blam::tag_class_t;

        if(!shader.valid())
            return {};

        auto it          = index.find(shader);
        auto shader_name = shader.to_name().to_string(magic);

        if(it == index.end())
        {
            cDebug(
                "Failed to find shader: {1} {0}",
                shader_name,
                shader.tag.str());
            return {};
        }

        blam::bitm::header_t const* bitm_ptr = nullptr;

        auto               curr_magic = magic;
        blam::tag_t const* bitm_tag   = nullptr;

        //        Vecf2 prescale = {1, 1};

        switch(shader.tag_class)
        {
        case tag_class_t::bitm: {
            bitm_tag = &(*it);

            break;
        }
        default:
            Throw(undefined_behavior("invalid tag for bitmap cache"));
        }

        if(!bitm_tag)
        {
            cDebug(
                "Failed to get bitmap from: {0} {1}",
                shader.tag.str(),
                shader.to_name().to_string(magic));
            return {};
            Throw(undefined_behavior("failed to get bitmap tag"));
        }

        if(bitm_tag->storage == blam::image_storage_t::external)
        {
            bitm_ptr = bitm_header->get_block(bitm_tag->offset)
                           .data(bitm_magic, blam::single_value)
                           .value();
            curr_magic = bitm_header->block_magic(bitm_magic, bitm_tag->offset);
        } else
            bitm_ptr
                = bitm_tag->template data<blam::bitm::header_t>(magic).value();

        if(!bitm_ptr)
            Throw(undefined_behavior("failed to get bitmap header"));

        auto const& bitm = *bitm_ptr;

        BitmapItem out;
        out.header     = &bitm;
        out.tag        = bitm_tag;
        out.shader_tag = &(*it);

        if(auto image_ = bitm.images.data(curr_magic); image_.has_value())
        {
            auto& im = image_.value();
            if(im.size() == 0)
                return {};
            //            if(im[0].type != blam::bitm::type_t::tex_2d)
            //            {
            //                cDebug(
            //                    "unimplemented texture type: {0}",
            //#if C_HAS_INCLUDE(<string_view>)
            //                    magic_enum::enum_name(im[0].type)
            //#else
            //                    "[unknown]"
            //#endif
            //                );
            //                return {};
            //            }

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
                auto& bucket
                    = get_bucket<gfx::texture_2da_t>(fmt, img.mip->type);
                img.layer = bucket.ptr++;
                break;
            }
            case blam::bitm::type_t::tex_cube: {
                auto& bucket
                    = get_bucket<gfx::texture_cube_array_t>(fmt, img.mip->type);
                img.layer = bucket.ptr++;
                break;
            }
            default:
                cDebug(
                    "unimplemented texture type: {0}",
#if C_HAS_INCLUDE(<string_view>)
                    magic_enum::enum_name(im[0].type)
#else
                    "[unknown]"
#endif
                );
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
    virtual Tup<u32, i16> get_id(blam::tagref_t const& tag, i16 idx) override
    {
        return std::make_tuple(tag.tag_id, idx);
    }

    generation_idx_t resolve(blam::tagref_t const& array, i16 idx = 0)
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

using ShaderManifest
    = Components::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename V>
struct ShaderCache
    : DataCache<ShaderItem, u32, blam::tagref_t const&>,
      Components::RestrictedSubsystem<ShaderCache<V>, ShaderManifest>
{
    using type  = ShaderCache<V>;
    using Proxy = Components::proxy_of<ShaderManifest>;

    ShaderCache(BitmapCache<V>& bitmap_cache) : bitm_cache(bitmap_cache)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        index = blam::tag_index_view(map);
        magic = map.magic;
        evict_all();
    }

    BitmapCache<V>&         bitm_cache;
    blam::tag_index_view<V> index;
    blam::magic_data_t      magic;

    template<blam::tag_class_t Tag>
    generation_idx_t get_bitm_idx(blam::tagref_typed_t<Tag> const& bitm)
    {
        if(!bitm.valid())
            return generation_idx_t();

        return bitm_cache.predict(bitm.to_plain(), 0);
    }

    template<typename T>
    T const* extract_shader(typename blam::tag_index_view<V>::iterator it)
    {
        return (*it).template data<T>(magic).value();
    }

    ShaderItem predict_impl(blam::tagref_t const& shader)
    {
        auto _ = bitm_cache.allocator->debug().scope("ShaderCache");

        using blam::tag_class_t;
        using namespace blam::shader;

        if(!shader.valid())
            return {};

        auto it                    = index.find(shader);
        C_UNUSED(auto shader_name) = shader.to_name().to_string(magic);

        if(it == index.end())
            return {};

        ShaderItem out = {};
        out.tag        = &(*it);
        out.header     = extract_shader<blam::shader::radiosity_properties>(it);

        switch(shader.tag_class)
        {
        case tag_class_t::senv: {
            auto const& shader_model = *extract_shader<shader_env>(it);

            out.senv.base_bitm = get_bitm_idx(shader_model.diffuse.base);
            out.senv.primary_bitm
                = get_bitm_idx(shader_model.diffuse.primary.map);
            out.senv.secondary_bitm
                = get_bitm_idx(shader_model.diffuse.secondary.map);
            out.senv.micro_bitm = get_bitm_idx(shader_model.diffuse.micro.map);

            out.senv.self_illum = get_bitm_idx(shader_model.self_illum.map.map);
            out.senv.bump       = get_bitm_idx(shader_model.bump.map);
            out.senv.reflection_bitm
                = get_bitm_idx(shader_model.reflection.reflection);

            break;
        }
        case tag_class_t::soso: {
            auto const& shader_model
                = *extract_shader<blam::shader::shader_model>(it);

            out.soso.base_bitm   = get_bitm_idx(shader_model.maps.base);
            out.soso.multi_bitm  = get_bitm_idx(shader_model.maps.multipurpose);
            out.soso.detail_bitm = get_bitm_idx(shader_model.maps.detail.map);

            break;
        }
        case tag_class_t::schi: {
            auto const& shader_model = *extract_shader<shader_chicago<V>>(it);

            if(auto maps = shader_model.maps.data(magic); maps.has_value())
            {
                u8 i = 0;
                for(chicago::map_t const& map : maps.value())
                    out.schi.maps.at(i++) = get_bitm_idx(map.map.map);
            }
            if(auto layers = shader_model.layers.data(magic);
               layers.has_value())
            {
                u8 i = 0;
                for(auto const& layer : layers.value())
                    out.schi.layers.at(i++) = predict(layer.to_plain());
            }

            break;
        }
        case tag_class_t::scex: {
            auto const& shader_model
                = *extract_shader<shader_chicago_extended<V>>(it);

            if(auto maps4 = shader_model.maps_4stage.data(magic);
               maps4.has_value())
            {
                u8 i = 0;
                for(chicago::map_t const& map : maps4.value())
                    out.scex.maps.at(i++) = get_bitm_idx(map.map.map);
            }
            if(auto layers = shader_model.layers.data(magic);
               layers.has_value())
            {
                u8 i = 0;
                for(auto const& layer : layers.value())
                    out.scex.layers.at(i++) = predict(layer.to_plain());
            }

            break;
        }
        case tag_class_t::sgla: {
            shader_glass const& shader_model
                = *extract_shader<shader_glass>(it);

            out.color_bitm = get_bitm_idx(shader_model.diffuse.map.map);

            break;
        }
        case tag_class_t::swat: {
            shader_water<V> const& shader_model
                = *extract_shader<shader_water<V>>(it);

            out.swat.base       = get_bitm_idx(shader_model.base);
            out.swat.reflection = get_bitm_idx(shader_model.reflection_map);

            break;
        }
        case tag_class_t::spla: {
            auto const& shader_model = *extract_shader<shader_plasma>(it);

            out.color_bitm = get_bitm_idx(shader_model.primary_noise.noise.map);

            break;
        }
        case tag_class_t::smet: {
            auto const& shader_model = *extract_shader<shader_meter>(it);

            out.color_bitm = get_bitm_idx(shader_model.map);

            break;
        }
        case tag_class_t::sotr: {
            blam::shader::shader_transparent<V> const& shader_model
                = *extract_shader<blam::shader::shader_transparent<V>>(it);

            if(auto maps = shader_model.maps.data(magic); maps.has_value())
            {
                for(chicago::map_t const& map : maps.value())
                    out.color_bitm = get_bitm_idx(map.map.map);
            }

            break;
        }
        default:
            Throw(undefined_behavior("unhandled shader type"));
        }

        if(!out.color_bitm.valid())
            out.header = nullptr;

        return out;
    }

    void populate_material(
        materials::senv_micro& mat, generation_idx_t const& shader_id)
    {
        using blam::tag_class_t;

        ShaderItem const& shader = find(shader_id)->second;

        mat = {};

        switch(shader.tag->tag_class())
        {
        case tag_class_t::scex: {
            mat.lightmap.material = materials::id::scex;
            break;
        }
        case tag_class_t::schi: {
            blam::shader::shader_chicago<V> const* info
                = shader.header->as<blam::shader::shader_chicago<V>>();
            auto& base = *bitm_cache.assign_atlas_data(
                mat.base, shader.schi.maps.at(0));
            mat.base.uv_scale = {1};
            mat.base.bias     = base.image.bias;

            mat.lightmap.material = materials::id::schi;
            break;
        }
        case tag_class_t::senv: {
            auto const* info = shader.header->as<blam::shader::shader_env>();

            auto& base = *bitm_cache.assign_atlas_data(
                mat.base, shader.senv.base_bitm);
            mat.base.uv_scale = {1};
            mat.base.bias     = base.image.bias;

            auto* primary = bitm_cache.assign_atlas_data(
                mat.primary, shader.senv.primary_bitm);
            if(primary)
            {
                mat.primary.uv_scale = {info->diffuse.primary.scale};
                mat.primary.bias     = primary->image.bias;
            }

            auto* secondary = bitm_cache.assign_atlas_data(
                mat.secondary, shader.senv.secondary_bitm);
            if(secondary)
            {
                mat.secondary.uv_scale = {info->diffuse.secondary.scale};
                mat.secondary.bias     = secondary->image.bias;
            }

            auto* micro = bitm_cache.assign_atlas_data(
                mat.micro, shader.senv.micro_bitm);
            mat.micro.uv_scale = {info->diffuse.micro.scale};
            if(micro)
                mat.micro.bias = micro->image.bias;

            mat.lightmap.material = materials::id::senv;
            break;
        }
        case tag_class_t::swat: {
            //            auto const* info =
            //            shader.header->as<blam::shader::shader_glass>();
            bitm_cache.assign_atlas_data(mat.base, shader.color_bitm);
            mat.base.uv_scale = {1};
            mat.base.bias     = 0;

            mat.lightmap.material = materials::id::swat;
            break;
        }
        case tag_class_t::sgla: {
            //            auto const* info =
            //            shader.header->as<blam::shader::shader_glass>();
            bitm_cache.assign_atlas_data(mat.base, shader.color_bitm);
            mat.base.uv_scale = {1};
            mat.base.bias     = 0;

            mat.lightmap.material = materials::id::sgla;
            break;
        }
        case tag_class_t::smet: {
            mat.lightmap.material = materials::id::smet;
            break;
        }
        case tag_class_t::sotr: {
            bitm_cache.assign_atlas_data(mat.base, shader.color_bitm);
            mat.base.uv_scale = {1};
            mat.base.bias     = 0;

            mat.lightmap.material = materials::id::sotr;
            break;
        }
        case tag_class_t::soso: {
            bitm_cache.assign_atlas_data(mat.base, shader.color_bitm);
            mat.base.uv_scale = {1};
            mat.base.bias = 0;

            mat.lightmap.material = materials::id::soso;
            break;
        }
        default: {
            cDebug(
                "Material not set up: {0}/{1}",
                shader.tag->tagclass.at(0).str(),
                shader.tag->to_name().to_string(magic));
            break;
        }
        }
    }

    u32 get_id(blam::tagref_t const& args)
    {
        return args.tag_id;
    }

    void start_restricted(Proxy&, time_point const&)
    {
    }
    void end_restricted(Proxy&, time_point const&)
    {
    }
};

using ModelManifest
    = Components::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename V>
struct ModelCache
    : DataCache<ModelItem<V>, Tup<u32, u16>, blam::tagref_t const&, u16>,
      Components::RestrictedSubsystem<ModelCache<V>, ModelManifest>
{
    using type    = ModelCache<V>;
    using Variant = typename std::conditional_t<
        std::is_same_v<V, blam::xbox_version_t>,
        blam::xbox_t,
        blam::grbx_t>;
    using Proxy = Components::proxy_of<ModelManifest>;

    ModelCache(
        BitmapCache<V>& bitm_cache,
        ShaderCache<V>& shader_cache,
        gfx_api*        allocator) :
        bitm_cache(bitm_cache),
        shader_cache(shader_cache), allocator(allocator)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        version      = V::version_v;
        tags         = map.tags;
        index        = blam::tag_index_view(map);
        magic        = map.magic;
        vertex_magic = V::version_v == blam::version_t::xbox
                           ? map.magic
                           : tags->vertex_magic(magic);
        vert_ptr = 0, element_ptr = 0;
        this->evict_all();
    }

    blam::version_t             version;
    blam::tag_index_t<V> const* tags;
    blam::tag_index_view<V>     index;
    blam::magic_data_t          magic;
    blam::magic_data_t          vertex_magic;
    BitmapCache<V>&             bitm_cache;
    ShaderCache<V>&             shader_cache;
    gfx_api*                    allocator;

    Bytes vert_buffer, element_buffer;
    u32   vert_ptr, element_ptr;

    blam::mod2::header<V> const* get_header(blam::tagref_t const& mod2)
    {
        auto header_it = index.find(mod2);

        if(header_it == index.end())
            return nullptr;

        auto header
            = (*header_it).template data<blam::mod2::header<V>>(magic).value();

        return &header[0];
    }

    inline auto vertex_data(blam::mod2::submesh_header const& model)
    {
        if constexpr(std::is_same_v<V, blam::xbox_version_t>)
        {
            auto const& vertex_ref = model.xbox_vertex_ref()
                                         .data(magic, blam::single_value)
                                         .value();
            return vertex_ref->vertices(model.vert_count)
                .data(vertex_magic)
                .value();
        } else
            return model.vertex_segment(*tags).data(vertex_magic).value();
    }
    inline auto index_data(blam::mod2::submesh_header const& model)
    {
        if constexpr(std::is_same_v<V, blam::xbox_version_t>)
            return model.xbox_index_segment().data(vertex_magic).value();
        else
            return model.index_segment(*tags).data(vertex_magic).value();
    }

    virtual ModelItem<V> predict_impl(
        blam::tagref_t const& mod2, u16 geom_idx) override
    {
        auto _ = allocator->debug().scope("ModelCache");

        using namespace blam::mod2;

        auto const* header = get_header(mod2);

        if(!header)
            return {};

        ModelItem<V> out;
        out.mesh    = {};
        out.header  = header;
        out.tag     = &(*index.find(mod2));
        out.uvscale = Vecf2(header->uvscale.u, header->uvscale.v);

        auto const& shaders_ = header->shaders.data(magic);
        if(shaders_.has_error())
            Throw(undefined_behavior("no shaders found"));
        auto const& shaders = shaders_.value();

        auto const& geom = header[0].geometries.data(magic).value()[geom_idx];
        {
            out.mesh.header = &geom;

            for(auto const& model : geom.meshes(magic))
            {
                auto elements = index_data(model.data);
                auto vertices = vertex_data(model.data);

                using element_type = typename std::remove_const<
                    typename decltype(elements)::value_type>::type;
                using vertex_type = typename std::remove_const<
                    typename decltype(vertices)::value_type>::type;

                if(elements.empty() || vertices.empty())
                    Throw(undefined_behavior(
                        "failed to locate element or vertex data"));

                out.mesh.sub.emplace_back();
                auto& draw_data         = out.mesh.sub.back();
                draw_data.header        = &model.data;
                draw_data.draw.elements = {
                    .count         = static_cast<u32>(elements.size()),
                    .offset        = element_ptr,
                    .vertex_offset = vert_ptr / sizeof(vertex_type),
                    .type          = semantic::TypeEnum::UShort,
                };
                draw_data.draw.instances.count = 1;
                //                    = {.elements = {
                //                           .count         = elements.size(),
                //                           .offset        = element_ptr /
                //                           sizeof(element_type),
                //                           .vertex_offset = vert_ptr /
                //                           sizeof(vertex_type), .type =
                //                           semantic::TypeEnum::UShort,
                //                       },
                //                       .instances = {
                //                           .count = 1,
                //                       }};
                draw_data.shader
                    = shader_cache.predict(shaders[model.data.shader_idx].ref);

                auto vert_dest
                    = (*vert_buffer.at(vert_ptr)).template as<vertex_type>();
                auto element_dest = (*element_buffer.at(element_ptr))
                                        .template as<element_type>();

                std::copy(vertices.begin(), vertices.end(), vert_dest.begin());
                std::copy(
                    elements.begin(), elements.end(), element_dest.begin());

                //                Array<u16, 2> last_indices = {{elements[0],
                //                elements[1]}};

                vert_ptr += vertices.size_bytes();
                element_ptr += elements.size_bytes();
            }
        }

        return out;
    }

    virtual void evict_impl() override
    {
        vert_ptr    = 0;
        element_ptr = 0;
    }
    virtual Tup<u32, u16> get_id(
        blam::tagref_t const& tag, u16 geom_idx) override
    {
        return std::make_tuple(tag.tag_id, geom_idx);
    }

    ModelAssembly predict_regions(
        blam::tagref_t const& tag, u16 max_lod = blam::mod2::lod_high_ext)
    {
        ModelAssembly assem  = {};
        auto          header = get_header(tag);

        if(!header)
            return {};

        for(auto const& region : header->regions.data(magic).value())
        {
            assem.models.emplace_back();
            auto& mod = assem.models.back();
            for(auto const& perm : region.permutations.data(magic).value())
            {
                auto const& lod = perm.meshindex_lod.at(max_lod);
                mod.at(max_lod) = this->predict(tag, lod);
            }
        }

        return assem;
    }

    void start_restricted(Proxy&, time_point const&)
    {
    }
    void end_restricted(Proxy&, time_point const&)
    {
    }
};

using BSPManifest
    = Components::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename V>
struct BSPCache
    : DataCache<BSPItem, blam::bsp::info const*, blam::bsp::info const&>,
      Components::RestrictedSubsystem<BSPCache<V>, BSPManifest>
{
    using type  = BSPCache<V>;
    using Proxy = Components::proxy_of<BSPManifest>;

    BSPCache(
        BitmapCache<V>& bitm_cache,
        ShaderCache<V>& shader_cache,
        ModelCache<V>&  model_cache) :
        version(V::version_v),
        bitm_cache(bitm_cache), shader_cache(shader_cache),
        model_cache(model_cache)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        index    = blam::tag_index_view(map);
        magic    = map.magic;
        vert_ptr = 0, element_ptr = 0, light_ptr = 0, portal_ptr = 0;
        evict_all();
    }

    blam::version_t         version;
    BitmapCache<V>&         bitm_cache;
    ShaderCache<V>&         shader_cache;
    ModelCache<V>&          model_cache;
    blam::tag_index_view<V> index;
    blam::magic_data_t      magic;

    Bytes                      vert_buffer, element_buffer, light_buffer;
    semantic::mem_chunk<Vecf3> portal_buffer;
    u32                        vert_ptr, element_ptr, light_ptr, portal_ptr;

    virtual BSPItem predict_impl(blam::bsp::info const& bsp) override
    {
        //        GFX::DBG::SCOPE _("BSPCache");

        auto        bsp_magic = bsp.bsp_magic(magic);
        auto const& section_  = bsp.to_bsp(bsp_magic);
        auto const& section
            = *section_.to_header().data(bsp_magic, blam::single_value).value();

        BSPItem out;
        out.mesh = &section;
        out.tag  = &(*index.find(bsp.tag));

        if(!out.tag->valid())
            return {};

        //        auto shader = section.shaders.data(bsp_magic);

        for(auto const& portal :
            section.cluster_portals.data(bsp_magic).value())
        {
            auto vertices = portal.vertices.data(bsp_magic).value();
            std::copy(
                vertices.begin(),
                vertices.end(),
                portal_buffer.begin() + portal_ptr);
            out.portals.push_back({
                .arrays = {
                     .count  = static_cast<u32>(vertices.size()),
                     .offset = static_cast<u32>(portal_ptr),
                },
            });
            portal_ptr += vertices.size();
        }

        for(auto const& group : section.submesh_groups.data(bsp_magic).value())
        {
            out.groups.emplace_back();
            auto& group_data = out.groups.back();
            group_data.group = &group;
            for(auto const& mesh : group.material.data(bsp_magic).value())
            {
                group_data.meshes.emplace_back();
                auto& mesh_data = group_data.meshes.back();

                /* Just dig up the textures, long process */
                mesh_data.shader = shader_cache.predict(mesh.shader);
                if(group.lightmap_idx != -1)
                    mesh_data.light_bitm = bitm_cache.resolve(
                        section.lightmaps, group.lightmap_idx);

                /* ... and moving on */

                if(version == blam::version_t::xbox)
                {
                    auto indices
                        = mesh.indices(section).data(bsp_magic).value();
                    auto vertices
                        = mesh.xbox_vertices().data(bsp_magic).value();
                    auto light_verts
                        = mesh.xbox_light_verts().data(bsp_magic).value();

                    if(vertices.empty() || indices.empty())
                    {
                        group_data.meshes.erase(--group_data.meshes.end());
                        continue;
                    }

                    using vertex_type = typename std::remove_const<
                        typename decltype(vertices)::value_type>::type;
                    using light_type = typename std::remove_const<
                        typename decltype(light_verts)::value_type>::type;
                    using element_type = typename std::remove_const<
                        typename decltype(indices)::value_type>::type;

                    mesh_data.mesh          = &mesh;
                    mesh_data.draw.elements = {
                        .count         = C_FCAST<u32>(indices.size()),
                        .offset        = element_ptr,
                        .vertex_offset = vert_ptr / sizeof(vertex_type),
                        .type          = semantic::TypeEnum::UShort,
                    };
                    mesh_data.draw.instances.count = 1;

                    MemCpy(
                        vertices,
                        (*vert_buffer.at(vert_ptr)).template as<vertex_type>());
                    MemCpy(
                        indices,
                        (*element_buffer.at(element_ptr))
                            .template as<element_type>());
                    MemCpy(
                        light_verts,
                        (*light_buffer.at(light_ptr))
                            .template as<light_type>());

                    vert_ptr += vertices.size_bytes();
                    element_ptr += indices.size_bytes();
                    light_ptr += light_verts.size_bytes();
                } else
                {
                    auto indices
                        = mesh.indices(section).data(bsp_magic).value();
                    auto vertices = mesh.pc_vertices().data(bsp_magic).value();
                    auto light_verts
                        = mesh.pc_light_verts().data(bsp_magic).value();

                    using vertex_type = typename std::remove_const<
                        typename decltype(vertices)::value_type>::type;
                    using light_type = typename std::remove_const<
                        typename decltype(light_verts)::value_type>::type;
                    using element_type = typename std::remove_const<
                        typename decltype(indices)::value_type>::type;

                    mesh_data.mesh          = &mesh;
                    mesh_data.draw.elements = {
                        .count         = C_FCAST<u32>(indices.size()),
                        .offset        = element_ptr,
                        .vertex_offset = vert_ptr / sizeof(vertex_type),
                        .type          = semantic::TypeEnum::UShort,
                    };
                    mesh_data.draw.instances.count = 1;

                    MemCpy(
                        vertices,
                        (*vert_buffer.at(vert_ptr)).template as<vertex_type>());
                    MemCpy(
                        indices,
                        (*element_buffer.at(element_ptr))
                            .template as<element_type>());
                    MemCpy(
                        light_verts,
                        (*light_buffer.at(light_ptr))
                            .template as<light_type>());

                    vert_ptr += vertices.size_bytes();
                    element_ptr += indices.size_bytes();
                    light_ptr += light_verts.size_bytes();
                }
            }
        }

        return out;
    }
    virtual blam::bsp::info const* get_id(blam::bsp::info const& bsp) override
    {
        return &bsp;
    }

    void start_restricted(Proxy&, time_point const&)
    {
    }
    void end_restricted(Proxy&, time_point const&)
    {
    }
};
