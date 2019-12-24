#include <blam/cblam.h>
#include <coffee/comp_app/AppWrap>
#include <coffee/comp_app/gl_config.h>
#include <coffee/core/CApplication>
#include <coffee/core/CArgParser>
#include <coffee/core/CFiles>
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
using vertex_type = blam::vert::vertex<blam::vert::uncompressed>;
using Components::ComponentRef;
using Components::EntityContainer;

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

using bitm_format_hash = Tup<u32, u32, PixFmt, PixCmp, BitFmt, CompFlags>;

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

    auto find(generation_idx_t id) const
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
        WkPtr<GFX::PIP>                   shader;
    };
    struct LOD
    {
        blam::mod2::geometry_header const* header;
        Vector<SubModel>                   sub;
    };

    blam::mod2::header const* header;
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
    ModelAssembly             model; /* Reference to cached model */

    inline bool valid() const
    {
        return scenery;
    }
};

struct VehicleItem
{
    blam::scn::vehicle const* vehicle;
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
    struct
    {
        blam::bitm::image_t const* mip;
        PixDesc                    fmt;
        u32                        index;
        bitm_format_hash           bucket;

        Vecf2 offset;
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
        Size               size;
    };

    BitmapCache(
        blam::map_container const& map,
        blam::magic_data_t const&  bitmap_magic,
        GFX_ALLOC*                 allocator) :
        magic(map.magic),
        bitm_magic(bitmap_magic), index(map),
        bitm_header(blam::bitm::bitmap_header_t::from_data(
            Bytes::From(bitmap_magic.base_ptr, bitmap_magic.max_size))),
        allocator(allocator)
    {
    }

    blam::magic_data_t                 magic;
    blam::magic_data_t                 bitm_magic;
    blam::tag_index_view               index;
    blam::bitm::bitmap_header_t const* bitm_header;
    GFX_ALLOC*                         allocator;

    Map<bitm_format_hash, TextureBucket> tex_buckets;

    static inline bitm_format_hash create_hash(
        blam::bitm::image_t const& img, PixDesc const& fmt)
    {
        return std::make_tuple(
            img.isize.w,
            img.isize.h,
            fmt.pixfmt,
            fmt.comp,
            fmt.bfmt,
            fmt.cmpflg);
    }

    TextureBucket& get_bucket(
        blam::bitm::image_t const& img, PixDesc const& fmt)
    {
        auto hash = create_hash(img, fmt);

        auto it = tex_buckets.find(hash);

        if(it != tex_buckets.end())
            return it->second;

        auto& bucket = tex_buckets.insert({hash, {}}).first->second;

        bucket.ptr  = 0;
        bucket.fmt  = fmt;
        bucket.size = img.isize;
        std::tie(bucket.sampler, bucket.surface) =
            allocator->alloc_surface_sampler<GFX::S_2DA>(
                fmt.pixfmt, 1, C_CAST<u32>(fmt.cmpflg) << 10);

        return bucket;
    }

    void commit_bitmap(BitmapItem const& img)
    {
        auto& bucket = get_bucket(*img.image.mip, img.image.fmt);

        auto img_data = img.image.mip->data(bitm_magic);
        auto img_size = img.image.mip->isize.convert<i32>();

        bucket.surface->upload(
            img.image.fmt,
            Size3(img_size.w, img_size.h, 1),
            img_data,
            Point3(0, 0, img.image.index));
    }

    void allocate_storage()
    {
        Map<bitm_format_hash, u32> count;

        for(auto const& bitm : m_cache)
            count[bitm.second.image.bucket]++;

        for(auto& bucket : tex_buckets)
        {
            auto& props = bucket.second;
            props.surface->allocate(
                Size3(props.size.w, props.size.h, count[bucket.first]),
                props.fmt.comp);
        }

        for(auto const& bitm : m_cache)
            commit_bitmap(bitm.second);
    }

    virtual BitmapItem predict_impl(
        blam::tagref_t const& shader, i16 idx) override
    {
        using blam::tag_class_t;

        auto it = index.find(shader);

        if(it == index.end())
            return {};

        blam::bitm::header_t const* bitm_ptr = nullptr;

        auto               curr_magic = magic;
        blam::tag_t const* bitm_tag   = nullptr;

        switch(shader.tag_class)
        {
        case tag_class_t::senv:
        {
            auto shader_env =
                (*it)->to_reflexive<blam::shader_env>().data(magic);

            auto bitm_it = index.find(shader_env[0].bitmap);

            if(bitm_it == index.end())
                return {};

            bitm_tag = (*bitm_it);

            break;
        }
        case tag_class_t::soso:
        {
            auto shader_model =
                (*it)->to_reflexive<blam::shader_model>().data(magic);

            auto bitm_it = index.find(shader_model[0].bitmap);

            if(bitm_it == index.end())
                return {};

            bitm_tag = (*bitm_it);

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
            return {};

        if(bitm_tag->storage == blam::image_storage_t::external)
        {
            bitm_ptr =
                bitm_header->get_block(bitm_tag->offset).data(bitm_magic).data;
            curr_magic = bitm_header->block_magic(bitm_magic, bitm_tag->offset);
        } else
            bitm_ptr =
                bitm_tag->to_reflexive<blam::bitm::header_t>().data(magic).data;

        if(!bitm_ptr)
            return {};

        auto const& bitm = *bitm_ptr;

        BitmapItem out;
        out.header = &bitm;

        GFX::DBG::SCOPE _("BitmapCache::predict_impl");

        auto const& image = bitm.images.data(curr_magic)[idx];
        {
            auto& img = out.image;
            img.mip   = &image;
            img.index = 0;

            PixDesc fmt;
            if(image.compressed())
            {
                std::tie(fmt.pixfmt, fmt.cmpflg) = image.to_compressed_fmt();
            } else
            {
                fmt.pixfmt                   = image.to_pixfmt();
                std::tie(fmt.bfmt, fmt.comp) = image.to_fmt();
            }

            auto& bucket = get_bucket(image, fmt);
            img.bucket   = create_hash(image, fmt);
            img.fmt      = fmt;

            img.index = bucket.ptr;
            bucket.ptr++;
        }

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
        bitmap_cache(bitmap_cache), index(map), magic(map.magic), vert_ptr(0),
        element_ptr(0)
    {
    }

    BitmapCache&         bitmap_cache;
    blam::tag_index_view index;
    blam::magic_data_t   magic;

    Bytes vert_buffer, element_buffer;
    u32   vert_ptr, element_ptr;

    virtual BSPItem predict_impl(blam::bsp::info const& bsp) override
    {
        auto bsp_magic = bsp.bsp_magic(magic);
        auto section   = bsp.to_bsp(bsp_magic).to_header().data(bsp_magic)[0];

        BSPItem out;
        out.mesh = &section;

        auto shader = section.shaders.data(bsp_magic);

        for(auto const& group : section.submesh_groups.data(bsp_magic))
        {
            auto& group_data = out.groups.emplace_back();
            group_data.group = &group;
            for(auto const& mesh : group.material.data(bsp_magic))
            {
                auto& mesh_data = group_data.meshes.emplace_back();

                /* Just dig up the textures, long process */
                mesh_data.color_bitm = bitmap_cache.predict(mesh.shader, 0);
                //                mesh_data.light_bitm =
                //                    bitmap_cache.resolve(section.lightmaps,
                //                    group.lightmap_idx);

                /* ... and moving on */

                auto indices  = mesh.pc_indices(section).data(bsp_magic);
                auto vertices = mesh.pc_vertices().data(bsp_magic);

                using vertex_type =
                    std::remove_const<decltype(vertices)::value_type>::type;
                using element_type =
                    std::remove_const<decltype(indices)::value_type>::type;

                mesh_data.mesh          = &mesh;
                mesh_data.draw.m_eltype = semantic::TypeEnum::UShort;
                mesh_data.draw.m_eoff = element_ptr / sizeof(blam::vert::idx_t);
                mesh_data.draw.m_elems = C_FCAST<u32>(indices.elements);
                mesh_data.draw.m_voff  = vert_ptr / sizeof(vertex_type);
                mesh_data.draw.m_insts = 1;

                MemCpy(vertices, vert_buffer.at(vert_ptr).as<vertex_type>());
                MemCpy(
                    indices, element_buffer.at(element_ptr).as<element_type>());

                vert_ptr += vertices.size;
                element_ptr += indices.size;
            }
        }

        return out;
    }
    virtual blam::bsp::info const* get_id(blam::bsp::info const& bsp) override
    {
        return &bsp;
    }
};

struct ModelCache
    : DataCache<ModelItem, Tup<u32, u16>, blam::tagref_t const&, u16>
{
    ModelCache(blam::map_container const& map, BitmapCache& bitm_cache) :
        tags(map.tags), index(map), magic(map.magic),
        vertex_magic(tags->vertex_magic(magic)), bitm_cache(bitm_cache),
        vert_ptr(0), element_ptr(0)
    {
    }

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

        auto header =
            (*header_it)->to_reflexive<blam::mod2::header>().data(magic);

        return &header[0];
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

        for(auto const& shader : header->shaders.data(magic))
            bitm_cache.predict(shader.ref, 0);

        auto const& geom = header[0].geometries.data(magic)[geom_idx];
        {
            out.mesh.header = &geom;

            for(auto const& model : geom.mesh_headers.data(magic))
            {
                auto elements = model.index_segment(*tags).data(vertex_magic);
                auto vertices = model.vertex_segment(*tags).data(vertex_magic);

                using element_type =
                    std::remove_const<decltype(elements)::value_type>::type;
                using vertex_type =
                    std::remove_const<decltype(vertices)::value_type>::type;

                if(!elements || !vertices)
                    Throw(undefined_behavior(
                        "failed to locate element or vertex data"));

                auto& draw_data         = out.mesh.sub.emplace_back();
                draw_data.header        = &model;
                draw_data.draw.m_eltype = semantic::TypeEnum::UShort;
                draw_data.draw.m_eoff   = element_ptr / sizeof(element_type);
                draw_data.draw.m_elems  = C_FCAST<u32>(elements.elements);
                draw_data.draw.m_voff   = vert_ptr / sizeof(vertex_type);
                draw_data.draw.m_insts  = 1;

                MemCpy(vertices, vert_buffer.at(vert_ptr).as<vertex_type>());
                MemCpy(
                    elements,
                    element_buffer.at(element_ptr).as<element_type>());

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
            auto& mod = assem.models.emplace_back();
            for(auto const& perm : region.permutations.data(magic))
            {
                auto const& lod = perm.meshindex_lod.at(max_lod);
                mod.at(max_lod) = predict(tag, lod);
            }
        }

        return assem;
    }
};

struct SceneryCache : DataCache<SceneryItem, u32, blam::tagref_t const&>
{
    SceneryCache(blam::map_container const& map, ModelCache& model_cache) :
        model_cache(model_cache), magic(map.magic), index(map)
    {
    }

    ModelCache&          model_cache;
    blam::magic_data_t   magic;
    blam::tag_index_view index;

    virtual SceneryItem predict_impl(blam::tagref_t const& tag) override
    {
        auto it = index.find(tag);

        if(it == index.end())
            return {};

        auto scenery = (*it)->to_reflexive<blam::scn::scenery>().data(magic);

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

struct VehicleCache : DataCache<VehicleItem, u32, blam::tagref_t const&>
{
    VehicleCache(blam::map_container const& map, ModelCache& model_cache) :
        model_cache(model_cache), magic(map.magic), index(map)
    {
    }

    ModelCache&          model_cache;
    blam::magic_data_t   magic;
    blam::tag_index_view index;

    virtual VehicleItem predict_impl(blam::tagref_t const& vehi) override
    {
        auto it = index.find(vehi);

        if(it == index.end())
            return {};

        auto vehicle = (*it)->to_reflexive<blam::scn::vehicle>().data(magic);

        if(!vehicle[0].model.valid())
            return {};

        VehicleItem out;
        out.vehicle = &vehicle[0];
        out.model   = model_cache.predict_regions(
            vehicle[0].model.to_plain(), blam::mod2::lod_high_ext);

        return out;
    }
    virtual u32 get_id(blam::tagref_t const& tag) override
    {
        return tag.tag_id;
    }
};

struct BipedCache : DataCache<BipedItem, u32, blam::tagref_t const&>
{
    BipedCache(blam::map_container const& map, ModelCache& model_cache) :
        magic(map.magic), index(map), model_cache(model_cache)
    {
    }

    blam::magic_data_t   magic;
    blam::tag_index_view index;
    ModelCache&          model_cache;

    virtual BipedItem predict_impl(blam::tagref_t const& bipd) override
    {
        auto it = index.find(bipd);

        if(it == index.end())
            return {};

        auto biped = (*it)->to_reflexive<blam::scn::biped>().data(magic);

        BipedItem out;
        out.header = &biped[0];
        out.model  = model_cache.predict_regions(
            biped[0].model.to_plain(), blam::mod2::lod_high_ext);

        return out;
    }
    virtual u32 get_id(blam::tagref_t const& tag) override
    {
        return tag.tag_id;
    }
};

template<typename T>
struct EquipmentCache : DataCache<T, u32, blam::tagref_t const&>
{
    EquipmentCache(blam::map_container const& map, ModelCache& model_cache) :
        magic(map.magic), index(map), model_cache(model_cache)
    {
    }

    blam::magic_data_t   magic;
    blam::tag_index_view index;
    ModelCache&          model_cache;

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

struct BlamData
{
    BlamData() :
        map_file(MkUrl(GetInitArgs().arguments().at(0), RSCA::SystemFile)),
        bitmap_file(Resource(MkUrl(
            Path(GetInitArgs().arguments().at(0)).dirname() + "bitmaps.map",
            RSCA::SystemFile))),
        map_container(map_file, blam::pc_version),
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
    Resource                   map_file;
    Resource                   bitmap_file;
    blam::map_container        map_container;
    blam::scn::scenario const* scenario;

    BitmapCache bitm_cache;

    BSPCache            bsp_cache;
    Vector<GFX::D_DATA> bsps;
    ShPtr<GFX::BUF_A>   bsp_buf;
    ShPtr<GFX::BUF_E>   bsp_index;
    ShPtr<GFX::V_DESC>  bsp_attr;
    PIP_PARAM*          bsp_pipeline;
    GFX::RenderPass     bsp_pass;
    GFX::OPT_DRAW       bsp_draw;

    ModelCache                    model_cache;
    SceneryCache                  scenery_cache;
    VehicleCache                  vehicle_cache;
    BipedCache                    biped_cache;
    EquipmentCache<EquipmentItem> equip_cache;
    EquipmentCache<WeaponItem>    weap_cache;
    //    EquipmentCache<MPEquipmentItem> mp_equip_cache;

    ShPtr<GFX::BUF_A>  model_buf;
    ShPtr<GFX::BUF_E>  model_index;
    ShPtr<GFX::V_DESC> model_attr;
    PIP_PARAM*         model_pipeline;

    Vector<Matf4>     model_mats;
    GFX::RenderPass   model_pass;
    GFX::OPT_DRAW     model_draw;
    ShPtr<GFX::BUF_S> model_matrix_store;

    camera_t            camera;
    Matf4               camera_matrix;
    StandardCameraOpts  camera_opts;
    ShPtr<std_camera_t> std_camera;

    ControllerOpts                               controller_opts;
    ControllerCamera<camera_t*, ControllerOpts*> controller_camera;
};

void create_resources(EntityContainer& e, BlamData& data)
{
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

    data.bsp_buf->commit(50_MB);
    data.bsp_index->commit(20_MB);

    data.model_buf->commit(30_MB);
    data.model_index->commit(15_MB);

    data.model_matrix_store->commit(10_MB);

    GFX::V_ATTR pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr;
    pos_attr.m_idx  = 0;
    tex_attr.m_idx  = 1;
    nrm_attr.m_idx  = 2;
    bin_attr.m_idx  = 3;
    tan_attr.m_idx  = 4;
    pos_attr.m_off  = offsetof(vertex_type, position);
    tex_attr.m_off  = offsetof(vertex_type, texcoord);
    nrm_attr.m_off  = offsetof(vertex_type, normal);
    bin_attr.m_off  = offsetof(vertex_type, binorm);
    tan_attr.m_off  = offsetof(vertex_type, tangent);
    tex_attr.m_size = 2;

    pos_attr.m_size = nrm_attr.m_size = bin_attr.m_size = tan_attr.m_size = 3;
    pos_attr.m_type = tex_attr.m_type = nrm_attr.m_type = bin_attr.m_type =
        tan_attr.m_type = semantic::TypeEnum::Scalar;
    pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
        bin_attr.m_stride = tan_attr.m_stride = sizeof(vertex_type);
    pos_attr.m_bassoc = tex_attr.m_bassoc = nrm_attr.m_bassoc =
        bin_attr.m_bassoc = tan_attr.m_bassoc = 0;

    data.bsp_attr =
        gfx.alloc_desc<5>({{pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr}});
    data.bsp_attr->setIndexBuffer(data.bsp_index.get());
    data.bsp_attr->bindBuffer(0, *data.bsp_buf);

    using model_vertex_type = blam::vert::mod2_vertex<blam::vert::uncompressed>;

    pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
        bin_attr.m_stride = tan_attr.m_stride = sizeof(model_vertex_type);

    data.model_attr =
        gfx.alloc_desc<5>({{pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr}});
    data.model_attr->setIndexBuffer(data.model_index.get());
    data.model_attr->bindBuffer(0, *data.model_buf);

    data.bitm_cache.allocator = &gfx;
}

void load_scenario_bsp(EntityContainer& e, BlamData& data)
{
    auto& magic = data.map_container.magic;

    data.bsp_cache.vert_buffer    = data.bsp_buf->map();
    data.bsp_cache.element_buffer = data.bsp_index->map();

    Vector<generation_idx_t> bsp_meshes;

    /* Start loading up vertex data */
    auto scenario = data.scenario;
    for(auto const& bsp : scenario->bsp_info.data(magic))
        bsp_meshes.push_back(data.bsp_cache.predict(bsp));

    data.bsp_buf->unmap();
    data.bsp_index->unmap();

    auto& bsp_pass       = data.bsp_pass;
    bsp_pass.pipeline    = data.bsp_pipeline->pipeline_ptr();
    bsp_pass.framebuffer = GFX::DefaultFramebuffer();
    for(auto const& mesh_id : bsp_meshes)
    {
        if(!mesh_id.valid())
            continue;

        auto const& mesh = data.bsp_cache.find(mesh_id)->second;

        for(auto const& group : mesh.groups)
            for(auto const& mesh : group.meshes)
            {
                if(mesh.color_bitm.valid())
                {
                    auto const& texture = data.bitm_cache.find(mesh.color_bitm);

                    cDebug(
                        "Mesh {0} uses texture {1}@{2}",
                        mesh.mesh,
                        0,
                        texture->second.image.index);
                }
                bsp_pass.draws.push_back(
                    {data.bsp_attr,
                     &data.bsp_pipeline->get_state(),
                     GFX::D_CALL().withIndexing().withInstancing(),
                     mesh.draw});
            }
    }
    GFX::OptimizeRenderPass(bsp_pass, data.bsp_draw);
}

template<typename T, typename CacheType>
void load_unit_group(
    CacheType&                   cache,
    blam::reflex_group<T> const& group,
    blam::magic_data_t const&    magic,
    BlamData&                    data)
{
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

                for(auto const& mesh : model.sub)
                {
                    data.model_mats.push_back(
                        typing::vectors::translation(Matf4(), instance.pos) *
                        typing::vectors::matrixify(
                            typing::vectors::normalize_quat(Quatf(
                                1,
                                instance.rot.y(),
                                instance.rot.z(),
                                instance.rot.x()))));
                    data.model_pass.draws.push_back(
                        {data.model_attr,
                         &data.model_pipeline->get_state(),
                         GFX::D_CALL()
                             .withIndexing()
                             .withTriStrip()
                             .withInstancing(),
                         mesh.draw});
                }
            }
    }
}

void load_scenario_scenery(EntityContainer& e, BlamData& data)
{
    data.model_cache.vert_buffer    = data.model_buf->map();
    data.model_cache.element_buffer = data.model_index->map();

    auto scenario = data.scenario;
    auto magic    = data.map_container.magic;

    auto pipeline = data.model_pipeline;

    pipeline->build_state();

    load_unit_group(data.scenery_cache, scenario->scenery, magic, data);
    load_unit_group(data.vehicle_cache, scenario->vehicles, magic, data);
    load_unit_group(data.biped_cache, scenario->bipeds, magic, data);
    load_unit_group(data.equip_cache, scenario->equips, magic, data);
    load_unit_group(data.weap_cache, scenario->weapon_spawns, magic, data);
    //    load_unit_group(data.mp_equip_cache, scenario->mp_equipment, magic,
    //    data);

    GFX::RenderPass& scenery_pass = data.model_pass;
    scenery_pass.pipeline         = data.model_pipeline->pipeline_ptr();

    data.model_buf->unmap();
    data.model_index->unmap();

    GFX::OptimizeRenderPass(scenery_pass, data.model_draw);

    Vector<Matf4> matrices_realloc = std::move(data.model_mats);

    auto matrix_store = data.model_matrix_store->map().as<Matf4>();

    u32 mat_idx = 0;
    for(auto const& draw : scenery_pass.draws)
    {
        matrix_store[draw.d_data.m_ioff] = matrices_realloc.at(mat_idx);
        mat_idx++;
    }

    data.model_matrix_store->unmap();

    GFX::ERROR ec;
    data.model_matrix_store->bindrange(0, 0, 10_MB, ec);

    pipeline->set_constant(
        *std::find_if(
            pipeline->constants_begin(),
            pipeline->constants_end(),
            pipeline->constant_by_name("camera")),
        Bytes::From(data.camera_matrix));

    pipeline->build_state();
    pipeline->get_state();
}

i32 blam_main(i32, cstring_w*)
{
    auto blam_queue = RuntimeQueue::CreateNewQueue("Blam Graphics!");

    comp_app::app_error app_ec;
    auto&               e = comp_app::createContainer();
    comp_app::configureDefaults(e);

    auto loader = e.service<comp_app::AppLoader>();

    comp_app::addDefaults(e, *e.service<comp_app::AppLoader>(), app_ec);
    comp_app::AppContainer<BlamData>::addTo(
        e,
        [](EntityContainer& e, BlamData& data, time_point const&) {
            if(!FileExists(data.map_file))
                Throw(undefined_behavior("map file not found"));

            blam::tag_index_view index(data.map_container);

            auto& magic = data.map_container.magic;

            data.scenario =
                data.map_container.tags->scenario(data.map_container.map)
                    .to_reflexive<blam::scn::scenario>()
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

            auto const& view_u = *std::find_if(
                pipeline.constants_begin(),
                pipeline.constants_end(),
                pipeline.constant_by_name("view"));

            //            pipeline.set_sampler(
            //                *std::find_if(
            //                    pipeline.constants_begin(),
            //                    pipeline.constants_end(),
            //                    pipeline.constant_by_name("color_tex")),
            //                data.textures[blam::bitm::format_t::DXT1].sampler->handle());

            pipeline.set_constant(view_u, Bytes::From(data.camera_matrix));

            pipeline.build_state();
            pipeline.get_state();

            load_scenario_bsp(e, data);
            load_scenario_scenery(e, data);

            data.bitm_cache.allocate_storage();

            GFX::RASTSTATE cull_disable;
            cull_disable.m_doCull = false;
            GFX::SetRasterizerState(cull_disable);

            GFX::DEPTSTATE depth_enable;
            depth_enable.m_test = true;
            GFX::SetDepthState(depth_enable);
        },
        [](EntityContainer& e,
           BlamData&        data,
           time_point const&,
           duration const&) {
            GFX::DefaultFramebuffer()->clear(0, {0.5f, 0.5f, 0.9f, 1.f}, 1.0);

            data.std_camera->tick();
            data.controller_camera(
                e.service<comp_app::ControllerInput>()->state(0));

            using namespace typing::vectors::scene;
            {
                data.camera.aspect =
                    e.service<comp_app::Windowing>()->size().aspect();
                data.camera.zVals = {0.01f, 1000.f};

                data.camera_matrix =
                    GenPerspective(data.camera) * GenTransform(data.camera) *
                    typing::vectors::matrixify(typing::vectors::normalize_quat(
                        Quatf(1, -pi / 4, 0, 0)));
            }

            GFX::ERROR ec;
            GFX::MultiDraw(data.bsp_pipeline->pipeline(), data.bsp_draw, ec);
            C_ERROR_CHECK(ec)

            GFX::MultiDraw(
                data.model_pipeline->pipeline(), data.model_draw, ec);
            C_ERROR_CHECK(ec)
        },
        [](EntityContainer& e, BlamData& data, time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN(blam_main)
