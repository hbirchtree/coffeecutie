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
using namespace StandardInput;
using GFX       = RHI::GLEAM::GLEAM_API;
using GFX_ALLOC = RHI::Components::GraphicsAllocator<GFX>;
using PIP_PARAM = GFX_ALLOC::AllocData::PipelineParams;

using camera_t     = typing::vectors::scene::camera<scalar>;
using std_camera_t = StandardCamera<camera_t*, StandardCameraOpts*>;

using comp_app::detail::duration;
using comp_app::detail::time_point;
using vertex_type      = blam::vert::vertex<blam::vert::uncompressed>;
using xbox_vertex_type = blam::vert::vertex<blam::vert::compressed>;
using Components::ComponentRef;
using Components::EntityContainer;

using halo_version = blam::pc_version_t;

struct memory_budget
{
    static constexpr auto bsp_buffer      = 30_MB;
    static constexpr auto bsp_elements    = 10_MB;
    static constexpr auto mesh_buffer     = 20_MB;
    static constexpr auto mesh_elements   = 10_MB;
    static constexpr auto matrix_buffer   = 4_MB;
    static constexpr auto material_buffer = 4_MB;

    static constexpr auto grand_total = bsp_buffer + bsp_elements +
                                        mesh_buffer + mesh_elements +
                                        matrix_buffer + material_buffer;
};

using cache_id_t = u64;

struct generation_idx_t
{
    generation_idx_t() : i(0), gen(0)
    {
    }
    generation_idx_t(cache_id_t i, u32 gen) : i(i), gen(gen)
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

using bitm_format_hash = Tup<PixFmt, PixCmp, BitFmt, CompFlags>;

struct BspReference
{
    u32 draw_idx;

    generation_idx_t bsp;
    generation_idx_t bitmap;

    struct
    {
        GFX::D_DATA draw;
        GFX::D_CALL call;
    } draw;
    bool visible = false;
};

struct ModelReference
{
    Matf4 transform;
    u32   draw_idx;

    generation_idx_t                  bitmap;
    generation_idx_t                  model;
    blam::scn::object_spawn<0> const* instance;

    struct
    {
        GFX::D_DATA draw;
        GFX::D_CALL call;
    } draw;
};

enum ObjectTags
{
    ObjectScenery   = 0x1,
    ObjectEquipment = 0x2,
    ObjectVehicle   = 0x4,
    ObjectBiped     = 0x8,

    ObjectMod2 = 0x80,
    ObjectBsp  = 0x100,
};

using BspTag   = Components::TagType<BspReference>;
using ModelTag = Components::TagType<ModelReference>;

using BspStore   = Components::Allocators::VectorContainer<BspTag>;
using ModelStore = Components::Allocators::VectorContainer<ModelTag>;

template<typename T, typename IdType, typename... IType>
struct DataCache
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

struct BSPItem
{
    BSPItem() : mesh(nullptr)
    {
    }

    struct Mesh
    {
        blam::bsp::submesh_header const* mesh;
        GFX::D_DATA                      draw;
        generation_idx_t                 color_bitm;
        generation_idx_t                 light_bitm;
        WkPtr<GFX::PIP>                  shader;
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
        generation_idx_t                  color_bitm;
        blam::tag_t const*                shader;
    };
    struct LOD
    {
        blam::mod2::geometry_header const* header;
        Vector<SubModel>                   sub;
    };

    blam::mod2::header const* header;
    blam::tag_t const*        tag;
    LOD                       mesh;

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

struct SceneryItem
{
    blam::scn::scenery const* scenery;
    blam::tag_t const*        tag;
    ModelAssembly             model; /* Reference to cached model */

    inline bool valid() const
    {
        return scenery;
    }
};

struct VehicleItem
{
    blam::scn::vehicle const* vehicle;
    blam::tag_t const*        tag;
    ModelAssembly             model;

    inline bool valid() const
    {
        return vehicle;
    }
};

struct BitmapItem
{
    BitmapItem() : header(nullptr)
    {
    }

    struct SubImage
    {
    };

    blam::bitm::header_t const* header;
    blam::tag_t const*          shader_tag;
    blam::tag_t const*          tag;
    struct
    {
        blam::bitm::image_t const* mip;
        PixDesc                    fmt;
        bitm_format_hash           bucket;

        u32   index;
        Veci2 offset;
        Vecf2 scale;
    } image;

    inline bool valid() const
    {
        return header;
    }
};

struct BipedItem
{
    blam::scn::biped const* header;
    blam::tag_t const*      tag;
    ModelAssembly           model;

    inline bool valid() const
    {
        return header;
    }
};

struct EquipmentItem
{
    using equipment_type = blam::scn::equipment;

    equipment_type const* header;
    blam::tag_t const*    tag;
    ModelAssembly         model;

    inline bool valid() const
    {
        return header;
    }
};

struct MPEquipmentItem
{
    using equipment_type = blam::scn::multiplayer_equipment;

    equipment_type const* header;
    blam::tag_t const*    tag;
    ModelAssembly         model;

    inline bool valid() const
    {
        return header;
    }
};

struct WeaponItem
{
    using equipment_type = blam::scn::unit;

    equipment_type const* header;
    blam::tag_t const*    tag;
    ModelAssembly         model;

    inline bool valid() const
    {
        return header;
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

        auto img_data = img.image.mip->data(bitm_magic);
        auto img_size = img.image.mip->isize.convert<i32>();

        bucket.surface->upload(
            img.image.fmt,
            Size3(img_size.w, img_size.h, 1),
            img_data,
            Point3(
                img.image.offset.x(), img.image.offset.y(), img.image.index));
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
                    //                }
                    //                else if((offset.h + imsize.h) <=
                    //                pool.max.h)
                    //                {
                    //                    img_offset.w = 0;
                    //                    img_offset.h = offset.h;
                    //                    offset.w     = imsize.w;
                    //                    offset.h += imsize.h;
                } else
                {
                    layer++;
                    offset     = imsize;
                    img_offset = {};
                    img_layer  = layer;
                }

                img->image.index  = layer;
                img->image.offset = {img_offset.w, img_offset.h};
                img->image.scale  = {imsize.w / scalar(pool.max.w),
                                    imsize.h / scalar(pool.max.h)};

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
    }

    virtual BitmapItem predict_impl(
        blam::tagref_t const& shader, i16 idx) override
    {
        using blam::tag_class_t;

        if(!shader.valid())
            return {};

        auto it = index.find(shader);

        if(it == index.end())
        {
            cDebug(
                "Failed to find shader: {0}",
                shader.to_name().to_string(magic));
            return {};
        }

        blam::bitm::header_t const* bitm_ptr = nullptr;

        auto               curr_magic = magic;
        blam::tag_t const* bitm_tag   = nullptr;

        switch(shader.tag_class)
        {
        case tag_class_t::senv:
        {
            auto shader_env =
                (*it)->to_reflexive<blam::shader_env>().data(magic);

            if(!shader_env[0].bitmap.valid())
                return {};

            auto bitm_it = index.find(shader_env[0].bitmap);

            if(bitm_it == index.end())
                Throw(undefined_behavior("failed to locate bitmap"));

            bitm_tag = (*bitm_it);

            break;
        }
        case tag_class_t::schi:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_chicago>().data(magic);

            if(!shader_model[0].bitmap.valid())
                return {};

            auto bitm_it = index.find(shader_model[0].bitmap);

            if(bitm_it == index.end())
            {
                cDebug(
                    "Failed to find: {0}",
                    shader_model[0].bitmap.to_name().to_string(magic));
                return {};
            }

            bitm_tag = (*bitm_it);

            break;
        }
        case tag_class_t::scex:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_chicago_extended>().data(
                    magic);

            for(auto const& map : shader_model[0].maps.data(magic))
            {
                auto bitm_it = index.find(map.map5);

                if(bitm_it == index.end())
                {
                    break;
                    Throw(undefined_behavior("failed to locate map"));
                }

                bitm_tag = (*bitm_it);
            }

            break;
        }
        case tag_class_t::soso:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_model>().data(magic);

            auto bitm_it = index.find(shader_model[0].bitmap);

            if(bitm_it == index.end())
                Throw(undefined_behavior("failed to locate bitmap"));

            bitm_tag = (*bitm_it);

            break;
        }
        case tag_class_t::sgla:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_glass>().data(magic);

            if(!shader_model[0].bitmap2.valid())
                break;

            auto bitm_it = index.find(shader_model[0].bitmap2);

            if(bitm_it == index.end())
                Throw(undefined_behavior("failed to locate bitmap"));

            bitm_tag = (*bitm_it);

            cDebug("Glass shader");

            break;
        }
        case tag_class_t::swat:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_water>().data(magic);

            auto bitm_it = index.find(shader_model[0].bitmap);

            if(bitm_it == index.end())
                Throw(undefined_behavior("failed to locate bitmap"));

            bitm_tag = (*bitm_it);

            cDebug("Water shader");

            break;
        }
        case tag_class_t::smet:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_metal>().data(magic);

            auto bitm_it = index.find(shader_model[0].bitmap);

            if(bitm_it == index.end())
                Throw(undefined_behavior("failed to locate bitmap"));

            bitm_tag = (*bitm_it);

            cDebug("Metal shader");
            break;
        }
        case tag_class_t::bitm:
        {
            bitm_tag = (*it);

            break;
        }
        default:
            break;
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
            auto const& image = image_[0];

            auto& img = out.image;
            img.mip   = &image;
            img.index = 0;

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

            img.index = bucket.ptr;
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

struct BSPCache
    : DataCache<BSPItem, blam::bsp::info const*, blam::bsp::info const&>
{
    BSPCache(blam::map_container const& map, BitmapCache& bitmap_cache) :
        version(map.map->version), bitmap_cache(bitmap_cache), index(map),
        magic(map.magic), vert_ptr(0), element_ptr(0)
    {
    }

    blam::version_t      version;
    BitmapCache&         bitmap_cache;
    blam::tag_index_view index;
    blam::magic_data_t   magic;

    Bytes vert_buffer, element_buffer;
    u32   vert_ptr, element_ptr;

    virtual BSPItem predict_impl(blam::bsp::info const& bsp) override
    {
        auto        bsp_magic = bsp.bsp_magic(magic);
        auto const& section =
            bsp.to_bsp(bsp_magic).to_header().data(bsp_magic)[0];

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
                mesh_data.color_bitm = bitmap_cache.predict(mesh.shader, 0);
                mesh_data.light_bitm =
                    bitmap_cache.resolve(section.lightmaps, group.lightmap_idx);

                /* ... and moving on */

                if(version == blam::version_t::xbox)
                {
                    auto indices  = mesh.pc_indices(section).data(bsp_magic);
                    auto vertices = mesh.xbox_vertices().data(bsp_magic);

                    if(!vertices || !indices)
                    {
                        group_data.meshes.erase(--group_data.meshes.end());
                        continue;
                    }

                    using vertex_type =
                        std::remove_const<decltype(vertices)::value_type>::type;
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

                    vert_ptr += vertices.size;
                    element_ptr += indices.size;
                } else
                {
                    auto indices  = mesh.pc_indices(section).data(bsp_magic);
                    auto vertices = mesh.pc_vertices().data(bsp_magic);

                    using vertex_type =
                        std::remove_const<decltype(vertices)::value_type>::type;
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

                    vert_ptr += vertices.size;
                    element_ptr += indices.size;
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

    ModelCache(blam::map_container const& map, BitmapCache& bitm_cache) :
        version(map.map->version), tags(map.tags), index(map), magic(map.magic),
        vertex_magic(
            map.map->version == blam::version_t::xbox
                ? map.magic
                : tags->vertex_magic(magic)),
        bitm_cache(bitm_cache), vert_ptr(0), element_ptr(0)
    {
    }

    blam::version_t          version;
    blam::tag_index_t const* tags;
    blam::tag_index_view     index;
    blam::magic_data_t       magic;
    blam::magic_data_t       vertex_magic;
    BitmapCache&             bitm_cache;

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
        out.mesh   = {};
        out.header = header;
        out.tag    = *index.find(mod2);

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
                draw_data.color_bitm =
                    bitm_cache.predict(shaders[model.data.shader_idx].ref, 0);
                draw_data.shader =
                    *index.find(shaders[model.data.shader_idx].ref.to_plain());

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

template<typename Version>
struct SceneryCache : DataCache<SceneryItem, u32, blam::tagref_t const&>
{
    SceneryCache(
        blam::map_container const& map, ModelCache<Version>& model_cache) :
        model_cache(model_cache),
        magic(map.magic), index(map)
    {
    }

    ModelCache<Version>& model_cache;
    blam::magic_data_t   magic;
    blam::tag_index_view index;

    virtual SceneryItem predict_impl(blam::tagref_t const& tag) override
    {
        auto it = index.find(tag);

        if(it == index.end())
            return {};

        auto scenery =
            (*it)->template to_reflexive<blam::scn::scenery>().data(magic);

        if(scenery.elements != 1)
            Throw(undefined_behavior("no scenery found"));

        SceneryItem out;
        out.scenery = &scenery[0];
        out.model   = model_cache.predict_regions(scenery[0].model.to_plain());

        if(!out.model.models.size())
            return {};

        return out;
    }

    virtual u32 get_id(blam::tagref_t const& tag) override
    {
        return tag.tag_id;
    }
};

template<typename Version>
struct VehicleCache : DataCache<VehicleItem, u32, blam::tagref_t const&>
{
    VehicleCache(
        blam::map_container const& map, ModelCache<Version>& model_cache) :
        model_cache(model_cache),
        magic(map.magic), index(map)
    {
    }

    ModelCache<Version>& model_cache;
    blam::magic_data_t   magic;
    blam::tag_index_view index;

    virtual VehicleItem predict_impl(blam::tagref_t const& vehi) override
    {
        auto it = index.find(vehi);

        if(it == index.end())
            return {};

        auto vehicle =
            (*it)->template to_reflexive<blam::scn::vehicle>().data(magic);

        if(!vehicle[0].model.valid())
            return {};

        VehicleItem out;
        out.vehicle = &vehicle[0];
        out.model   = model_cache.predict_regions(vehicle[0].model.to_plain());

        return out;
    }
    virtual u32 get_id(blam::tagref_t const& tag) override
    {
        return tag.tag_id;
    }
};

template<typename Version>
struct BipedCache : DataCache<BipedItem, u32, blam::tagref_t const&>
{
    BipedCache(
        blam::map_container const& map, ModelCache<Version>& model_cache) :
        magic(map.magic),
        index(map), model_cache(model_cache)
    {
    }

    blam::magic_data_t   magic;
    blam::tag_index_view index;
    ModelCache<Version>& model_cache;

    virtual BipedItem predict_impl(blam::tagref_t const& bipd) override
    {
        auto it = index.find(bipd);

        if(it == index.end())
            return {};

        auto biped =
            (*it)->template to_reflexive<blam::scn::biped>().data(magic);

        BipedItem out;
        out.header = &biped[0];
        out.model  = model_cache.predict_regions(biped[0].model.to_plain());

        return out;
    }
    virtual u32 get_id(blam::tagref_t const& tag) override
    {
        return tag.tag_id;
    }
};

template<typename T, typename Version>
struct EquipmentCache : DataCache<T, u32, blam::tagref_t const&>
{
    EquipmentCache(
        blam::map_container const& map, ModelCache<Version>& model_cache) :
        magic(map.magic),
        index(map), model_cache(model_cache)
    {
    }

    blam::magic_data_t   magic;
    blam::tag_index_view index;
    ModelCache<Version>& model_cache;

    virtual T predict_impl(blam::tagref_t const& equip) override
    {
        auto it = index.find(equip);

        if(it == index.end())
            return {};

        auto equipment =
            (*it)->template to_reflexive<typename T::equipment_type>().data(
                magic);

        T out;
        out.header = &equipment[0];
        out.model  = model_cache.predict_regions(equipment[0].model.to_plain());

        return out;
    }

    virtual u32 get_id(blam::tagref_t const& equip) override
    {
        return equip.tag_id;
    }
};

template<typename Version>
struct BlamData
{
    BlamData() :
#if defined(COFFEE_ANDROID) || 1
        map_file(MkUrl("c10.map")), bitmap_file("bitmaps.map"_rsc),
#else
        map_file(MkUrl(GetInitArgs().arguments().at(0), RSCA::SystemFile)),
        bitmap_file(Resource(MkUrl(
            Path(GetInitArgs().arguments().at(0)).dirname() + "bitmaps.map",
            RSCA::SystemFile))),
#endif
        map_container(map_file, Version()),
        bitm_cache(
            map_container,
            blam::magic_data_t(C_OCAST<Bytes>(bitmap_file)),
            nullptr),
        bsp_cache(map_container, bitm_cache),
        model_cache(map_container, bitm_cache),
        scenery_cache(map_container, model_cache),
        vehicle_cache(map_container, model_cache),
        biped_cache(map_container, model_cache),
        equip_cache(map_container, model_cache),
        weap_cache(map_container, model_cache),
        std_camera(MkShared<std_camera_t>(&camera, &camera_opts)),
        controller_camera(&camera, &controller_opts)
    {
    }

    /* Blam! data */
    Resource                                      map_file;
    Resource                                      bitmap_file;
    blam::map_container                           map_container;
    blam::scn::scenario<blam::hsc::bc::v2> const* scenario;

    BitmapCache bitm_cache;

    BSPCache            bsp_cache;
    Vector<GFX::D_DATA> bsps;
    ShPtr<GFX::BUF_A>   bsp_buf;
    ShPtr<GFX::BUF_E>   bsp_index;
    ShPtr<GFX::V_DESC>  bsp_attr;
    PIP_PARAM*          bsp_pipeline;
    GFX::RenderPass     bsp_pass;
    GFX::OPT_DRAW       bsp_draw;

    ModelCache<Version>   model_cache;
    SceneryCache<Version> scenery_cache;
    VehicleCache<Version> vehicle_cache;
    BipedCache<Version>   biped_cache;

    EquipmentCache<EquipmentItem, Version> equip_cache;
    EquipmentCache<WeaponItem, Version>    weap_cache;
    //    EquipmentCache<MPEquipmentItem> mp_equip_cache;

    ShPtr<GFX::BUF_A>  model_buf;
    ShPtr<GFX::BUF_E>  model_index;
    ShPtr<GFX::V_DESC> model_attr;
    PIP_PARAM*         model_pipeline;

    Vector<Matf4>     model_mats;
    GFX::RenderPass   model_pass;
    GFX::OPT_DRAW     model_draw;
    ShPtr<GFX::BUF_S> model_matrix_store;
    ShPtr<GFX::BUF_S> material_store;

    camera_t            camera;
    Matf4               camera_matrix;
    StandardCameraOpts  camera_opts;
    ShPtr<std_camera_t> std_camera;

    ControllerOpts                               controller_opts;
    ControllerCamera<camera_t*, ControllerOpts*> controller_camera;
};

template<typename Version>
struct MeshRenderer : Components::RestrictedSubsystem<
                          Components::TagType<MeshRenderer<Version>>,
                          type_list_t<BspTag, ModelTag>,
                          empty_list_t>
{
    using parent_type = Components::RestrictedSubsystem<
        Components::TagType<MeshRenderer<Version>>,
        type_list_t<BspTag, ModelTag>,
        empty_list_t>;

    using Proxy = typename parent_type::Proxy;

    struct Pass
    {
        Pass()
        {
        }

        GFX::RenderPass source;
        GFX::OPT_DRAW   draw;

        decltype(source.draws)& draws()
        {
            return source.draws;
        }

        inline void clear()
        {
            source.draws.clear();
            draw = {};
        }
    };

    enum Passes
    {
        Pass_Opaque,
        Pass_Metal,
        Pass_LastOpaque,
        Pass_Glass = Pass_LastOpaque,

        Pass_Count,
    };

    MeshRenderer(BlamData<Version>& data) : m_data(data)
    {
        bsp[Pass_Opaque].source.pipeline = data.bsp_pipeline->pipeline_ptr();
        bsp[Pass_Metal].source.pipeline  = data.bsp_pipeline->pipeline_ptr();
        bsp[Pass_Glass].source.pipeline  = data.bsp_pipeline->pipeline_ptr();

        model[Pass_Opaque].source.pipeline =
            data.model_pipeline->pipeline_ptr();
        model[Pass_Metal].source.pipeline = data.model_pipeline->pipeline_ptr();
        model[Pass_Glass].source.pipeline = data.model_pipeline->pipeline_ptr();

        bsp[Pass_Glass].source.blend.m_doBlend   = true;
        model[Pass_Glass].source.blend.m_doBlend = true;
    }

    MeshRenderer<Version> const& get() const
    {
        return *this;
    }
    MeshRenderer<Version>& get()
    {
        return *this;
    }

    void end_restricted(Proxy& p, time_point const& time)
    {
        if(time - last_update > Chrono::seconds(1))
            update_draws(p, time);

        for(auto const& pass : slice_num(bsp, Pass_LastOpaque))
        {
            GFX::SetBlendState(pass.source.blend);
            GFX::MultiDraw(*pass.source.pipeline.lock(), pass.draw);
        }
        for(auto const& pass : slice_num(model, Pass_LastOpaque))
        {
            GFX::SetBlendState(pass.source.blend);
            GFX::MultiDraw(*pass.source.pipeline.lock(), pass.draw);
        }
    }

    void update_draws(Proxy& p, time_point const&)
    {
        for(auto& pass : bsp)
            pass.clear();
        for(auto& pass : model)
            pass.clear();

        /* Update draws */
        for(auto& ent : p.select(ObjectBsp))
        {
            auto  ref     = p.template ref<Proxy>(ent);
            auto& bsp_ref = ref.template get<BspTag>();

            if(!bsp_ref.visible)
                continue;

            bsp_ref.draw_idx = bsp[Pass_Opaque].draws().size();
            bsp[Pass_Opaque].draws().push_back(
                {m_data.bsp_attr,
                 &m_data.bsp_pipeline->get_state(),
                 bsp_ref.draw.call,
                 bsp_ref.draw.draw});
        }

        for(auto& ent : p.select(ObjectMod2))
        {
            auto  ref     = p.template ref<Proxy>(ent);
            auto& mod_ref = ref.template get<ModelTag>();

            mod_ref.draw_idx = model[Pass_Opaque].draws().size();
            model[Pass_Opaque].draws().push_back(
                {m_data.model_attr,
                 &m_data.model_pipeline->get_state(),
                 mod_ref.draw.call,
                 mod_ref.draw.draw});
        }

        GFX::OptimizeRenderPass(bsp[Pass_Opaque].source, bsp[Pass_Opaque].draw);
        GFX::OptimizeRenderPass(
            model[Pass_Opaque].source, model[Pass_Opaque].draw);

        /* Update transforms and texture references */

        struct Material
        {
            Vecf2 scale;
            Veci2 offset;
            u32   source;
            u32   layer;
            u32   pad_[2];
        };

        auto material_store =
            m_data.material_store->map().template as<Material>();

        auto model_material_store = material_store.at(2_MB);
        material_store            = material_store.at(0, 2_MB);

        auto matrix_store =
            m_data.model_matrix_store->map().template as<Matf4>();

        for(auto& ent : p.select(ObjectMod2))
        {
            auto            ref     = p.template ref<Proxy>(ent);
            ModelReference& mod_ref = ref.template get<ModelTag>();
            auto&           draw_data =
                model[Pass_Opaque].draws().at(mod_ref.draw_idx).d_data;

            matrix_store[draw_data.m_ioff] = mod_ref.transform;

            Material& mat = model_material_store[draw_data.m_ioff];

            if(!mod_ref.bitmap.valid())
            {
                mat.layer  = 0;
                mat.offset = {};
                mat.scale  = {1, 1};
                mat.source = 0;
                continue;
            }

            BitmapItem& bitmap = m_data.bitm_cache.find(mod_ref.bitmap)->second;

            mat.layer  = bitmap.image.index;
            mat.offset = bitmap.image.offset;
            mat.scale  = bitmap.image.scale;

            auto comp_flags = std::get<3>(bitmap.image.bucket);
            switch(comp_flags)
            {
            case CompFlags::DXT1:
                mat.source = 0;
                break;
            case CompFlags::DXT3:
                mat.source = 1;
                break;
            case CompFlags::DXT5:
                mat.source = 2;
                break;
            }

            ModelItem const& model_ =
                m_data.model_cache.find(mod_ref.model)->second;

            //            cDebug(
            //                "Model: {0},{1} : {2} : {3} -> {4}@{5}+{6},{7} ->
            //                {8}",
            //                model_.tag->to_name().to_string(m_data.map_container.magic),
            //                "[???]",
            //                bitmap.shader_tag->to_name().to_string(
            //                    m_data.map_container.magic),
            //                bitmap.tag->to_name().to_string(m_data.map_container.magic),
            //                bitmap.image.index,
            //                magic_enum::enum_name(bitmap.image.fmt.cmpflg),
            //                bitmap.image.offset.x(),
            //                bitmap.image.offset.y(),
            //                mat.source);
        }

        for(auto& ent : p.select(ObjectBsp))
        {
            auto          ref     = p.template ref<Proxy>(ent);
            BspReference& bsp_ref = ref.template get<BspTag>();

            auto& draw_data =
                bsp[Pass_Opaque].draws().at(bsp_ref.draw_idx).d_data;

            if(!bsp_ref.bitmap.valid())
                continue;

            Material&   mat    = material_store[draw_data.m_ioff];
            BitmapItem& bitmap = m_data.bitm_cache.find(bsp_ref.bitmap)->second;

            mat.layer  = bitmap.image.index;
            mat.offset = bitmap.image.offset;
            mat.scale  = bitmap.image.scale;

            auto comp_flags = std::get<3>(bitmap.image.bucket);
            switch(comp_flags)
            {
            case CompFlags::DXT1:
                mat.source = 0;
                break;
            case CompFlags::DXT3:
                mat.source = 1;
                break;
            case CompFlags::DXT5:
                mat.source = 2;
                break;
            }
        }

        m_data.material_store->unmap();
        m_data.model_matrix_store->unmap();
    }

    time_point last_update;

    BlamData<Version>& m_data;

    Array<Pass, Pass_Count> bsp;
    Array<Pass, Pass_Count> model;
};

template<typename Version>
void create_resources(EntityContainer& e, BlamData<Version>& data)
{
    ProfContext _(__PRETTY_FUNCTION__);

    {
        using namespace Display::EventHandlers;
        auto eventhandler =
            e.service<comp_app::BasicEventBus<Input::CIEvent>>();

        eventhandler->addEventHandler(
            0,
            ExitOn<OnKey<Input::CK_Escape>>(
                e.service_ref<comp_app::Windowing>()));

        eventhandler->addEventHandler(
            0, std_camera_t::KeyboardInput(data.std_camera));
        eventhandler->addEventHandler(
            0, std_camera_t::MouseInput(&data.camera));

        auto eventhandler_w =
            e.service<comp_app::BasicEventBus<Display::Event>>();

        eventhandler_w->addEventHandler(0, WindowResize<GFX>());
    }

    /* Set up graphics data */
    GFX::OPTS options;
    options.crash_on_error        = true;
    options.old_shader_processing = false;
    auto& gfx = e.register_subsystem_inplace<GFX_ALLOC::tag_type, GFX_ALLOC>(
        options, true);

    data.bsp_buf = gfx.alloc_buffer<GFX::BUF_A>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);
    data.bsp_index = gfx.alloc_buffer<GFX::BUF_E>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);

    data.model_buf = gfx.alloc_buffer<GFX::BUF_A>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);
    data.model_index = gfx.alloc_buffer<GFX::BUF_E>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);

    data.model_matrix_store = gfx.alloc_buffer<GFX::BUF_S>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, sizeof(Matf4), 0);

    data.material_store = gfx.alloc_buffer<GFX::BUF_S>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable,
        sizeof(Vecf2) + sizeof(Veci2) + sizeof(u32),
        0);

    data.bsp_buf->commit(memory_budget::bsp_buffer);
    data.bsp_index->commit(memory_budget::bsp_elements);

    data.model_buf->commit(memory_budget::mesh_buffer);
    data.model_index->commit(memory_budget::mesh_elements);

    data.model_matrix_store->commit(memory_budget::matrix_buffer);
    data.material_store->commit(memory_budget::material_buffer);

    GFX::V_ATTR pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr;
    pos_attr.m_idx = 0;
    tex_attr.m_idx = 1;
    nrm_attr.m_idx = 2;
    bin_attr.m_idx = 3;
    tan_attr.m_idx = 4;
    if(data.map_container.map->is_xbox())
    {
        pos_attr.m_off = offsetof(xbox_vertex_type, position);
        tex_attr.m_off = offsetof(xbox_vertex_type, texcoord);
        nrm_attr.m_off = offsetof(xbox_vertex_type, normal);
        bin_attr.m_off = offsetof(xbox_vertex_type, binorm);
        tan_attr.m_off = offsetof(xbox_vertex_type, tangent);
    } else
    {
        pos_attr.m_off = offsetof(vertex_type, position);
        tex_attr.m_off = offsetof(vertex_type, texcoord);
        nrm_attr.m_off = offsetof(vertex_type, normal);
        bin_attr.m_off = offsetof(vertex_type, binorm);
        tan_attr.m_off = offsetof(vertex_type, tangent);
    }
    tex_attr.m_size = 2;

    pos_attr.m_size = nrm_attr.m_size = bin_attr.m_size = tan_attr.m_size = 3;
    if(data.map_container.map->is_xbox())
    {
        pos_attr.m_type = tex_attr.m_type = semantic::TypeEnum::Scalar;
        nrm_attr.m_type = bin_attr.m_type = tan_attr.m_type =
            semantic::TypeEnum::Packed_UFloat;

        pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
            bin_attr.m_stride = tan_attr.m_stride = sizeof(xbox_vertex_type);
    } else
    {
        pos_attr.m_type = tex_attr.m_type = nrm_attr.m_type = bin_attr.m_type =
            tan_attr.m_type = semantic::TypeEnum::Scalar;
        pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
            bin_attr.m_stride = tan_attr.m_stride = sizeof(vertex_type);
    }
    pos_attr.m_bassoc = tex_attr.m_bassoc = nrm_attr.m_bassoc =
        bin_attr.m_bassoc = tan_attr.m_bassoc = 0;

    data.bsp_attr =
        gfx.alloc_desc<5>({{pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr}});
    data.bsp_attr->setIndexBuffer(data.bsp_index.get());
    data.bsp_attr->bindBuffer(0, *data.bsp_buf);

    using model_vertex_type = blam::vert::mod2_vertex<blam::vert::uncompressed>;
    using xbox_model_vertex_type =
        blam::vert::mod2_vertex<blam::vert::compressed>;

    if(data.map_container.map->is_xbox())
    {
        tex_attr.m_type = semantic::TypeEnum::Short;
        tex_attr.m_flags =
            GLEAMAPI::AttributePacked | GLEAMAPI::AttributeNormalization;

        pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
            bin_attr.m_stride                 = tan_attr.m_stride =
                sizeof(xbox_model_vertex_type);
    } else
    {
        pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
            bin_attr.m_stride = tan_attr.m_stride = sizeof(model_vertex_type);
    }

    data.model_attr =
        gfx.alloc_desc<5>({{pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr}});
    data.model_attr->setIndexBuffer(data.model_index.get());
    data.model_attr->bindBuffer(0, *data.model_buf);

    data.bitm_cache.allocator = &gfx;
}

template<typename Version>
void load_scenario_bsp(EntityContainer& e, BlamData<Version>& data)
{
    ProfContext _(__PRETTY_FUNCTION__);

    using namespace Components;

    auto& magic = data.map_container.magic;

    data.bsp_cache.vert_buffer    = data.bsp_buf->map();
    data.bsp_cache.element_buffer = data.bsp_index->map();

    Vector<generation_idx_t> bsp_meshes;

    /* Start loading up vertex data */
    auto scenario = data.scenario;
    for(auto const& bsp : scenario->bsp_info.data(magic))
    {
        bsp_meshes.push_back(data.bsp_cache.predict(bsp));
    }

    data.bsp_buf->unmap();
    data.bsp_index->unmap();

    auto& bsp_pass       = data.bsp_pass;
    bsp_pass.pipeline    = data.bsp_pipeline->pipeline_ptr();
    bsp_pass.framebuffer = GFX::DefaultFramebuffer();

    EntityRecipe bsp;
    bsp.components = {type_hash_v<BspTag>()};
    bsp.tags       = ObjectBsp;

    for(auto const& mesh_id : bsp_meshes)
    {
        if(!mesh_id.valid())
            continue;

        auto const& mesh = data.bsp_cache.find(mesh_id)->second;

        for(auto const& group : mesh.groups)
            for(BSPItem::Mesh const& mesh : group.meshes)
            {
                auto          mesh_ent = e.ref(e.create_entity(bsp));
                BspReference& bsp_ref  = mesh_ent.get<BspTag>();

                bsp_ref.bitmap    = mesh.color_bitm;
                bsp_ref.bsp       = mesh_id;
                bsp_ref.visible   = true;
                bsp_ref.draw.draw = mesh.draw;
                bsp_ref.draw.call =
                    GFX::D_CALL().withIndexing().withInstancing();
            }
    }

    GFX::OptimizeRenderPass(bsp_pass, data.bsp_draw);
}

template<typename T, typename CacheType, typename Version>
void load_unit_group(
    CacheType&                   cache,
    blam::reflex_group<T> const& group,
    blam::magic_data_t const&    magic,

    BlamData<Version>& data,

    EntityContainer& e,
    u32              tags)
{
    ProfContext _(__PRETTY_FUNCTION__);

    using namespace Components;

    EntityRecipe unit;
    unit.components = {type_hash_v<ModelTag>()};
    unit.tags       = tags | ObjectMod2;

    Map<i16, generation_idx_t> objects;
    i16                        id = 0;
    for(auto const& object : group.ref.data(magic))
    {
        auto it = cache.predict(object[0]);

        if(!it.valid())
            continue;

        objects.insert({id++, it});
    }

    for(auto const& instance : group.base.data(magic))
    {
        auto obj_ref = objects.find(instance.ref);

        auto const& object = cache.find(obj_ref->second);

        if(object == cache.m_cache.end())
            continue;

        ModelAssembly const& mesh_data = object->second.model;

        for(auto const& region : mesh_data.models)
            for(auto const& lod : region)
            {
                if(!lod.valid())
                    continue;

                auto const& model = data.model_cache.find(lod)->second.mesh;

                for(ModelItem::SubModel const& mesh : model.sub)
                {
                    auto mesh_ent = e.ref(e.create_entity(unit));

                    ModelReference& model = mesh_ent.get<ModelTag>();

                    model.transform =
                        typing::vectors::translation(Matf4(), instance.pos) *
                        typing::vectors::matrixify(
                            typing::vectors::normalize_quat(Quatf(
                                1, 0, instance.rot.y(), -instance.rot.x())));
                    model.draw.draw = mesh.draw;
                    model.model     = obj_ref->second;
                    model.instance =
                        C_RCAST<decltype(model.instance)>(&instance);
                    model.draw.call = GFX::D_CALL()
                                          .withIndexing()
                                          .withTriStrip()
                                          .withInstancing();
                    model.bitmap = mesh.color_bitm;
                    model.instance =
                        C_RCAST<decltype(model.instance)>(&instance);
                }
            }
    }
}

template<typename Version>
void load_scenario_scenery(EntityContainer& e, BlamData<Version>& data)
{
    ProfContext _(__PRETTY_FUNCTION__);

    {
        ProfContext _("Buffer mapping");
        data.model_cache.vert_buffer    = data.model_buf->map();
        data.model_cache.element_buffer = data.model_index->map();
    }

    auto scenario = data.scenario;
    auto magic    = data.map_container.magic;

    auto pipeline = data.model_pipeline;

    pipeline->build_state();

    load_unit_group(
        data.scenery_cache, scenario->scenery, magic, data, e, ObjectScenery);
    load_unit_group(
        data.vehicle_cache, scenario->vehicles, magic, data, e, ObjectVehicle);
    load_unit_group(
        data.biped_cache, scenario->bipeds, magic, data, e, ObjectBiped);
    load_unit_group(
        data.equip_cache, scenario->equips, magic, data, e, ObjectEquipment);
    load_unit_group(
        data.weap_cache,
        scenario->weapon_spawns,
        magic,
        data,
        e,
        ObjectEquipment);

    data.model_buf->unmap();
    data.model_index->unmap();
}

i32 blam_main(i32, cstring_w*)
{
    auto blam_queue = RuntimeQueue::CreateNewQueue("Blam Graphics!");

    comp_app::app_error app_ec;
    auto&               e = comp_app::createContainer();
    comp_app::configureDefaults(e);

    auto loader = e.service<comp_app::AppLoader>();

    cDebug(
        "Buffer budget: {0} / {1} MB",
        memory_budget::grand_total,
        memory_budget::grand_total / Unit_MB);

    comp_app::addDefaults(e, *e.service<comp_app::AppLoader>(), app_ec);
    comp_app::AppContainer<BlamData<halo_version>>::addTo(
        e,
        [](EntityContainer&        e,
           BlamData<halo_version>& data,
           time_point const&) {
            ProfContext _(__PRETTY_FUNCTION__);

            e.register_component_inplace<ModelStore>();
            e.register_component_inplace<BspStore>();

            if(!FileExists(data.map_file))
                Throw(undefined_behavior("map file not found"));

            blam::tag_index_view index(data.map_container);

            auto& magic = data.map_container.magic;

            data.scenario =
                data.map_container.tags->scenario(data.map_container.map)
                    .to_reflexive<blam::scn::scenario<blam::hsc::bc::v2>>()
                    .data(magic)
                    .data;
            {
                auto map_name      = data.map_container.map->full_mapname();
                auto window_config = e.service<comp_app::WindowInfo>();
                window_config->setName(
                    window_config->name() + " : " + map_name);
            }

            create_resources(e, data);

            auto& gfx      = e.subsystem_cast<GFX_ALLOC>();
            auto& pipeline = gfx.alloc_standard_pipeline<2>(
                {{"map.vert"_rsc, "map.frag"_rsc}});

            auto& scenery_pipeline = gfx.alloc_standard_pipeline<2>(
                {{"scenery.vert"_rsc, "scenery.frag"_rsc}});

            data.bsp_pipeline   = &pipeline;
            data.model_pipeline = &scenery_pipeline;

            pipeline.build_state();
            pipeline.get_state();
            data.model_pipeline->build_state();
            data.model_pipeline->get_state();

            load_scenario_bsp(e, data);
            load_scenario_scenery(e, data);

            {
                ProfContext _("Texture allocation");
                data.bitm_cache.allocate_storage();
            }

            GFX::ERROR ec;
            data.model_matrix_store->bindrange(0, 0, 4_MB, ec);
            data.material_store->bindrange(1, 0, 2_MB, ec);
            data.material_store->bindrange(2, 2_MB, 2_MB, ec);

            {
                auto pipeline = data.model_pipeline;
                pipeline->set_constant(
                    "camera", Bytes::From(data.camera_matrix));

                pipeline->set_sampler(
                    "bc1_tex",
                    data.bitm_cache
                        .get_bucket(
                            PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT1)))
                        .sampler->handle()
                        .bind(0));
                pipeline->set_sampler(
                    "bc3_tex",
                    data.bitm_cache
                        .get_bucket(
                            PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT3)))
                        .sampler->handle()
                        .bind(1));
                pipeline->set_sampler(
                    "bc5_tex",
                    data.bitm_cache
                        .get_bucket(
                            PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT5)))
                        .sampler->handle()
                        .bind(2));

                pipeline->build_state();
                pipeline->get_state();
            }

            pipeline.set_constant("view", Bytes::From(data.camera_matrix));
            pipeline.set_sampler(
                "bc1_tex",
                data.bitm_cache
                    .get_bucket(PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT1)))
                    .sampler->handle()
                    .bind(0));
            pipeline.set_sampler(
                "bc3_tex",
                data.bitm_cache
                    .get_bucket(PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT3)))
                    .sampler->handle()
                    .bind(1));
            pipeline.set_sampler(
                "bc5_tex",
                data.bitm_cache
                    .get_bucket(PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT5)))
                    .sampler->handle()
                    .bind(2));

            pipeline.build_state();
            pipeline.get_state();

            e.register_subsystem_inplace<
                Components::TagType<MeshRenderer<halo_version>>,
                MeshRenderer<halo_version>>(std::ref(data));

            GFX::RASTSTATE cull_disable;
            cull_disable.m_doCull  = true;
            cull_disable.m_culling = (u32)typing::graphics::VertexFace::Front;
            GFX::SetRasterizerState(cull_disable);

            GFX::DEPTSTATE depth_enable;
            depth_enable.m_test = true;
            GFX::SetDepthState(depth_enable);
        },
        [](EntityContainer&        e,
           BlamData<halo_version>& data,
           time_point const&,
           duration const&) {
            GFX::DefaultFramebuffer()->clear(0, {0.5f, 0.5f, 0.9f, 1.f}, 1.0);

            data.std_camera->tick();
            if(e.service<comp_app::ControllerInput>())
                data.controller_camera(
                    e.service<comp_app::ControllerInput>()->state(0));

            using namespace typing::vectors::scene;
            {
                data.camera.aspect =
                    e.service<comp_app::Windowing>()->size().aspect();
                data.camera.zVals = {0.01f, 1000.f};

                data.camera_matrix =
                    GenPerspective(data.camera) * GenTransform(data.camera) *
                    typing::vectors::scale(Matf4(), {10}) *
                    typing::vectors::matrixify(typing::vectors::normalize_quat(
                        Quatf(1, -pi / 4, 0, 0)));
            }
        },
        [](EntityContainer&        e,
           BlamData<halo_version>& data,
           time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN(blam_main)
