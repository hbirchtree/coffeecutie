#pragma once

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_font.h>

#include <coffee/comp_app/AppWrap>
#include <coffee/comp_app/gl_config.h>
#include <coffee/core/CApplication>
#include <coffee/core/CArgParser>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/EventHandlers>
#include <coffee/core/Scene>

#include <peripherals/semantic/chunk_ops.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_compat.h>

#include <fmt_extensions/vector_types.h>

#include <coffee/core/CDebug>

#include "blam_files.h"
#include "data.h"
#include "data_cache.h"
#include "materials.h"

using gfx_api = gleam::api;
using semantic::Bytes;
using semantic::BytesConst;
using semantic::Span;
using stl_types::Range;
using type_safety::empty_list_t;
using type_safety::type_list_t;
using typing::vector_types::Matf3;
using typing::vector_types::Matf4;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;
using typing::vector_types::Veci2;
using typing::vector_types::Veci3;
using typing::vector_types::Vecui2;
using typing::vector_types::Vecui3;

namespace gfx {

using namespace gleam;

}

using namespace Coffee;

using typing::PixCmp;
using typing::pixels::BitFmt;
using typing::pixels::CompFlags;
using typing::pixels::PixDesc;
using typing::pixels::PixFmt;

using bitm_format_hash
    = std::tuple<blam::bitm::type_t, PixFmt, PixCmp, BitFmt, CompFlags>;

using Components::time_point;

struct BSPItem
{
    struct Mesh
    {
        blam::bsp::material const*  mesh{nullptr};
        gleam::draw_command::data_t draw;
        generation_idx_t            light_bitm;
        generation_idx_t            shader;
    };
    struct Group
    {
        blam::bsp::lightmap const* group{nullptr};
        std::vector<Mesh>          meshes;
    };
    struct Subcluster
    {
        blam::bsp::subcluster const* cluster{nullptr};
        Span<u32 const>              indices;
        u32                          debug_color_idx{0};
    };
    struct Cluster
    {
        blam::bsp::cluster const*                     cluster{nullptr};
        std::vector<Subcluster>                       sub;
        std::vector<blam::bsp::cluster_portal const*> portals;
    };

    blam::bsp::header const*                 mesh{nullptr};
    blam::tag_t const*                       tag{nullptr};
    std::vector<Group>                       groups;
    std::vector<Cluster>                     clusters;
    std::vector<gleam::draw_command::data_t> portals;
    std::vector<u32>                         portal_color_ptrs;

    inline bool valid() const
    {
        return mesh;
    }

    inline std::optional<std::pair<u32, u32>> find_cluster(
        Vecf3 const& point) const
    {
        u32 cluster_id = 0;
        for(auto const& cluster : clusters)
        {
            u32 sub_id = 0;
            for(auto const& sub : cluster.sub)
            {
                if(!sub.cluster->bounds.contains(point))
                {
                    sub_id++;
                    continue;
                }
                return std::pair{cluster_id, sub_id};
            }
            cluster_id++;
        }
        return std::nullopt;
    }
};

template<typename V>
struct ModelItem
{
    struct SubModel
    {
        blam::mod2::part const*     header{nullptr};
        gleam::draw_command::data_t draw;
        generation_idx_t            shader;
    };
    struct LOD
    {
        blam::mod2::geometry_header<V> const* header{nullptr};
        std::vector<SubModel>                 sub;
    };

    blam::mod2::header<V> const* header{nullptr};
    blam::tag_t const*           tag{nullptr};
    LOD                          mesh;

    inline bool valid() const
    {
        return header;
    }
};

struct ModelAssembly
{
    blam::mod2::region const*                header;
    semantic::Span<blam::mod2::region const> regions;
    std::vector<generation_idx_t>            models;
};

struct BitmapItem
{
    blam::bitm::header_t const* header{nullptr};
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
    struct
    {
        u32 base{0};
        u32 last{5};
    } mipmaps;

    inline bool valid() const
    {
        return header;
    }
};

struct ShaderItem
{
    blam::shader::radiosity_properties const* header{nullptr};
    blam::tag_t const*                        tag{nullptr};
    blam::tag_class_t tag_class{blam::tag_class_t::undefined};

    inline blam::tag_class_t type()
    {
        if(!tag)
            Throw(undefined_behavior("invalid shader item"));
        return tag_class;
    }

    struct soso_t
    {
        generation_idx_t base_bitm;
        generation_idx_t multi_bitm;
        generation_idx_t detail_bitm;
        generation_idx_t reflection_bitm;
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
        generation_idx_t bump;
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

using BitmapManifest
    = Components::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename V>
struct BitmapCache
    : DataCache<BitmapItem, std::tuple<u32, i16>, blam::tagref_t const&, i16>,
      Components::RestrictedSubsystem<BitmapCache<V>, BitmapManifest>
{
    using type  = BitmapCache;
    using Proxy = Components::proxy_of<BitmapManifest>;

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

    BitmapCache(
        BlamFiles const*           files,
        gfx_api*                   allocator,
        RenderingParameters const* params) :
        files(files),
        allocator(allocator), params(params)
    {
    }

    inline void load_from(
        blam::map_container<V> const& map,
        blam::magic_data_t const&     bitmap_magic)
    {
        index = blam::tag_index_view(map);
        bitm_header
            = blam::bitm::bitmap_atlas_view::from_data(bitmap_magic.data());
        magic = map.magic;
        bitm_magic
            = map.map->version == blam::version_t::xbox ? magic : bitmap_magic;
        bitm_magic.magic_offset = 0;
        evict_all();
    }

    blam::tag_index_view<V>    index;
    BlamFiles const*           files;
    gfx_api*                   allocator;
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
//    template<typename T>
//    requires std::is_same_v<T, gfx::texture_3d_t>
//    auto bucket_to_type()
//    {
//        return gfx::textures::d3;
//    }
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
        Veci2 tex_offset
            = {(pool_offset[0] >> mipmap) - mip_pad,
               (pool_offset[1] >> mipmap) - mip_pad};

        (tex_offset[0] >>= 2) <<= 2;
        (tex_offset[1] >>= 2) <<= 2;

        mipmap += img.mipmaps.base;

#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION >= 0x320
        if(bucket.type == blam::bitm::type_t::tex_cube)
        {
            gfx::texture_cube_array_t& texture
                = bucket.template texture_as<gfx::texture_cube_array_t>();
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
            gfx::compat::texture_2da_t& texture
                = bucket.template texture_as<gfx::compat::texture_2da_t>();

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

        auto& bucket
            = get_bucket<BucketType>(img.image.fmt, img.image.mip->type);

        C_UNUSED(auto name) = img.tag->to_name().to_string(magic);

        auto bmagic = img.image.mip->shared() ? bitm_magic : magic.no_magic();

        for(auto i : Range<u16>(img.mipmaps.last - img.mipmaps.base))
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

                if(params->mipmap_bias > 0
                   && img->image.mip->mipmaps > params->mipmap_bias)
                {
                    imsize >>= params->mipmap_bias;
                    img->mipmaps.base = params->mipmap_bias;
                    img->mipmaps.last
                        = params->mipmap_bias
                          + std::min<i32>(
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
                auto img_layer  = layer;

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
                    img_layer  = layer;
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
            auto  size
                = size_3d<i32>{pool.max.x, pool.max.y, layers}.convert<u32>();
            props.surface->alloc(size);

            auto [type, fmt, _, __, comp] = bucket.first;
            std::string bucket_name       = Strings::fmt(
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

        auto [bitm_ptr, curr_magic]
            = bitm_tag->image(magic, bitm_header).value();

        if(!bitm_ptr)
            Throw(undefined_behavior("failed to get bitmap header"));

        auto const& bitm = *bitm_ptr;

        BitmapItem out;
        out.header = &bitm;
        out.tag    = bitm_tag;

        for(auto const& sequence : bitm.sequences.data(curr_magic).value())
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
                auto& bucket = get_bucket<gfx::compat::texture_2da_t>(
                    fmt, img.mip->type);
                img.layer = bucket.ptr++;
                break;
            }
#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION >= 0x320
            case blam::bitm::type_t::tex_cube: {
                if(!allocator->feature_info().texture.cube_array)
                    return {};
                auto& bucket
                    = get_bucket<gfx::texture_cube_array_t>(fmt, img.mip->type);
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
        out.tag_class  = shader.tag_class;
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

            //            out.senv.self_illum =
            //            get_bitm_idx(shader_model.self_illum.map.map);
            out.senv.bump = get_bitm_idx(shader_model.bump.map);
            out.senv.reflection_bitm
                = get_bitm_idx(shader_model.reflection.reflection);

            break;
        }
        case tag_class_t::soso: {
            blam::shader::shader_model const& shader_model
                = *extract_shader<blam::shader::shader_model>(it);

            out.soso.base_bitm   = get_bitm_idx(shader_model.maps.base);
            out.soso.multi_bitm  = get_bitm_idx(shader_model.maps.multipurpose);
            out.soso.detail_bitm = get_bitm_idx(shader_model.maps.detail.map);
            out.soso.reflection_bitm
                = get_bitm_idx(shader_model.reflection.reflection);

            break;
        }
        case tag_class_t::schi: {
            auto const& shader_model = *extract_shader<shader_chicago<V>>(it);

            if(auto maps = shader_model.maps.data(magic); maps.has_value())
            {
                u8 i = 0;
                for(chicago::map_t const& map : maps.value())
                {
                    out.schi.maps.at(i++) = get_bitm_idx(map.map.map);
                }
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
            shader_water const& shader_model
                = *extract_shader<shader_water>(it);

            out.swat.base       = get_bitm_idx(shader_model.base);
            out.swat.reflection = get_bitm_idx(shader_model.reflection_map);
            out.swat.bump       = get_bitm_idx(shader_model.ripple.map);

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
            shader_transparent const& shader_model
                = *extract_shader<shader_transparent>(it);

            if(auto maps = shader_model.maps.data(magic); maps.has_value())
            {
                for(shader_transparent::map_t const& map : maps.value())
                    out.color_bitm = get_bitm_idx(map.map.map);
            }

            break;
        }
        default:
            Throw(undefined_behavior("unhandled shader type"));
        }

        return out;
    }

    void populate_material(
        materials::senv_micro&  mat,
        generation_idx_t const& shader_id,
        Vecf2 const&            base_map_scale)
    {
        using blam::tag_class_t;
        using namespace blam::shader;

        ShaderItem const& shader = find(shader_id)->second;

        mat = {};

        switch(shader.tag_class)
        {
        case tag_class_t::scex: {
            shader_chicago_extended<V> const* info
                = shader.header->as<blam::shader::shader_chicago_extended<V>>();

            auto maps = info->maps_4stage.data(magic).value();
            for(auto i : Range<>(4))
            {
                auto id = shader.scex.maps.at(i);
                if(!shader.schi.maps.at(i).valid())
                    continue;
                BitmapItem const& bitm
                    = *bitm_cache.assign_atlas_data(mat.maps[i], id);
                chicago::map_t const& map = maps[i];
                mat.maps[i].uv_scale      = map.map.uv_scale * base_map_scale;
                mat.maps[i].bias          = bitm.image.bias;

                u16 flags = static_cast<u8>(map.color_func)
                            | (static_cast<u8>(map.alpha_func) << 4);
                mat.lightmap.meta1 |= flags << (i * 8);
            }

            mat.material.material = materials::id::scex;
            break;
        }
        case tag_class_t::schi: {
            shader_chicago<V> const* info
                = shader.header->as<blam::shader::shader_chicago<V>>();

            auto maps = info->maps.data(magic).value();
            for(auto i : Range<>(4))
            {
                auto id = shader.schi.maps.at(i);
                if(!shader.schi.maps.at(i).valid())
                    continue;
                BitmapItem const& bitm
                    = *bitm_cache.assign_atlas_data(mat.maps[i], id);
                chicago::map_t const& map = maps[i];
                mat.maps[i].uv_scale      = map.map.uv_scale * base_map_scale;
                mat.maps[i].bias          = bitm.image.bias;

                u16 flags = static_cast<u8>(map.color_func)
                            | (static_cast<u8>(map.alpha_func) << 4);
                mat.lightmap.meta1 |= flags << (i * 8);
            }

            mat.material.material = materials::id::schi;
            break;
        }
        case tag_class_t::senv: {
            shader_env const* info
                = shader.header->as<blam::shader::shader_env>();

            auto base = bitm_cache.assign_atlas_data(
                mat.maps[0], shader.senv.base_bitm);
            if(base)
            {
                mat.maps[0].uv_scale = Vecf2{1};
                mat.maps[0].bias     = base->image.bias;
            }

            auto* micro = bitm_cache.assign_atlas_data(
                mat.maps[1], shader.senv.micro_bitm);
            if(micro)
            {
                mat.maps[1].uv_scale = Vecf2(info->diffuse.micro.scale);
                mat.maps[1].bias     = micro->image.bias;
            }

            mat.material.flags |= (micro ? 1 : 0) << 10;

            auto* primary = bitm_cache.assign_atlas_data(
                mat.maps[2], shader.senv.primary_bitm);
            if(primary)
            {
                mat.maps[2].uv_scale = Vecf2(info->diffuse.primary.scale);
                mat.maps[2].bias     = primary->image.bias;
            }

            auto* secondary = bitm_cache.assign_atlas_data(
                mat.maps[3], shader.senv.secondary_bitm);
            if(secondary)
            {
                mat.maps[3].uv_scale = Vecf2(info->diffuse.secondary.scale);
                mat.maps[3].bias     = secondary->image.bias;
            }

            mat.material.flags |= (primary && secondary ? 1 : 0) << 9;

            mat.lightmap.meta1
                = bitm_cache.get_atlas_layer(shader.senv.self_illum);

            auto* bump
                = bitm_cache.assign_atlas_data(mat.maps[4], shader.senv.bump);
            if(bump)
            {
                mat.maps[4].uv_scale = Vecf2(info->bump.scale);
                mat.maps[4].bias     = bump->image.bias;
            }

            mat.material.material = materials::id::senv;
            mat.material.flags    = static_cast<u32>(info->flags)
                                 | static_cast<u32>(info->shader_type) << 4;
            mat.material.inputs1[0] = info->reflection.lightmap_brightness;
            mat.material.inputs1[1] = info->specular.brightness;

            auto& reflection = info->reflection;
            if(shader.senv.reflection_bitm.valid())
            {
                mat.material.inputs[0] = Vecf4(
                    info->specular.perpendicular_color,
                    reflection.perpendicular_brightness);
                mat.material.inputs[1] = Vecf4(
                    info->specular.parallel_color,
                    reflection.parallel_brightness);
                mat.lightmap.reflection
                    = bitm_cache.get_atlas_layer(shader.senv.reflection_bitm);
                mat.material.flags |= 1 << 6; /* Flag for reflection */
                mat.material.flags |= static_cast<u32>(reflection.type) << 7;
            }

            /* Allocation of flag bits:
             * 0-3:   Top-level flags (senv::flags)
             * 4-5:   Shader type
             *
             * Reflection properties:
             * 6:     Reflection toggle
             * 7-8:   Reflection map type
             *
             * Blending properties:
             * 9-10:  Detail map function
             * 10-11: Micro map function
             *
             * Texture scrolling animation:
             * 12-15: U-animation function
             * 16-19: V-animation function
             *
             *
             */

            break;
        }
        case tag_class_t::swat: {
            shader_water const* info = shader.header->as<shader_water>();
            bitm_cache.assign_atlas_data(mat.maps[0], shader.swat.base);
            mat.maps[0].uv_scale = Vecf2(1);
            mat.maps[0].bias     = 0;

            bitm_cache.assign_atlas_data(mat.maps[1], shader.swat.reflection);
            mat.lightmap.reflection = mat.maps[1].layer;

            bitm_cache.assign_atlas_data(mat.maps[1], shader.swat.bump);
            mat.maps[1].uv_scale = Vecf2{info->ripple.scale};
            mat.maps[1].bias     = 2.f;

            mat.material.material = materials::id::swat;
            mat.material.flags    = static_cast<u32>(info->flags);
            mat.material.inputs1  = Vecf2{
                glm::radians(info->ripple.anim_angle),
                info->ripple.anim_velocity};
            mat.material.inputs[0]
                = Vecf4(info->parallel.tint_color, info->parallel.brightness);
            mat.material.inputs[1] = Vecf4(
                info->perpendicular.tint_color, info->perpendicular.brightness);
            break;
        }
        case tag_class_t::sgla: {
            //            auto const* info =
            //            shader.header->as<blam::shader::shader_glass>();
            bitm_cache.assign_atlas_data(mat.maps[0], shader.color_bitm);
            mat.maps[0].uv_scale = Vecf2(1);
            mat.maps[0].bias     = 0;

            mat.material.material = materials::id::sgla;
            break;
        }
        case tag_class_t::smet: {
            mat.material.material = materials::id::smet;
            break;
        }
        case tag_class_t::sotr: {
            bitm_cache.assign_atlas_data(mat.maps[0], shader.color_bitm);
            mat.maps[0].uv_scale = Vecf2(1);
            mat.maps[0].bias     = 0;

            mat.material.material = materials::id::sotr;
            break;
        }
        case tag_class_t::soso: {
            shader_model const* info
                = shader.header->as<blam::shader::shader_model>();
            bitm_cache.assign_atlas_data(mat.maps[0], shader.soso.base_bitm);
            mat.maps[0].uv_scale = base_map_scale;
            mat.maps[0].bias     = 2;

            bitm_cache.assign_atlas_data(mat.maps[1], shader.soso.multi_bitm);
            mat.maps[1].uv_scale = Vecf2(1);
            mat.maps[1].bias     = 2;

            auto* detail = bitm_cache.assign_atlas_data(
                mat.maps[2], shader.soso.detail_bitm);
            if(detail)
            {
                mat.maps[2].uv_scale = Vecf2(info->maps.detail.scale);
                mat.maps[2].bias     = detail->image.bias;
            }

            mat.lightmap.reflection
                = bitm_cache.get_atlas_layer(shader.soso.reflection_bitm);
            if(mat.lightmap.reflection != 0)
                mat.material.flags |= 0x1;

            mat.material.material = materials::id::soso;
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

    template<typename PropertyAnim>
    requires stl_types::is_any_of<
        PropertyAnim,
        blam::shader::texture_property_anim,
        blam::shader::simple_tex_property_anim>
        //
        f32 tex_animation(PropertyAnim const& anim, f32 const& time)
    {
        using namespace blam::shader;
        switch(anim.function)
        {
        case animation_function::slide:
        case animation_function::slide_variable:
            return std::fmod(time, anim.period);
        case animation_function::cosine:
        case animation_function::cosine_variable:
            return glm::cos(time * anim.period + anim.phase) * anim.scale;
        default:
            return 0;
        }
    }

    template<typename Anim>
    requires stl_types::is_any_of<
        Anim,
        blam::shader::texture_uv_rotation_animation,
        blam::shader::simple_texture_uv_animation>
        //
        Vecf2 uv_animation(Anim const& anim, f32 const& time)
    {
        return Vecf2{tex_animation(anim.u, time), tex_animation(anim.v, time)};
    }

    void populate_chicago_uv_anims(
        materials::senv_micro&                          mat,
        Span<blam::shader::chicago::map_t const> const& maps,
        f32                                             t)
    {
        using namespace blam::shader;

        u32 i = 0;
        for(chicago::map_t const& map : maps)
        {
            auto        uv = uv_animation(map.anim_2d, t);
            auto const& i2 = mat.material.inputs[0];
            switch(i)
            {
            case 0:
                mat.material.inputs1 = uv;
                break;
            case 1:
                mat.material.inputs[0] = Vecf4(uv, 0, 0);
                break;
            case 2:
                mat.material.inputs[0] = Vecf4(i2.x, i2.y, uv.x, uv.y);
                break;
            case 3:
                mat.material.inputs[1] = Vecf4(uv, 0, 0);
                break;
            }
            i++;
        }
    }

    void update_uv_animations(
        materials::senv_micro&  mat,
        generation_idx_t const& shader_id,
        time_point const&       time)
    {
        using namespace blam::shader;

        ShaderItem const& shader = find(shader_id)->second;

        f32 t = stl_types::Chrono::to_float(time) / 10.f;

        switch(shader.tag_class)
        {
        case blam::tag_class_t::scex: {
            shader_chicago_extended<V> const* info
                = shader.header->as<blam::shader::shader_chicago_extended<V>>();
            auto maps = info->maps_4stage.data(magic).value();
            populate_chicago_uv_anims(mat, maps, t);
            break;
        }
        case blam::tag_class_t::schi: {
            shader_chicago<V> const* info
                = shader.header->as<shader_chicago<V>>();
            auto maps = info->maps.data(magic).value();
            populate_chicago_uv_anims(mat, maps, t);
            break;
        }
        case blam::tag_class_t::senv: {
            shader_env const* info = shader.header->as<shader_env>();
            (void)info;
            break;
        }
        default:
            break;
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
    : DataCache<
          ModelItem<V>,
          std::tuple<u32, blam::mod2::mod2_lod>,
          blam::tagref_t const&,
          blam::mod2::mod2_lod>,
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

    inline auto vertex_data(blam::mod2::part const& model)
    {
        return model.vertex_segment(*tags, magic).data(vertex_magic);
    }
    inline auto index_data(blam::mod2::part const& model)
    {
        return model.index_segment(*tags).data(vertex_magic);
    }

    virtual ModelItem<V> predict_impl(
        blam::tagref_t const& mod2, blam::mod2::mod2_lod lod) override
    {
        auto _ = allocator->debug().scope("ModelCache");

        using namespace blam::mod2;

        blam::mod2::header<V> const* header = get_header(mod2);

        if(!header)
            return {};

        ModelItem<V> out;
        out.mesh   = {};
        out.header = header;
        out.tag    = &(*index.find(mod2));

        Span<const blam::shader::shader_desc> shaders;
        if(auto const& shaders_ = header->shaders.data(magic);
           shaders_.has_error())
            Throw(undefined_behavior("no shaders found"));
        else
            shaders = shaders_.value();

        blam::mod2::model_data_t model_data;
        if(auto model_data_ = header->model_at(lod, magic);
           model_data_.has_value())
            model_data = std::move(model_data_.value());
        else
            return {};
        //        auto const& geom =
        //        header[0].geometries.data(magic).value()[geom_idx];
        {
            //            out.mesh.header = &geom;

            //            for(auto const& model : geom.meshes(magic))
            for(auto const* part : model_data.parts)
            {
                auto elements_ = index_data(*part);
                auto vertices_ = vertex_data(*part);

                if(!elements_.has_value() || !vertices_.has_value())
                {
                    cWarning(
                        "Invalid element/vertex data for model: {0}",
                        mod2.to_name().to_string(magic));
                    continue;
                }
                auto elements = elements_.value();
                auto vertices = vertices_.value();

                if(elements.empty() || vertices.empty())
                {
                    cWarning(
                        "Failed to get element/vertex data for model: {0}",
                        mod2.to_name().to_string(magic));
                    continue;
                }

                using element_type = typename std::remove_const<
                    typename decltype(elements)::value_type>::type;
                using vertex_type = typename std::remove_const<
                    typename decltype(vertices)::value_type>::type;

                out.mesh.sub.emplace_back();
                auto& draw_data         = out.mesh.sub.back();
                draw_data.header        = part;
                draw_data.draw.elements = {
                    .count         = static_cast<u32>(elements.size()),
                    .offset        = element_ptr,
                    .vertex_offset = vert_ptr / sizeof(vertex_type),
                    .type          = semantic::TypeEnum::UShort,
                };
                draw_data.draw.instances.count = 1;
                draw_data.shader
                    = shader_cache.predict(shaders[part->shader_idx].ref);

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
    virtual std::tuple<u32, blam::mod2::mod2_lod> get_id(
        blam::tagref_t const& tag, blam::mod2::mod2_lod lod) override
    {
        return std::make_tuple(tag.tag_id, lod);
    }

    ModelAssembly predict_regions(
        blam::tagref_t const& tag,
        blam::mod2::mod2_lod  max_lod = blam::mod2::lod_high_ext)
    {
        ModelAssembly assem  = {};
        auto          header = get_header(tag);

        if(!header)
            return {};

        assem.models.push_back(this->predict(tag, max_lod));

        //        for(auto const& region : header->regions.data(magic).value())
        //        {
        //            assem.models.emplace_back();
        //            auto& mod = assem.models.back();
        //            for(auto const& perm :
        //            region.permutations.data(magic).value())
        //            {
        //                auto const& lod = perm.meshindex_lod.at(max_lod);
        //                mod = this->predict(tag, lod);
        //                break;
        //            }
        //        }

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

    BSPCache(BitmapCache<V>& bitm_cache, ShaderCache<V>& shader_cache) :
        version(V::version_v), bitm_cache(bitm_cache),
        shader_cache(shader_cache)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        index    = blam::tag_index_view(map);
        magic    = map.magic;
        vert_ptr = 0, element_ptr = 0, light_ptr = 0, portal_ptr = 12,
        portal_color_ptr = 4;
        evict_all();
    }

    blam::version_t         version;
    BitmapCache<V>&         bitm_cache;
    ShaderCache<V>&         shader_cache;
    blam::tag_index_view<V> index;
    blam::magic_data_t      magic;

    Bytes                      vert_buffer, element_buffer, light_buffer;
    semantic::mem_chunk<Vecf3> portal_buffer, portal_color_buffer;
    u32 vert_ptr, element_ptr, light_ptr, portal_ptr, portal_color_ptr;

    virtual BSPItem predict_impl(blam::bsp::info const& bsp) override
    {
        using namespace semantic::chunk_ops;

        //        GFX::DBG::SCOPE _("BSPCache");

        auto        bsp_magic = bsp.bsp_magic(magic);
        auto const& section_  = bsp.to_bsp(bsp_magic).to_header().data(
            bsp_magic, blam::single_value);
        if(section_.has_error())
        {
            cDebug("Failed to find BSP section");
            return {};
        }
        auto const& section = *section_.value();

        BSPItem out;
        out.mesh = &section;
        out.tag  = &(*index.find(bsp.tag));

        if(!out.tag->valid())
            return {};

        //        auto shader = section.shaders.data(bsp_magic);

        Span<blam::bsp::cluster_portal const> portals
            = section.cluster_portals.data(bsp_magic).value();

        //        for(auto const& portal : portals)
        //        {
        //            auto vertices = portal.vertices.data(bsp_magic).value();
        //            std::copy(
        //                vertices.begin(),
        //                vertices.end(),
        //                portal_buffer.begin() + portal_ptr);
        //            portal_color_buffer[portal_color_ptr] = Vecf3(1);
        //            out.portals.push_back({
        //                .arrays = {
        //                     .count  = static_cast<u32>(vertices.size()),
        //                     .offset = static_cast<u32>(portal_ptr),
        //                },
        //            });
        //            out.portal_color_ptrs.push_back(portal_color_ptr);

        //            portal_ptr += vertices.size();
        //            portal_color_ptr++;
        //        }

        for(blam::bsp::cluster const& cluster :
            section.clusters.data(bsp_magic).value())
        {
            out.clusters.push_back({
                .cluster = &cluster,
            });
            BSPItem::Cluster& it = out.clusters.back();
            for(auto const& portal_idx :
                cluster.portals.data(bsp_magic).value())
                it.portals.push_back(&portals[portal_idx]);
            for(blam::bsp::subcluster const& sub :
                cluster.sub_clusters.data(bsp_magic).value())
            {
                auto indices    = sub.indices.data(bsp_magic).value();
                auto [min, max] = sub.bounds.points();
                std::array<Vecf3, 8> vertices = {{
                    min,
                    Vecf3(max.x, min.y, min.z),
                    Vecf3(max.x, max.y, min.z),
                    Vecf3(min.x, max.y, min.z),
                    Vecf3(min.x, max.y, max.z),
                    Vecf3(min.x, min.y, max.z),
                    Vecf3(max.x, min.y, max.z),
                    max,
                }};
                std::copy(
                    vertices.begin(),
                    vertices.end(),
                    portal_buffer.begin() + portal_ptr);
                portal_color_buffer[portal_color_ptr] = Vecf3(0, 1, 0);
                out.portals.push_back({
                    .arrays = {
                        .count  = static_cast<u32>(8),
                        .offset = static_cast<u32>(portal_ptr),
                    },
                });
                it.sub.push_back(BSPItem::Subcluster{
                    .cluster = &sub,
                    .indices = indices,
                });
                portal_ptr += 8;
                portal_color_ptr++;
            }
        }

        /* TODO: Find link between indices in cluster and submeshes */

        auto submeshes      = section.lightmaps.data(bsp_magic);
        auto leaves         = section.leaves.data(bsp_magic);
        auto leaf_surfaces_ = section.leaf_surfaces.data(bsp_magic);

        if(submeshes.has_error() || leaves.has_error()
           || leaf_surfaces_.has_error())
        {
            cDebug("Error finding submeshes");
            return {};
        }

//        for(auto const& leaf : leaves.value())
//        {
//            cDebug("Leaf of cluster {}", leaf.cluster);
//        }
//        for(auto const& surface : leaf_surfaces_.value())
//        {
//            cDebug(
//                "Leaf surface {} in node {}",
//                surface.surface,
//                surface.node);
//        }

        for(auto const& group : submeshes.value())
        {
            auto meshes = group.materials.data(bsp_magic);
            if(meshes.has_error())
                continue;
            out.groups.emplace_back();
            auto& group_data = out.groups.back();
            group_data.group = &group;
            for(auto const& mesh : meshes.value())
            {
                group_data.meshes.emplace_back();
                auto& mesh_data = group_data.meshes.back();

                /* Just dig up the textures, long process */
                mesh_data.shader = shader_cache.predict(mesh.shader);
                if(group.lightmap_idx != -1)
                    mesh_data.light_bitm = bitm_cache.resolve(
                        section.lightmap_, group.lightmap_idx);

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

struct FontItem
{
    blam::font const* font{nullptr};
    u32               atlas_layer;
};

template<typename V>
struct FontCache
    : DataCache<FontItem, blam::font const*, blam::tagref_t const&>,
      Components::SubsystemBase
{
    using type = FontCache<V>;

    FontCache(gfx::api* allocator) :
        font_textures(std::make_shared<gfx::compat::texture_2da_t>(
            allocator, PixDesc(PixFmt::RGBA8), 1))
    {
    }

    blam::tag_index_view<V>                     index;
    blam::magic_data_t                          magic;
    std::shared_ptr<gfx::compat::texture_2da_t> font_textures;

    inline void load_from(blam::map_container<V> const& map)
    {
        index = blam::tag_index_view(map);
        magic = map.magic;
        evict_all();
    }

    virtual FontItem predict_impl(blam::tagref_t const& font_tag)
    {
        blam::font const* font = get_id(font_tag);
        return FontItem{
            .font        = font,
            .atlas_layer = 0,
        };
    }

    virtual blam::font const* get_id(blam::tagref_t const& font_tag)
    {
        if(auto tag_it = index.find(font_tag); tag_it == index.end())
            return nullptr;
        else
            return (*tag_it).template data<blam::font>(magic).value();
    }

    void allocate_font_texture()
    {
        Vecui3 total_size{0, 0, m_cache.size()};

        for(auto const& [id, font_] : m_cache)
        {
            [[maybe_unused]] FontItem const& font = font_;
        }
    }
};
