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
#include <coffee/graphics/apis/CGLeamRHI>

#include <coffee/interfaces/graphics_subsystem.h>

#include <coffee/strings/libc_types.h>
#include <coffee/strings/vector_types.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using GFX       = RHI::GLEAM::GLEAM_API;
using GFX_ALLOC = RHI::Components::GraphicsAllocator<GFX>;
using PIP_PARAM = GFX_ALLOC::AllocData::PipelineParams;

using cache_id_t = u64;

struct generation_idx_t
{
    generation_idx_t() : i(0), gen(0), _pad(0)
    {
    }
    generation_idx_t(cache_id_t i, u32 gen) : i(i), gen(gen), _pad(0)
    {
    }

    cache_id_t i;
    u32        gen;
    u32        _pad;

    inline bool valid() const
    {
        return i != 0 && gen != 0;
    }
};
template<size_t NumIdx>
using hierarchial_idx_t = Array<u32, NumIdx>;

using bitm_format_hash = Tup<PixFmt, PixCmp, BitFmt, CompFlags>;

struct BSPItem
{
    BSPItem() : mesh(nullptr)
    {
    }

    struct Mesh
    {
        blam::bsp::submesh_header const* mesh;
        GFX::D_DATA                      draw;
        generation_idx_t                 light_bitm;
        generation_idx_t                 shader;
    };
    struct Group
    {
        blam::bsp::submesh_group const* group;
        Vector<Mesh>                    meshes;
    };

    blam::bsp::header const* mesh;
    blam::tag_t const*       tag;
    Vector<Group>            groups;

    inline bool valid() const
    {
        return mesh;
    }
};

struct ModelItem
{
    ModelItem() : header(nullptr)
    {
    }

    struct SubModel
    {
        blam::mod2::submesh_header const* header;
        GFX::D_DATA                       draw;
        generation_idx_t                  shader;
    };
    struct LOD
    {
        blam::mod2::geometry_header const* header;
        Vector<SubModel>                   sub;
    };

    blam::mod2::header const* header;
    blam::tag_t const*        tag;
    LOD                       mesh;
    Vecf2                     uvscale;

    inline bool valid() const
    {
        return header;
    }
};

struct ModelAssembly
{
    mem_chunk<blam::mod2::region const> regions;
    Vector<Array<generation_idx_t, 5>>  models;
};

struct BitmapItem
{
    BitmapItem() : header(nullptr)
    {
    }

    blam::bitm::header_t const* header;
    blam::tag_t const*          shader_tag;
    blam::tag_t const*          tag;
    struct
    {
        blam::bitm::image_t const* mip;
        PixDesc                    fmt;
        bitm_format_hash           bucket;

        u32   layer;
        Vecf2 offset;
        Vecf2 scale;
    } image;

    inline bool valid() const
    {
        return header;
    }
};

struct ShaderItem
{
    ShaderItem() : header(nullptr)
    {
    }

    blam::shader_base const* header;
    blam::tag_t const*       tag;

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
        generation_idx_t map1;
        generation_idx_t map2;
    };

    union
    {
        generation_idx_t color_bitm;

        soso_t soso;
        senv_t senv;
        schi_t schi;

        struct
        {
        } scex;
        struct
        {
        } smet;
        struct
        {
        } swat;
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

    DataCache() : counter(0), generation(1)
    {
    }

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

    cache_id_t              counter;
    Map<cache_id_t, T>      m_cache;
    Map<IdType, cache_id_t> m_cache_key;
    u32                     generation;

    virtual T      predict_impl(IType... param) = 0;
    virtual IdType get_id(IType... args)        = 0;
    virtual void   evict_impl()
    {
    }
};

struct BitmapCache
    : DataCache<BitmapItem, Tup<u32, i16>, blam::tagref_t const&, i16>
{
    struct TextureBucket
    {
        ShPtr<GFX::S_2DA>  surface;
        ShPtr<GFX::SM_2DA> sampler;
        u32                ptr;
        PixDesc            fmt;
    };

    BitmapCache(
        blam::map_container const& map,
        blam::magic_data_t const&  bitmap_magic,
        GFX_ALLOC*                 allocator) :
        magic(map.magic),
        bitm_magic(
            map.map->version == blam::version_t::xbox ? magic : bitmap_magic),
        index(map),
        bitm_header(blam::bitm::bitmap_header_t::from_data(
            Bytes::From(bitmap_magic.base_ptr, bitmap_magic.max_size))),
        allocator(allocator)
    {
        bitm_magic.magic_offset = 0;
    }

    blam::magic_data_t                 magic;
    blam::magic_data_t                 bitm_magic;
    blam::tag_index_view               index;
    blam::bitm::bitmap_header_t const* bitm_header;
    GFX_ALLOC*                         allocator;

    Map<bitm_format_hash, TextureBucket> tex_buckets;

    static inline bitm_format_hash create_hash(PixDesc const& fmt)
    {
        return std::make_tuple(fmt.pixfmt, fmt.comp, fmt.bfmt, fmt.cmpflg);
    }

    TextureBucket& get_bucket(PixDesc const& fmt)
    {
        auto hash = create_hash(fmt);

        auto it = tex_buckets.find(hash);

        if(it != tex_buckets.end())
            return it->second;

        auto& bucket = tex_buckets.insert({hash, {}}).first->second;

        bucket.ptr = 0;
        bucket.fmt = fmt;
        std::tie(bucket.sampler, bucket.surface) =
            allocator->alloc_surface_sampler<GFX::S_2DA>(
                fmt.pixfmt, 1, C_CAST<u32>(fmt.cmpflg) << 10);

        bucket.sampler->setFiltering(
            Filtering::Linear, Filtering::Linear, Filtering::Linear);

        return bucket;
    }

    void commit_bitmap(BitmapItem const& img)
    {
        GFX::DBG::SCOPE _(img.tag->to_name().to_string(magic));

        auto& bucket = get_bucket(img.image.fmt);
        auto  name   = img.tag->to_name().to_string(magic);

        auto img_data =
            (img.tag->storage == blam::image_storage_t::internal &&
             index.file()->version == blam::version_t::custom_edition)
                ? img.image.mip->data(magic.no_magic())
                : img.image.mip->data(bitm_magic);

        auto img_size = img.image.mip->isize.convert<i32>();

        bucket.surface->upload(
            img.image.fmt,
            Size3(img_size.w, img_size.h, 1),
            img_data,
            Point3(
                img.image.offset.x(), img.image.offset.y(), img.image.layer));
    }

    void allocate_storage()
    {
        using size_bucket = Tup<u32, u32>;
        struct pool_size
        {
            u32          num    = 0;
            u32          layers = 0;
            size_2d<i32> max    = {};

            MultiMap<size_bucket, BitmapItem*> images;
        };

        ProfContext _("Building texture atlases");

        Map<bitm_format_hash, pool_size> fmt_count;

        for(auto& bitm : m_cache)
        {
            auto const& fmt = bitm.second.image.fmt;
            auto        hash =
                std::make_tuple(fmt.pixfmt, fmt.comp, fmt.bfmt, fmt.cmpflg);
            auto&       pool   = fmt_count[hash];
            auto const& imsize = bitm.second.image.mip->isize;

            pool.num++;
            pool.max.w = std::max<u32>(pool.max.w, imsize.w);
            pool.max.h = std::max<u32>(pool.max.h, imsize.h);
            pool.images.insert(
                {std::make_tuple(imsize.w, imsize.h), &bitm.second});
        }

        for(auto& pool_ : fmt_count)
        {
            auto& pool = pool_.second;

            size_2d<i32> offset = {0, 0};

            u32 layer = 0;

            for(auto it = pool.images.rbegin(); it != pool.images.rend(); ++it)
            {
                auto img    = it->second;
                auto imsize = img->image.mip->isize;

                auto img_offset = offset;
                auto img_layer  = layer;

                size_2d<i32> slack = {pool.max.w - offset.w,
                                      pool.max.h - offset.h};

                bool fits_width  = (offset.w + imsize.w) <= pool.max.w;
                i32  height_diff = offset.h - imsize.h;

                if(height_diff < 0)
                    offset.h += -height_diff;

                if(fits_width)
                {
                    img_offset.w = offset.w;
                    img_offset.h = offset.h - imsize.h;
                    offset.w += imsize.w;
                } else
                {
                    layer++;
                    offset     = imsize;
                    img_offset = {};
                    img_layer  = layer;
                }

                img->image.layer  = layer;
                img->image.offset = Vecf2(img_offset.w, img_offset.h);
                img->image.scale  = {imsize.w, imsize.h};

                img->image.scale.x() /= pool.max.w;
                img->image.scale.y() /= pool.max.h;

                cDebug(
                    "{0}x{1} - {2}x{3} @ {4} ({5},{6})",
                    img_offset.w,
                    img_offset.h,
                    offset.w,
                    offset.h,
                    img_layer,
                    imsize.w,
                    imsize.h);
            }

            pool.layers = layer + 1;
        }

        for(auto& bucket : tex_buckets)
        {
            auto& props = bucket.second;
            auto& pool  = fmt_count[bucket.first];
            props.surface->allocate(
                Size3(pool.max.w, pool.max.h, pool.layers), props.fmt.comp);
        }

        for(auto const& bitm : m_cache)
            commit_bitmap(bitm.second);

        for(auto& pool : fmt_count)
            for(auto& image : pool.second.images)
            {
                image.second->image.offset.x() /= pool.second.max.w;
                image.second->image.offset.y() /= pool.second.max.h;
            }
    }

    virtual BitmapItem predict_impl(
        blam::tagref_t const& shader, i16 idx) override
    {
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

        Vecf2 prescale = {1, 1};

        switch(shader.tag_class)
        {
        case tag_class_t::bitm:
        {
            bitm_tag = (*it);

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
            bitm_ptr =
                bitm_header->get_block(bitm_tag->offset).data(bitm_magic).data;
            curr_magic = bitm_header->block_magic(bitm_magic, bitm_tag->offset);
        } else
            bitm_ptr =
                bitm_tag->to_reflexive<blam::bitm::header_t>().data(magic).data;

        if(!bitm_ptr)
            Throw(undefined_behavior("failed to get bitmap header"));

        auto const& bitm = *bitm_ptr;

        BitmapItem out;
        out.header     = &bitm;
        out.tag        = bitm_tag;
        out.shader_tag = *it;

        GFX::DBG::SCOPE _("BitmapCache::predict_impl");

        auto image_ = bitm.images.data(curr_magic);

        if(image_.data)
        {
            if(image_.data->type != blam::bitm::type_t::tex_2d)
            {
                cDebug(
                    "unimplemented texture type: {0}",
#if C_HAS_INCLUDE(<string_view>)
                    magic_enum::enum_name(image_.data->type)
#else
                    "[unknown]"
#endif
                );
                return {};
            }

            auto const& image = image_[idx];

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

            auto& bucket = get_bucket(fmt);
            img.bucket   = create_hash(fmt);
            img.fmt      = fmt;

            img.layer = bucket.ptr;
            bucket.ptr++;
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
};

struct ShaderCache : DataCache<ShaderItem, u32, blam::tagref_t const&>
{
    ShaderCache(blam::map_container const& map, BitmapCache& bitmap_cache) :
        bitm_cache(bitmap_cache), index(map), magic(map.magic)
    {
    }

    BitmapCache&         bitm_cache;
    blam::tag_index_view index;
    blam::magic_data_t   magic;

    template<blam::tag_class_t Tag>
    generation_idx_t get_bitm_idx(blam::tagref_typed_t<Tag> const& bitm)
    {
        if(!bitm.valid())
            return generation_idx_t();

        return bitm_cache.predict(bitm.to_plain(), 0);
    }

    ShaderItem predict_impl(blam::tagref_t const& shader)
    {
        using blam::tag_class_t;

        if(!shader.valid())
            return {};

        auto it                    = index.find(shader);
        C_UNUSED(auto shader_name) = shader.to_name().to_string(magic);

        if(it == index.end())
            return {};

        ShaderItem out;
        out.tag    = *it;
        out.header = (*it)->to_reflexive<blam::shader_base>().data(magic).data;

        switch(shader.tag_class)
        {
        case tag_class_t::senv:
        {
            auto const& shader_env =
                (*it)->to_reflexive<blam::shader_env>().data(magic)[0];

            out.senv.base_bitm = get_bitm_idx(shader_env.diffuse.base);
            out.senv.primary_bitm =
                get_bitm_idx(shader_env.diffuse.primary.map);
            out.senv.secondary_bitm =
                get_bitm_idx(shader_env.diffuse.secondary.map);
            out.senv.micro_bitm = get_bitm_idx(shader_env.diffuse.micro.map);

            out.senv.self_illum = get_bitm_idx(shader_env.self_illum.map.map);
            out.senv.bump       = get_bitm_idx(shader_env.bump.map);
            out.senv.reflection_bitm =
                get_bitm_idx(shader_env.reflection.reflection);

            break;
        }
        case tag_class_t::soso:
        {
            auto const& shader_model =
                (*it)->to_reflexive<blam::shader_model>().data(magic)[0];

            out.soso.base_bitm   = get_bitm_idx(shader_model.maps.base);
            out.soso.multi_bitm  = get_bitm_idx(shader_model.maps.multipurpose);
            out.soso.detail_bitm = get_bitm_idx(shader_model.maps.detail.map);

            //            if(shader_model.maps.multipurpose.valid())
            //                out.color_bitm =
            //                get_bitm_idx(shader_model.maps.multipurpose);

            break;
        }
        case tag_class_t::schi:
        {
            auto const& shader_model =
                (*it)->to_reflexive<blam::shader_chicago>().data(magic)[0];

            auto        maps = shader_model.maps.data(magic);
            auto const& map1 = maps[0];

            out.schi.map1 = get_bitm_idx(maps[0].map.map);

            break;
        }
        case tag_class_t::scex:
        {
            auto const& shader_model =
                (*it)->to_reflexive<blam::shader_chicago_extended>().data(
                    magic)[0];

            auto maps4 = shader_model.maps_4stage.data(magic);

            out.color_bitm = get_bitm_idx(maps4[0].map.map);

            break;
        }
        case tag_class_t::sgla:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_glass>().data(magic)[0];

            out.color_bitm = get_bitm_idx(shader_model.diffuse.map.map);

            break;
        }
        case tag_class_t::swat:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_water>().data(magic)[0];

            out.color_bitm = get_bitm_idx(shader_model.base);

            break;
        }
        case tag_class_t::spla:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_plasma>().data(magic)[0];

            out.color_bitm = get_bitm_idx(shader_model.primary_noise.noise.map);

            break;
        }
        case tag_class_t::smet:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_meter>().data(magic)[0];

            out.color_bitm = get_bitm_idx(shader_model.map);

            break;
        }
        case tag_class_t::sotr:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_base>().data(magic)[0];

            break;
        }
        default:
            Throw(undefined_behavior("unhandled shader type"));
        }

        if(!out.color_bitm.valid())
            out.header = nullptr;

        return out;
    }
    u32 get_id(blam::tagref_t const& args)
    {
        return args.tag_id;
    }
};

struct BSPCache
    : DataCache<BSPItem, blam::bsp::info const*, blam::bsp::info const&>
{
    BSPCache(
        blam::map_container const& map,
        BitmapCache&               bitm_cache,
        ShaderCache&               shader_cache) :
        version(map.map->version),
        bitm_cache(bitm_cache), shader_cache(shader_cache), index(map),
        magic(map.magic), vert_ptr(0), element_ptr(0), light_ptr(0)
    {
    }

    blam::version_t      version;
    BitmapCache&         bitm_cache;
    ShaderCache&         shader_cache;
    blam::tag_index_view index;
    blam::magic_data_t   magic;

    Bytes vert_buffer, element_buffer, light_buffer;
    u32   vert_ptr, element_ptr, light_ptr;

    virtual BSPItem predict_impl(blam::bsp::info const& bsp) override
    {
        auto        bsp_magic = bsp.bsp_magic(magic);
        auto const& section_  = bsp.to_bsp(bsp_magic);
        auto const& section   = section_.to_header().data(bsp_magic)[0];

        BSPItem out;
        out.mesh = &section;
        out.tag  = *index.find(bsp.tag);

        auto shader = section.shaders.data(bsp_magic);

        for(auto const& group : section.submesh_groups.data(bsp_magic))
        {
            out.groups.emplace_back();
            auto& group_data = out.groups.back();
            group_data.group = &group;
            for(auto const& mesh : group.material.data(bsp_magic))
            {
                group_data.meshes.emplace_back();
                auto& mesh_data = group_data.meshes.back();

                /* Just dig up the textures, long process */
                mesh_data.shader = shader_cache.predict(mesh.shader);
                mesh_data.light_bitm =
                    bitm_cache.resolve(section.lightmaps, group.lightmap_idx);

                /* ... and moving on */

                if(version == blam::version_t::xbox)
                {
                    auto indices     = mesh.indices(section).data(bsp_magic);
                    auto vertices    = mesh.xbox_vertices().data(bsp_magic);
                    auto light_verts = mesh.xbox_light_verts().data(bsp_magic);

                    if(!vertices || !indices)
                    {
                        group_data.meshes.erase(--group_data.meshes.end());
                        continue;
                    }

                    using vertex_type =
                        std::remove_const<decltype(vertices)::value_type>::type;
                    using light_type = std::remove_const<decltype(
                        light_verts)::value_type>::type;
                    using element_type =
                        std::remove_const<decltype(indices)::value_type>::type;

                    mesh_data.mesh          = &mesh;
                    mesh_data.draw.m_eltype = semantic::TypeEnum::UShort;
                    mesh_data.draw.m_eoff =
                        element_ptr / sizeof(blam::vert::idx_t);
                    mesh_data.draw.m_elems = C_FCAST<u32>(indices.elements);
                    mesh_data.draw.m_voff  = vert_ptr / sizeof(vertex_type);
                    mesh_data.draw.m_insts = 1;

                    MemCpy(
                        vertices, vert_buffer.at(vert_ptr).as<vertex_type>());
                    MemCpy(
                        indices,
                        element_buffer.at(element_ptr).as<element_type>());
                    MemCpy(
                        light_verts,
                        light_buffer.at(light_ptr).as<light_type>());

                    vert_ptr += vertices.size;
                    element_ptr += indices.size;
                    light_ptr += light_verts.size;
                } else
                {
                    auto indices     = mesh.indices(section).data(bsp_magic);
                    auto vertices    = mesh.pc_vertices().data(bsp_magic);
                    auto light_verts = mesh.pc_light_verts().data(bsp_magic);

                    using vertex_type =
                        std::remove_const<decltype(vertices)::value_type>::type;
                    using light_type = std::remove_const<decltype(
                        light_verts)::value_type>::type;
                    using element_type =
                        std::remove_const<decltype(indices)::value_type>::type;

                    mesh_data.mesh          = &mesh;
                    mesh_data.draw.m_eltype = semantic::TypeEnum::UShort;
                    mesh_data.draw.m_eoff =
                        element_ptr / sizeof(blam::vert::idx_t);
                    mesh_data.draw.m_elems = C_FCAST<u32>(indices.elements);
                    mesh_data.draw.m_voff  = vert_ptr / sizeof(vertex_type);
                    mesh_data.draw.m_insts = 1;

                    MemCpy(
                        vertices, vert_buffer.at(vert_ptr).as<vertex_type>());
                    MemCpy(
                        indices,
                        element_buffer.at(element_ptr).as<element_type>());
                    MemCpy(
                        light_verts,
                        light_buffer.at(light_ptr).as<light_type>());

                    vert_ptr += vertices.size;
                    element_ptr += indices.size;
                    light_ptr += light_verts.size;
                }
            }
        }

        return out;
    }
    virtual blam::bsp::info const* get_id(blam::bsp::info const& bsp) override
    {
        return &bsp;
    }
};

namespace detail {

template<
    typename Version,
    typename std::enable_if<
        !std::is_same<Version, blam::xbox_version_t>::value>::type* = nullptr>
static inline auto vertex_data(
    blam::mod2::submesh_header const& model,
    blam::magic_data_t const&         magic,
    blam::tag_index_t const*          tags)
{
    return model.vertex_segment(*tags).data(magic);
}

template<
    typename Version,
    typename std::enable_if<
        std::is_same<Version, blam::xbox_version_t>::value>::type* = nullptr>
static inline auto vertex_data(
    blam::mod2::submesh_header const& model,
    blam::magic_data_t const&         magic,
    blam::tag_index_t const*)
{
    auto const& vertex_ref = model.xbox_vertex_ref().data(magic)[0];
    return vertex_ref.vertices(model.vert_count).data(magic);
}

template<
    typename Version,
    typename std::enable_if<
        std::is_same<Version, blam::xbox_version_t>::value>::type* = nullptr>
static inline auto index_data(
    blam::mod2::submesh_header const& model,
    blam::magic_data_t const&         magic,
    blam::tag_index_t const*          tags)
{
    return model.xbox_index_segment().data(magic);
}

template<
    typename Version,
    typename std::enable_if<
        !std::is_same<Version, blam::xbox_version_t>::value>::type* = nullptr>
static inline auto index_data(
    blam::mod2::submesh_header const& model,
    blam::magic_data_t const&         magic,
    blam::tag_index_t const*          tags)
{
    return model.index_segment(*tags).data(magic);
}

} // namespace detail

template<typename Version>
struct ModelCache
    : DataCache<ModelItem, Tup<u32, u16>, blam::tagref_t const&, u16>
{
    using Variant = typename std::conditional<
        std::is_same<Version, blam::xbox_version_t>::value,
        blam::xbox_variant,
        blam::pc_variant>::type;

    ModelCache(
        blam::map_container const& map,
        BitmapCache&               bitm_cache,
        ShaderCache&               shader_cache) :
        version(map.map->version),
        tags(map.tags), index(map), magic(map.magic),
        vertex_magic(
            map.map->version == blam::version_t::xbox
                ? map.magic
                : tags->vertex_magic(magic)),
        bitm_cache(bitm_cache), shader_cache(shader_cache), vert_ptr(0),
        element_ptr(0)
    {
    }

    blam::version_t          version;
    blam::tag_index_t const* tags;
    blam::tag_index_view     index;
    blam::magic_data_t       magic;
    blam::magic_data_t       vertex_magic;
    BitmapCache&             bitm_cache;
    ShaderCache&             shader_cache;

    Bytes vert_buffer, element_buffer;
    u32   vert_ptr, element_ptr;

    blam::mod2::header const* get_header(blam::tagref_t const& mod2)
    {
        auto header_it = index.find(mod2);

        if(header_it == index.end())
            return nullptr;

        auto header = (*header_it)
                          ->template to_reflexive<blam::mod2::header>()
                          .data(magic);

        return &header[0];
    }

    inline auto vertex_data(blam::mod2::submesh_header const& model)
    {
        return detail::vertex_data<Version>(model, vertex_magic, tags);
    }
    inline auto index_data(blam::mod2::submesh_header const& model)
    {
        return detail::index_data<Version>(model, vertex_magic, tags);
    }

    virtual ModelItem predict_impl(
        blam::tagref_t const& mod2, u16 geom_idx) override
    {
        using namespace blam::mod2;

        blam::mod2::header const* header = get_header(mod2);

        if(!header)
            return {};

        ModelItem out;
        out.mesh    = {};
        out.header  = header;
        out.tag     = *index.find(mod2);
        out.uvscale = Vecf2(header->uvscale.u, header->uvscale.v);

        auto const& shaders = header->shaders.data(magic);

        auto const& geom = header[0].geometries.data(magic)[geom_idx];
        {
            out.mesh.header = &geom;

            for(auto const& model : geom.template meshes<Variant>(magic))
            {
                auto elements = index_data(model.data);
                auto vertices = vertex_data(model.data);

                using element_type =
                    typename std::remove_const<typename decltype(
                        elements)::value_type>::type;
                using vertex_type =
                    typename std::remove_const<typename decltype(
                        vertices)::value_type>::type;

                if(!elements || !vertices)
                    Throw(undefined_behavior(
                        "failed to locate element or vertex data"));

                out.mesh.sub.emplace_back();
                auto& draw_data         = out.mesh.sub.back();
                draw_data.header        = &model.data;
                draw_data.draw.m_eltype = semantic::TypeEnum::UShort;
                draw_data.draw.m_eoff   = element_ptr / sizeof(element_type);
                draw_data.draw.m_elems  = C_FCAST<u32>(elements.elements);
                draw_data.draw.m_voff   = vert_ptr / sizeof(vertex_type);
                draw_data.draw.m_insts  = 1;
                draw_data.shader =
                    shader_cache.predict(shaders[model.data.shader_idx].ref);

                MemCpy(
                    vertices,
                    vert_buffer.at(vert_ptr).template as<vertex_type>());
                MemCpy(
                    elements,
                    element_buffer.at(element_ptr).template as<element_type>());

                vert_ptr += vertices.size;
                element_ptr += vertices.size;
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

        for(auto const& region : header->regions.data(magic))
        {
            assem.models.emplace_back();
            auto& mod = assem.models.back();
            for(auto const& perm : region.permutations.data(magic))
            {
                auto const& lod = perm.meshindex_lod.at(max_lod);
                mod.at(max_lod) = predict(tag, lod);
            }
        }

        return assem;
    }
};
