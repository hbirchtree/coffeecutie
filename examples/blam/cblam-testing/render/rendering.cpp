#include "render/rendering.h"

#include "caching.h"
#include "caching_item.h"
#include "components.h"
#include "crunched/loading_screen.h"
#include "data.h"
#include "loading_screen.h"
#include "map_marker.h"
#include "materials.h"
#include "selected_version.h"

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_shaders.h>
#include <blam/volta/blam_tag_classes.h>
#include <blam/volta/blam_tag_index.h>
#include <coffee/core/CProfiling>
#include <coffee/graphics/apis/gleam/rhi_compat.h>
#include <coffee/graphics/apis/gleam/rhi_debug.h>
#include <coffee/graphics/apis/gleam/rhi_draw_command.h>
#include <coffee/graphics/apis/gleam/rhi_program.h>
#include <coffee/graphics/apis/gleam/rhi_query.h>
#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/graphics/apis/gleam/rhi_texture.h>
#include <coffee/graphics/apis/gleam/rhi_urls.h>
#include <coffee/image/ktx_load.h>
#include <magic_enum/magic_enum.hpp>
#include <peripherals/concepts/graphics_api.h>
#include <peripherals/constants.h>
#include <peripherals/stl/iterator_slice.h>
#include <peripherals/stl/tuple_hash.h>
#include <peripherals/typing/enum/graphics/shader_stage.h>

using namespace libc_types::size_literals;
using namespace std::string_view_literals;
using namespace Coffee::resource_literals;

using semantic::RSCA;
using typing::pixels::CompFmt;
using typing::pixels::PixDesc;

namespace detail {

inline std::tuple<pix_fmt, comp_flags> get_bitm_hash(BitmapItem const& bitm)
{
    return std::make_tuple(bitm.image.fmt.pixfmt, bitm.image.fmt.cmpflg);
}

template<typename MapType>
inline void assign_map(MapType& map, BitmapItem const* bitm)
{
    map.atlas_scale  = {0};
    map.atlas_offset = {0};
    map.layer        = -1;

    if(!bitm)
        return;

    map.atlas_scale  = bitm->image.scale;
    map.atlas_offset = bitm->image.offset;
    map.layer        = bitm->image.layer;
}

} // namespace detail

struct alignas(16) PerInstanceData
{
    Matf4 transform;
    i32   bone_base{-1};
    i32   pad[3]{};
};

static_assert(sizeof(PerInstanceData) == 80);

using draw_data_t = gfx::draw_command::data_t;

/* Which material families a pass ended up holding. sotr's combiner register
 * file sets the register budget for every material sharing its binary, so a
 * pass that turns out to hold only one family is submitted with a program
 * built for just that family. A mixed pass falls back to the combined one, so
 * draw order is never disturbed. */
/* Matches `mat4 bones[256]` in fragments/scenery_common.glsl. Budget per
 * bucket, not per frame: each bucket binds its own window. 256 mat4 is 16KB,
 * the smallest GL_MAX_UNIFORM_BLOCK_SIZE ES3 or WebGL2 may report. */
static constexpr u32 kBonesPerBucket = 256;

enum MaterialClass : u8
{
    MatClass_Base    = 0x1,
    MatClass_Chicago = 0x2,
    MatClass_Sotr    = 0x4,
    MatClass_Camo    = 0x8,
};

/* How far to split the uber shader. Splitting cuts register pressure but costs
 * a bucket -- and so a submit -- wherever families meet, so the useful setting
 * is hardware-dependent. `full` is the only mode that leaves no shader
 * spilling, and measured best on NVIDIA Ampere.
 * COFFEE_SHADER_SPLIT=off|sotr|full, default full. */
enum class shader_split_t
{
    off,  /* one program, as before */
    full, /* sotr vs chicago vs the rest */
};

inline shader_split_t shader_split_mode()
{
    return shader_split_t::full;
}

struct PassPrograms
{
    std::shared_ptr<gfx::program_t> combined;
    std::shared_ptr<gfx::program_t> base;
    std::shared_ptr<gfx::program_t> chicago;
    std::shared_ptr<gfx::program_t> sotr;
    std::shared_ptr<gfx::program_t> camo;

    std::shared_ptr<gfx::program_t> const& for_classes(u8 classes) const
    {
        switch(shader_split_mode())
        {
        case shader_split_t::off:
            break;
        case shader_split_t::full:
            if(classes == MatClass_Sotr && sotr)
                return sotr;
            if(classes == MatClass_Chicago && chicago)
                return chicago;
            if(classes == MatClass_Base && base)
                return base;
            if(classes == MatClass_Camo && camo)
                return camo;
            break;
        }
        return combined;
    }
};

/* Additive, multiply and component-max blending are commutative, so draws in
 * those passes may be regrouped freely. Alpha blending is not: Pass_Glass has
 * to keep its back-to-front order. */
inline bool pass_is_order_independent(Passes pass)
{
    switch(pass)
    {
    case Pass_SkyAdditive:
    case Pass_SkyMultiply:
    case Pass_Additive:
    case Pass_Multiply:
    case Pass_Max:
        return true;
    default:
        return false;
    }
}

/* Map a shader tag onto the material family whose program can draw it. */
inline u8 material_class_of(blam::tag_class_t tag)
{
    switch(tag)
    {
    case blam::tag_class_t::sotr:
        return MatClass_Sotr;
    case blam::tag_class_t::schi:
    case blam::tag_class_t::scex:
        return MatClass_Chicago;
    default:
        return MatClass_Base;
    }
}

struct Pass
{
    Pass()
    {
        draws.emplace_back();
    }

    gfx::draw_command                     command;
    PassPrograms                          programs;
    u8                                    material_classes{0};
    std::vector<std::vector<draw_data_t>> draws;
    /* Material families present in each bucket, parallel to `draws`. A bucket
     * holding one family is submitted with that family's program; buckets are
     * broken at family changes rather than reordered, so draw order — which
     * Pass_Glass depends on — is untouched. */
    std::vector<u8> bucket_classes;
    /* Family per sortable draw, parallel to `sort_centers`. */
    std::vector<u8> sort_classes;

    /* Whether each bucket carries skinned draws, parallel to `draws`, and the
     * same per sortable draw. The two never share a bucket: only skinned
     * draws spend window, so mixing them would split batches for nothing. */
    std::vector<u8>  bucket_skinned;
    std::vector<u8>  sort_skinned;
    std::vector<u64> sort_parents;
    std::vector<u32> sort_bones;

    /* Slots spent in the newest bucket and which parents already paid there.
     * Only that bucket is appended to, so one list suffices; bounded by the
     * budget over the smallest skeleton. */
    u32                              open_bones{0};
    std::vector<std::pair<u64, u32>> open_owners;

    /* Start of each bucket's bone window, in matrices, parallel to `draws`.
     * A bone_base only means anything within its own bucket. */
    std::vector<u32> bucket_bone_base;

    /* Collapse families that the current mode draws with the same program. */
    static u8 split_family(u8 cls)
    {
        switch(shader_split_mode())
        {
        case shader_split_t::off:
            return 0;
        case shader_split_t::full:
            return cls;
        }
        return cls;
    }

    /* Start a new bucket when the current one is full or holds another
     * family, and record the family of whichever bucket we land in. */
    /* What this draw adds to the open bucket: nothing if its parent already
     * paid there. */
    u32 bone_cost(bool skinned, u64 parent, u32 bones) const
    {
        if(!skinned || bones == 0)
            return 0;
        for(auto const& owner : open_owners)
            if(owner.first == parent)
                return 0;
        return bones;
    }

    void open_bucket(
        u8 cls, size_t incoming, bool skinned, u64 parent = 0, u32 bones = 0)
    {
        bucket_classes.resize(draws.size(), 0);
        bucket_skinned.resize(draws.size(), 0);
        size_t num_draws = 0;
        for(auto const& d : draws.back())
            num_draws += d.instances.count;
        bool full = num_draws >= 128 || (num_draws + incoming) > 128;
        /* Only break where the chosen mode actually needs a different program:
         * in `sotr` mode chicago and base share one, so they share a bucket. */
        u8   effective    = split_family(cls);
        bool other_family = !draws.back().empty() &&
                            split_family(bucket_classes.back()) != effective;
        bool other_skin = !draws.back().empty() &&
                          static_cast<bool>(bucket_skinned.back()) != skinned;
        /* Only skinned buckets run out of window -- that is why the two are
         * kept apart. */
        u32  cost       = bone_cost(skinned, parent, bones);
        bool bones_full = skinned && !draws.back().empty() &&
                          open_bones + cost > kBonesPerBucket;
        if(full || other_family || other_skin || bones_full)
        {
            draws.emplace_back();
            bucket_classes.push_back(0);
            bucket_skinned.push_back(skinned ? 1 : 0);
            open_bones = 0;
            open_owners.clear();
            cost = skinned ? bones : 0;
        }
        bucket_classes.back() |= cls;
        bucket_skinned.back() = skinned ? 1 : 0;
        if(cost)
        {
            open_owners.push_back({parent, open_bones});
            open_bones += cost;
        }
    }

    gfx::buffer_slice_t material_buffer;
    gfx::buffer_slice_t transparent_buffer;
    gfx::buffer_slice_t matrix_buffer;

    std::vector<std::vector<std::shared_ptr<gfx::texture_t>>>
        reflection_textures;

    std::vector<materials::shader_data>      material_staging;
    std::vector<materials::transparent_data> transparent_staging;
    std::vector<PerInstanceData>             matrix_staging;

    Span<materials::shader_data>      material_mapping;
    Span<materials::transparent_data> transparent_mapping;
    Span<PerInstanceData>             matrix_mapping;

    void stage_materials(size_t material_size, size_t transparent_size)
    {
        material_staging.assign(
            material_size / sizeof(materials::shader_data), {});
        transparent_staging.assign(
            transparent_size / sizeof(materials::transparent_data), {});
        material_mapping = Span<materials::shader_data>(material_staging);
        transparent_mapping =
            Span<materials::transparent_data>(transparent_staging);
    }

    void stage_matrices(size_t matrix_size)
    {
        matrix_staging.assign(matrix_size / sizeof(PerInstanceData), {});
        matrix_mapping = Span<PerInstanceData>(matrix_staging);
    }

    std::string name;

    // Flat world-space centers parallel to draws (across all buckets).
    // Only populated for transparent passes; used for depth sorting.
    std::vector<Vecf3> sort_centers;

    model_tracker_t insert_draw(
        draw_data_t const& draw,
        u8                 cls,
        bool               skinned = false,
        u64                parent  = 0,
        u32                bones   = 0)
    {
        open_bucket(cls, draw.instances.count, skinned, parent, bones);

        auto& bucket_ = draws.back();
        auto  it      = std::find_if(
            bucket_.begin(), bucket_.end(), [&draw](draw_data_t const& d) {
                return d.elements.offset == draw.elements.offset;
            });
        if(it != bucket_.end())
        {
            return model_tracker_t{
                .bucket   = static_cast<u16>(draws.size() - 1),
                .draw     = static_cast<u16>(it - bucket_.begin()),
                .instance = static_cast<u16>(it->instances.count++),
                .enabled  = true,
            };
        } else
        {
            bucket_.push_back(draw);
            return model_tracker_t{
                .bucket   = static_cast<u16>(draws.size() - 1),
                .draw     = static_cast<u16>(bucket_.size() - 1),
                .instance = 0,
                .enabled  = true,
            };
        }
    }

    // Transparent passes: one draw per item (no instancing), records
    // center.
    model_tracker_t insert_sortable(
        draw_data_t  draw,
        Vecf3 const& center,
        u8           cls,
        bool         skinned = false,
        u64          parent  = 0,
        u32          bones   = 0)
    {
        draw.instances.count = 1;
        open_bucket(cls, 1, skinned, parent, bones);
        auto& bucket_ = draws.back();
        bucket_.push_back(draw);
        sort_centers.push_back(center);
        sort_classes.push_back(cls);
        sort_skinned.push_back(skinned ? 1 : 0);
        sort_parents.push_back(parent);
        sort_bones.push_back(bones);
        return model_tracker_t{
            .bucket   = static_cast<u16>(draws.size() - 1),
            .draw     = static_cast<u16>(bucket_.size() - 1),
            .instance = 0,
            .enabled  = true,
        };
    }

    // Sort transparent draws back-to-front by distance from cam.
    // Call after update_materials (material slots unchanged by reorder).
    void sort_by_depth(Vecf3 const& cam, bool group_by_family)
    {
        if(sort_centers.empty())
            return;

        std::vector<std::tuple<
            Vecf3,
            draw_data_t,
            std::shared_ptr<gfx::texture_t>,
            u8,
            u8,
            u64,
            u32>>
            flat;
        flat.reserve(sort_centers.size());
        size_t ci = 0;
        for(size_t bi = 0; bi < draws.size(); bi++)
            for(size_t di = 0; di < draws[bi].size(); di++)
            {
                auto const& cubes = reflections_for(bi);
                u8  cls  = ci < sort_classes.size() ? sort_classes[ci] : 0;
                u8  skin = ci < sort_skinned.size() ? sort_skinned[ci] : 0;
                u64 par  = ci < sort_parents.size() ? sort_parents[ci] : 0;
                u32 bn   = ci < sort_bones.size() ? sort_bones[ci] : 0;
                flat.push_back(
                    {sort_centers[ci++],
                     draws[bi][di],
                     di < cubes.size() ? cubes[di] : nullptr,
                     cls,
                     skin,
                     par,
                     bn});
            }

        /* stable: parts at equal distance (e.g. all sky model parts,
         * which share one transform) keep their creation order. */
        std::stable_sort(
            flat.begin(), flat.end(), [&](auto const& a, auto const& b) {
                return glm::distance2(std::get<0>(a), cam) >
                       glm::distance2(std::get<0>(b), cam);
            });

        /* Where blending is commutative, gather the families together so the
         * pass costs a handful of buckets instead of one per family change --
         * sotr and chicago interleave in depth, which otherwise degenerates to
         * roughly a bucket per draw. Stable, so depth order still holds within
         * each family. */
        if(group_by_family)
            std::stable_sort(
                flat.begin(), flat.end(), [](auto const& a, auto const& b) {
                    /* Skinning splits buckets like a family does, and the
                     * two alternate freely without this. */
                    return std::tie(std::get<4>(a), std::get<3>(a)) <
                           std::tie(std::get<4>(b), std::get<3>(b));
                });

        draws.clear();
        draws.emplace_back();
        sort_centers.clear();
        sort_classes.clear();
        sort_skinned.clear();
        sort_parents.clear();
        sort_bones.clear();
        bucket_classes.assign(1, 0);
        bucket_skinned.assign(1, 0);
        bucket_bone_base.clear();
        open_bones = 0;
        open_owners.clear();
        reflection_textures.clear();
        for(auto& [c, d, cube, cls, skin, par, bn] : flat)
        {
            /* Break on family, skinning or a spent window. Sorted order is
             * kept exactly, so this cannot alter blending. */
            u32  cost       = bone_cost(skin != 0, par, bn);
            bool bones_full = skin && !draws.back().empty() &&
                              open_bones + cost > kBonesPerBucket;
            if(draws.back().size() >= 128 || bones_full ||
               (!draws.back().empty() &&
                (split_family(bucket_classes.back()) != split_family(cls) ||
                 bucket_skinned.back() != skin)))
            {
                draws.emplace_back();
                bucket_classes.push_back(0);
                bucket_skinned.push_back(skin);
                open_bones = 0;
                open_owners.clear();
                cost = skin ? bn : 0;
            }
            bucket_classes.back() |= cls;
            bucket_skinned.back() = skin;
            if(cost)
            {
                open_owners.push_back({par, open_bones});
                open_bones += cost;
            }
            draws.back().push_back(d);
            sort_centers.push_back(c);
            sort_classes.push_back(cls);
            sort_skinned.push_back(skin);
            sort_parents.push_back(par);
            sort_bones.push_back(bn);
            if(cube)
                set_reflection(
                    static_cast<u16>(draws.size() - 1),
                    static_cast<u16>(draws.back().size() - 1),
                    cube);
        }
    }

    inline void clear()
    {
        draws.clear();
        draws.emplace_back();
        sort_centers.clear();
        sort_classes.clear();
        bucket_classes.assign(1, 0);
        bucket_skinned.assign(1, 0);
        sort_skinned.clear();
        sort_parents.clear();
        sort_bones.clear();
        open_bones = 0;
        open_owners.clear();
        bucket_bone_base.clear();
        reflection_textures.clear();
        material_classes = 0;
    }

    /* Record the cube for one draw, growing to match `draws`. */
    inline void set_reflection(
        u16 bucket, u16 draw, std::shared_ptr<gfx::texture_t> cube)
    {
        if(reflection_textures.size() <= bucket)
            reflection_textures.resize(bucket + 1);
        auto& b = reflection_textures[bucket];
        if(b.size() <= draw)
            b.resize(draw + 1);
        b[draw] = std::move(cube);
    }

    inline std::vector<std::shared_ptr<gfx::texture_t>> const& reflections_for(
        size_t bucket) const
    {
        static const std::vector<std::shared_ptr<gfx::texture_t>> none;
        return bucket < reflection_textures.size() ? reflection_textures[bucket]
                                                   : none;
    }

    inline materials::shader_data& material_of(size_t idx)
    {
        if(idx >= material_mapping.size())
            Throw(std::out_of_range("material index out of range"));
        return material_mapping[idx];
    }

    inline materials::transparent_data& transparent_of(size_t idx)
    {
        if(idx >= transparent_mapping.size())
            Throw(std::out_of_range("transparent index out of range"));
        return transparent_mapping[idx];
    }

    inline size_t required_storage() const
    {
        u32 total = 0;
        for(auto const& bucket : draws)
            for(auto const& draw : bucket)
                total += draw.instances.count;
        return total * sizeof(materials::shader_data);
    }

    inline size_t required_matrix_storage() const
    {
        return (required_storage() / sizeof(materials::shader_data)) *
               sizeof(PerInstanceData);
    }

    inline size_t required_transparent_storage() const
    {
        return (required_storage() / sizeof(materials::shader_data)) *
               sizeof(materials::transparent_data);
    }
};

/*!
 * Builds the per-frame draw lists and fills the material/matrix buffers
 */
template<typename V>
using DrawListBuilderManifest = compo::SubsystemManifest<
    type_list_t<
        DrawState,
        MeshTrackingData,
        AnimationPlayback,
        const BspReference,
        const SubModel,
        const Model,
        const ObjectSpawn,
        const Visibility,
        const PlayerCamera,
        const PlayerInfo>,
    type_list_t<
        BitmapCache<V>,
        ModelCache<V>,
        ShaderCache<V>,
        RenderingParameters,
        LoadingStatus>,
    empty_list_t>;

template<typename Version = blam::xbox_version_t>
struct DrawListBuilder
    : compo::RestrictedSubsystem<
          DrawListBuilder<Version>,
          DrawListBuilderManifest<Version>>
{
    using type  = DrawListBuilder<Version>;
    using Proxy = compo::proxy_of<DrawListBuilderManifest<Version>>;

    DrawListBuilder(
        gfx::api*             api,
        BlamResources&        resources,
        RenderingParameters&  render,
        ShaderCache<Version>& shader_cache,
        BitmapCache<Version>& bitm_cache)
        : m_api(api)
        , m_resources(resources)
        , m_render_params(render)
        , shader_cache(shader_cache)
        , bitm_cache(bitm_cache)
    {
        /* Ahead of MeshRenderer (3072): produces what it submits */
        this->priority = 3071;

        for(auto& buffered : m_bsp_sets)
            for(u32 vp = 0; vp < kMaxViewports; vp++)
            {
                u32 i = 0;
                for(auto& pass : buffered[vp])
                {
                    pass.command.program = resources.bsp_pipeline;
                    pass.programs        = {
                               .combined = resources.bsp_pipeline,
                               .base     = resources.bsp_pipeline_base,
                               .chicago  = resources.bsp_pipeline_chicago,
                               .sotr     = resources.bsp_pipeline_sotr,
                    };
                    pass.name = fmt::format(
                        "BSP::{}::{}",
                        vp,
                        magic_enum::enum_name(static_cast<Passes>(i++)));
                }
            }
        for(auto& buffered : m_model_sets)
            for(u32 vp = 0; vp < kMaxViewports; vp++)
            {
                u32 i = 0;
                for(auto& pass : buffered[vp])
                {
                    pass.command.program = resources.model_pipeline;
                    pass.programs        = {
                               .combined = resources.model_pipeline,
                               .base     = resources.model_pipeline_base,
                               .chicago  = resources.model_pipeline_chicago,
                               .sotr     = resources.model_pipeline_sotr,
                               .camo     = resources.camo,
                    };
                    pass.name = fmt::format(
                        "MOD::{}::{}",
                        vp,
                        magic_enum::enum_name(static_cast<Passes>(i++)));
                }
            }
    }

    void start_restricted(Proxy& p, compo::time_point const& time)
    {
        ProfContext _;

        m_submit = m_build;

        m_resources.model_matrix_store->next();
        m_resources.transparent_store->next();
        m_resources.material_store->next();
        m_resources.bone_matrix_buf->next();

        // Performance is terrible on Emscripten when updating every frame
        // We need a more efficient way to update the buffer in that case
        bool invalidated = true; //! compile_info::platform::is_emscripten;
        if(time - last_update <= std::chrono::seconds(10) && !invalidated)
            return;

        /* Clocks move on once per frame; posing happens per viewport, and
         * chaining to next_animation would double-step if it lived there.
         * A hitch must not fling every animation forward, hence the clamp. */
        {
            f32 delta = last_update.time_since_epoch().count() > 0
                            ? stl_types::chrono::to_f32(time - last_update)
                            : 0.f;
            delta     = std::clamp(delta, 0.f, 0.25f);

            ModelCache<Version>* model_cache;
            p.subsystem(model_cache);
            u32 playbacks = 0;
            for(auto ent : p.template select<AnimationPlayback>())
            {
                auto& anim     = ent.template get<AnimationPlayback>();
                anim.bone_base = kBoneUnposed;
                playbacks++;
                if(delta > 0.f)
                    model_cache->advance_playback(anim, delta);
            }
            /* Last frame's figures. `denied` above zero means instances fell
             * back to their bind pose for want of window. */
            if(getenv("BLAM_ANIM_STATS"))
                cDebug(
                    "anim_stats: playbacks={} skinned={} blocks={} denied={} "
                    "buckets={} bones={} cap={}/bucket",
                    playbacks,
                    m_bone_work.size(),
                    m_posed_instances,
                    m_denied_instances,
                    m_bone_buckets,
                    m_bone_upload.size(),
                    kMaxBoneMatrices);
        }

        m_epoch++;

        /* Same filter and ordering as MeshRenderer::m_players, so viewport i
         * here is viewport i there. */
        m_views.clear();
        for(auto ent : p.template select<PlayerCamera, PlayerInfo>())
        {
            auto const& [cam, info] = ent.components();
            if(info.is_remote() || !cam.is_active())
                continue;
            if(m_views.size() >= kMaxViewports)
                break;
            m_views.push_back({
                .seat     = info.seat_idx,
                .position = cam.camera.position,
            });
        }
        std::sort(
            m_views.begin(), m_views.end(), [](auto const& a, auto const& b) {
                return a.seat < b.seat;
            });
        if(m_views.empty())
            m_views.push_back({});

        last_update = time;

        /* One viewport at a time: the per-entity instance offsets in DrawState
         * and MeshTrackingData are scratch, reused by each pass over the
         * world, so a viewport must be built and have its materials populated
         * before the next overwrites them. */
        m_materials_ptr   = 0;
        m_transparent_ptr = 0;
        m_matrix_ptr      = 0;

        for(u32 vp = 0; vp < m_views.size(); vp++)
        {
            m_vp   = vp;
            m_seat = m_views[vp].seat;

            generate_draws(p);
            update_materials(p, time);

            for(i32 pi = Pass_LastOpaque + 1; pi < Pass_Count; ++pi)
            {
                bool grouped =
                    pass_is_order_independent(static_cast<Passes>(pi));
                model_build()[static_cast<Passes>(pi)].sort_by_depth(
                    m_views[vp].position, grouped);
                bsp_build()[static_cast<Passes>(pi)].sort_by_depth(
                    m_views[vp].position, grouped);
            }

            /* Buckets are final only now; the sort rebuilt the transparent
             * ones. */
            allocate_bones(p);
        }
        m_vp   = 0;
        m_seat = 0;
    }

    compo::time_point last_update{};

    size_t align_for_gpu_padding(size_t size) const
    {
        u32 padding = m_api->limits().buffers.ubo_alignment;
        if((size % padding) == 0)
            return size;
        u32 mask           = padding - 1;
        u32 unaligned_size = size & mask;
        u32 added_padding  = padding - unaligned_size;
        return size + added_padding;
    }

    gfx::api*             m_api;
    BlamResources&        m_resources;
    RenderingParameters&  m_render_params;
    ShaderCache<Version>& shader_cache;
    BitmapCache<Version>& bitm_cache;

    /* Two sets: one being built for the next frame, one the renderer is
     * submitting from. The GPU storage behind the spans already rotates
     * (revolving_buffer_t, per_frame_bufs=3), so each set naturally lands on
     * a different buffer. */
    /* Splitscreen submits a different set per viewport: a player must not
     * pay for what only the other one can see. */
    static constexpr u32 kMaxViewports = 4;

    /* Instances past the budget fall back to their bind pose rather than read
     * a bone slot that belongs to someone else. */
    static constexpr u32 kMaxBoneMatrices = kBonesPerBucket;

    /* Negative means "no skinning" to the shader. The two values separate
     * "not looked at yet" from "asked and got nothing", so a parent's
     * submodels settle it once between them. */
    static constexpr i32 kBoneUnposed = -1;
    static constexpr i32 kBoneDenied  = -2;
    using pass_set_t                  = std::array<Pass, Pass_Count>;

    std::array<std::array<pass_set_t, kMaxViewports>, 2> m_bsp_sets;
    std::array<std::array<pass_set_t, kMaxViewports>, 2> m_model_sets;
    u32                                                  m_build{0};
    u32                                                  m_submit{1};

    struct view_t
    {
        u32   seat{0};
        Vecf3 position{};
    };

    /* Active local viewports, sorted by seat -- MeshRenderer sorts its own
     * copy the same way, so index i means the same viewport in both. */
    std::vector<view_t> m_views;
    u32                 m_vp{0};   /* viewport currently being built */
    u32                 m_seat{0}; /* its seat, for visibility queries */

    /* Store cursors for the frame, shared by every viewport. */
    size_t m_materials_ptr{0};
    size_t m_transparent_ptr{0};
    size_t m_matrix_ptr{0};

    u32 view_count() const
    {
        return static_cast<u32>(m_views.size());
    }

    /* The set the builder writes; the renderer must never see it. */
    pass_set_t& bsp_build()
    {
        return m_bsp_sets[m_build][m_vp];
    }

    pass_set_t& model_build()
    {
        return m_model_sets[m_build][m_vp];
    }

    /* The completed set from the previous frame, for the renderer. */
    pass_set_t const& bsp_submit(u32 vp = 0) const
    {
        return m_bsp_sets[m_submit][std::min(vp, kMaxViewports - 1)];
    }

    pass_set_t const& model_submit(u32 vp = 0) const
    {
        return m_model_sets[m_submit][std::min(vp, kMaxViewports - 1)];
    }

    u32 m_epoch{0};

    /*! Whether a tracker's indices address the set currently being built */
    bool followable(model_tracker_t const& id) const
    {
        return id.enabled && id.epoch == m_epoch;
    }

    /*! Bone matrices for the frame, uploaded by the renderer */
    std::vector<Matf4> m_bone_upload;
    u32                m_posed_instances{0};

    /* One entry per animated instance in a draw list, grouped by bucket
     * before the windows are laid out. Members, so a frame allocates
     * nothing once they have grown. */
    struct bone_work_t
    {
        u16              pass;
        u16              bucket;
        u64              parent;
        generation_idx_t model;
        u32              instance;
        u32              bones;
    };

    std::vector<bone_work_t>         m_bone_work;
    std::vector<std::pair<u64, u32>> m_bone_owners;
    u32                              m_denied_instances{0};
    u32                              m_bone_buckets{0};

    std::vector<Matf4> const& bone_upload() const
    {
        return m_bone_upload;
    }

    void generate_static_draws(
        Proxy& p, size_t& materials_ptr, size_t& transparent_ptr)
    {
        ProfContext           _;
        std::map<Passes, i32> instance_offsets;
        for(Pass& pass : bsp_build())
            pass.clear();

        for(auto ent : p.template select<BspReference, DrawState, Visibility>())
        {
            auto&& [bsp, bsp_draw, vis] = ent.components();

            if(!vis.visible_for(m_seat))
                continue;

            Pass& wf              = bsp_build()[bsp_draw.current_pass];
            i32&  instance_offset = instance_offsets[bsp_draw.current_pass];
            wf.command.vertices   = m_resources.bsp_attr;
            wf.command.call       = {
                      .indexed   = true,
                      .instanced = true,
                      .mode      = gfx::drawing::primitive::triangle,
            };
            bsp_draw.draw.data.front().instances.offset = instance_offset;
            instance_offset += bsp_draw.draw.data.front().instances.count;
            auto sh_it   = shader_cache.find(bsp.shader);
            u8   mat_cls = sh_it != shader_cache.end()
                               ? material_class_of(sh_it->second.tag_class)
                               : MatClass_Base;
            wf.material_classes |= mat_cls;
            if(bsp_draw.current_pass > Pass_LastOpaque)
                wf.insert_sortable(
                    bsp_draw.draw.data.front(), bsp.sort_center, mat_cls);
            else
            {
                /* Opaque BSP keeps its single instanced bucket; only record
                 * which family it holds. */
                wf.bucket_classes.resize(wf.draws.size(), 0);
                wf.bucket_classes[0] |= mat_cls;
                wf.draws[0].push_back(bsp_draw.draw.data.front());
            }
        }

        if(!m_api->feature_info().program.buffer_binding)
            return;

        /* Allocate the material instances from the material pool */
        for(Pass& pass : bsp_build())
        {
            auto transparent_size =
                align_for_gpu_padding(pass.required_transparent_storage());
            auto material_size = align_for_gpu_padding(pass.required_storage());
            pass.material_buffer =
                m_resources.material_store->slice(materials_ptr, material_size);
            pass.transparent_buffer = m_resources.transparent_store->slice(
                transparent_ptr, transparent_size);
            pass.stage_materials(material_size, transparent_size);
            materials_ptr += material_size;
            transparent_ptr += transparent_size;
        }

        for(auto ent : p.template select<BspReference, DrawState, Visibility>())
        {
            auto&& [bsp, bsp_draw, vis] = ent.components();

            if(!vis.visible_for(m_seat))
                continue;
            populate_bsp_material(
                bsp,
                bsp_draw.current_pass,
                bsp_draw.draw.data.front().instances.offset);
        }
    }

    void generate_draws(Proxy& p)
    {
        ProfContext _;

        RenderingParameters* rendering_params;
        p.subsystem(rendering_params);

        ModelCache<Version>* model_cache;
        p.subsystem(model_cache);

        /* Frame-scoped, not call-scoped: every viewport carves its own
         * slices out of the same stores, so restarting at zero would have
         * each one overwrite the last. */
        size_t& materials_ptr   = m_materials_ptr;
        size_t& transparent_ptr = m_transparent_ptr;
        generate_static_draws(p, materials_ptr, transparent_ptr);

        for(Pass& pass : model_build())
            pass.clear();

        /* Skinned and unskinned go in on separate sweeps: they interleave in
         * entity order, and since the two never share a bucket, inserting
         * them as they come would break one at every alternation.
         *
         * Model lives on the parent entity — resolve it through
         * SubModel::parent, it is never on the submodel entity itself */
        for(int sweep = 0; sweep < 2; sweep++)
            for(auto ent :
                p.template select<SubModel, DrawState, MeshTrackingData>())
            {
                auto&& [model, model_draw, track] = ent.components();
                auto         parent = p.template ref<Proxy>(model.parent);
                Model const& mod    = parent.template get<Model>();

                if(!parent.template get<Visibility>().visible_for(m_seat) ||
                   (!rendering_params->render_scenery &&
                    (ent.tags() & ObjectSkybox) == 0))
                {
                    if(sweep == 0)
                        track.model_id = {};
                    continue;
                }

                {
                    /* Transparent goes in once, in entity order: its depth
                     * sort is stable, so insertion order settles draws at
                     * equal distance and resweeping would reorder them. */
                    auto const* a =
                        p.template get<AnimationPlayback>(model.parent);
                    bool const is_skinned =
                        a && a->animating() &&
                        model_cache->bone_count(mod.model) > 0;
                    if(model_draw.current_pass > Pass_LastOpaque)
                    {
                        if(sweep != 0)
                            continue;
                    } else if(is_skinned != (sweep == 1))
                        continue;
                }

                Pass& wf            = model_build()[model_draw.current_pass];
                wf.command.vertices = m_resources.model_attr;
                wf.command.call     = {
                        .indexed   = true,
                        .instanced = true,
                        .mode      = gfx::drawing::primitive::triangle_strip,
                };
                auto sh_it   = shader_cache.find(model.shader);
                u8   mat_cls = model_draw.current_pass == Pass_Postprocess
                                   ? MatClass_Camo
                               : sh_it != shader_cache.end()
                                   ? material_class_of(sh_it->second.tag_class)
                                   : MatClass_Base;
                wf.material_classes |= mat_cls;

                /* Own buckets for skinned draws, so the window only ever
                 * splits batches that carry bones. */
                auto const* anim =
                    p.template get<AnimationPlayback>(model.parent);
                u32 const  bones   = anim && anim->animating()
                                         ? model_cache->bone_count(mod.model)
                                         : 0;
                bool const skinned = bones > 0;

                if(model_draw.current_pass > Pass_LastOpaque)
                {
                    Vecf3 center   = Vecf3(mod.transform[3]);
                    track.model_id = wf.insert_sortable(
                        model_draw.draw.data.front(),
                        center,
                        mat_cls,
                        skinned,
                        model.parent,
                        bones);
                } else
                    track.model_id = wf.insert_draw(
                        model_draw.draw.data.front(),
                        mat_cls,
                        skinned,
                        model.parent,
                        bones);
                track.model_id.epoch = m_epoch;
            }

        for(Pass& pass : model_build())
        {
            i32 instance_offset = 0;
            for(auto& bucket : pass.draws)
                for(draw_data_t& draw : bucket)
                {
                    draw.instances.offset = instance_offset;
                    instance_offset += draw.instances.count;
                }
        }

        if(!m_api->feature_info().program.buffer_binding)
            return;

        size_t& matrix_ptr = m_matrix_ptr;
        for(Pass& pass : model_build())
        {
            auto material_size = align_for_gpu_padding(pass.required_storage());
            auto matrix_size =
                align_for_gpu_padding(pass.required_matrix_storage());
            auto transparent_size =
                align_for_gpu_padding(pass.required_transparent_storage());
            pass.material_buffer =
                m_resources.material_store->slice(materials_ptr, material_size);
            pass.matrix_buffer =
                m_resources.model_matrix_store->slice(matrix_ptr, matrix_size);
            pass.transparent_buffer = m_resources.transparent_store->slice(
                transparent_ptr, transparent_size);
            pass.stage_materials(material_size, transparent_size);
            pass.stage_matrices(matrix_size);
            matrix_ptr += matrix_size;
            materials_ptr += material_size;
            transparent_ptr += transparent_size;
        }

        for(auto ent :
            p.template select<SubModel, DrawState, MeshTrackingData>())
        {
            if(!rendering_params->render_scenery &&
               (ent.tags() & ObjectSkybox) == 0)
                continue;
            auto [smodel, sm_draw, track] = ent.components();

            if(!followable(track.model_id))
                continue;

            auto         parent = p.template ref<Proxy>(smodel.parent);
            Model const& model  = parent.template get<Model>();

            Pass& pass = model_build()[sm_draw.current_pass];
            if(track.model_id.bucket >= pass.draws.size())
                continue;
            draw_data_t const& draw =
                pass.draws[track.model_id.bucket].at(track.model_id.draw);
            auto instance_id = draw.instances.offset + track.model_id.instance;

            record_reflection_cube(
                pass, smodel, track.model_id.bucket, track.model_id.draw);

            ModelItem<Version>& cache_item =
                model_cache->find(model.model)->second;

            /* allocate_bones fills these once buckets are final. An instance
             * that never animates keeps this and skips skinning. */
            auto& pid     = pass.matrix_mapping[instance_id];
            pid.transform = model.transform;
            pid.bone_base = kBoneUnposed;
            populate_mod2_material(
                smodel,
                sm_draw.current_pass,
                cache_item,
                model_context(model),
                instance_id,
                parent.template get<Visibility>().interior);
        }
    }

    /* Reserves and fills one bone window per bucket, once the layout is
     * final -- sort_by_depth rebuilds it for the transparent passes.
     *
     * bone_base is bucket-relative, so a bucket only has to fit
     * kMaxBoneMatrices rather than the frame. Parents sharing a bucket share
     * a block; one whose submodels straddle a family split pays per bucket. */
    void allocate_bones(Proxy& p)
    {
        ModelCache<Version>* model_cache;
        p.subsystem(model_cache);

        RenderingParameters* rendering_params;
        p.subsystem(rendering_params);

        if(m_vp == 0)
        {
            m_bone_upload.clear();
            m_posed_instances  = 0;
            m_denied_instances = 0;
            m_bone_buckets     = 0;
        }

        /* Collected first, grouped by bucket after: entity order has nothing
         * to do with bucket order, and a window must be contiguous. */
        m_bone_work.clear();
        for(auto ent :
            p.template select<SubModel, DrawState, MeshTrackingData>())
        {
            if(!rendering_params->render_scenery &&
               (ent.tags() & ObjectSkybox) == 0)
                continue;
            auto [smodel, sm_draw, track] = ent.components();

            if(!followable(track.model_id))
                continue;

            auto* anim = p.template get<AnimationPlayback>(smodel.parent);
            if(!anim || !anim->animating())
                continue;

            /* Trackers still address the layout only for opaque passes;
             * sort_by_depth rebuilt the transparent ones after these indices
             * were handed out. Those come from the sorted arrays below. */
            if(sm_draw.current_pass > Pass_LastOpaque)
                continue;

            Pass& pass = model_build()[sm_draw.current_pass];
            if(track.model_id.bucket >= pass.draws.size())
                continue;
            auto const& bucket = pass.draws[track.model_id.bucket];
            if(track.model_id.draw >= bucket.size())
                continue;

            auto parent = p.template ref<Proxy>(smodel.parent);
            u32  bones =
                model_cache->bone_count(parent.template get<Model>().model);
            if(bones == 0)
                continue;

            m_bone_work.push_back({
                .pass     = static_cast<u16>(sm_draw.current_pass),
                .bucket   = track.model_id.bucket,
                .parent   = smodel.parent,
                .model    = parent.template get<Model>().model,
                .instance = static_cast<u32>(
                    bucket.at(track.model_id.draw).instances.offset +
                    track.model_id.instance),
                .bones = bones,
            });
        }

        /* Transparent passes, read off the layout the sort left behind:
         * sort_parents and sort_bones were rebuilt alongside `draws`, and a
         * sortable draw always holds exactly one instance. */
        for(i32 pi = Pass_LastOpaque + 1; pi < Pass_Count; ++pi)
        {
            Pass&  pass = model_build()[static_cast<Passes>(pi)];
            size_t ci   = 0;
            for(size_t bi = 0; bi < pass.draws.size(); bi++)
                for(size_t di = 0; di < pass.draws[bi].size(); di++, ci++)
                {
                    if(ci >= pass.sort_bones.size() || pass.sort_bones[ci] == 0)
                        continue;
                    u64 const parent = pass.sort_parents[ci];
                    auto*     anim = p.template get<AnimationPlayback>(parent);
                    auto*     mod  = p.template get<Model>(parent);
                    if(!anim || !anim->animating() || !mod)
                        continue;
                    m_bone_work.push_back({
                        .pass     = static_cast<u16>(pi),
                        .bucket   = static_cast<u16>(bi),
                        .parent   = parent,
                        .model    = mod->model,
                        .instance = static_cast<u32>(
                            pass.draws[bi][di].instances.offset),
                        .bones = pass.sort_bones[ci],
                    });
                }
        }

        std::stable_sort(
            m_bone_work.begin(),
            m_bone_work.end(),
            [](bone_work_t const& a, bone_work_t const& b) {
                return std::tie(a.pass, a.bucket) < std::tie(b.pass, b.bucket);
            });

        size_t i = 0;
        while(i < m_bone_work.size())
        {
            u16 const pass_idx   = m_bone_work[i].pass;
            u16 const bucket_idx = m_bone_work[i].bucket;
            Pass&     pass       = model_build()[pass_idx];

            /* Bound at this offset, so it must satisfy
             * UNIFORM_BUFFER_OFFSET_ALIGNMENT: a misaligned bind is rejected
             * and leaves the previous bucket's bones in place. */
            {
                size_t bytes   = m_bone_upload.size() * sizeof(Matf4);
                size_t aligned = align_for_gpu_padding(bytes);
                m_bone_upload.resize(
                    (aligned + sizeof(Matf4) - 1) / sizeof(Matf4));
            }

            size_t const window = m_bone_upload.size();

            /* Windows bind at full width, so the buffer must hold a whole
             * one past this offset. */
            if((window + kMaxBoneMatrices) * sizeof(Matf4) >
               m_resources.bone_matrix_buf->size())
            {
                for(;
                    i < m_bone_work.size() && m_bone_work[i].pass == pass_idx &&
                    m_bone_work[i].bucket == bucket_idx;
                    i++)
                {
                    m_denied_instances++;
                    pass.matrix_mapping[m_bone_work[i].instance].bone_base =
                        kBoneDenied;
                }
                continue;
            }

            m_bone_buckets++;
            pass.bucket_bone_base.resize(pass.draws.size(), 0);
            pass.bucket_bone_base[bucket_idx] = static_cast<u32>(window);

            /* Who already holds a block here. Bounded by the cap over the
             * smallest skeleton, so a linear scan beats a map. */
            m_bone_owners.clear();
            u32 used = 0;

            for(; i < m_bone_work.size() && m_bone_work[i].pass == pass_idx &&
                  m_bone_work[i].bucket == bucket_idx;
                i++)
            {
                auto const& work = m_bone_work[i];

                auto owner = std::find_if(
                    m_bone_owners.begin(),
                    m_bone_owners.end(),
                    [&work](auto const& o) { return o.first == work.parent; });

                i32 local;
                if(owner != m_bone_owners.end())
                    local = static_cast<i32>(owner->second);
                else if(used + work.bones <= kMaxBoneMatrices)
                {
                    local = static_cast<i32>(used);
                    m_bone_upload.resize(window + used + work.bones);
                    model_cache->evaluate_pose(
                        work.model,
                        *p.template get<AnimationPlayback>(work.parent),
                        Span<Matf4>(
                            m_bone_upload.data() + window + used, work.bones));
                    m_bone_owners.push_back({work.parent, used});
                    used += work.bones;
                    m_posed_instances++;
                } else
                {
                    m_denied_instances++;
                    local = kBoneDenied;
                }

                pass.matrix_mapping[work.instance].bone_base = local;
            }
        }
    }

    void update_materials(Proxy& p, time_point const& time)
    {
        if(!m_api->feature_info().program.buffer_binding)
            return;

        RenderingParameters* rendering_params;
        p.subsystem(rendering_params);

        ModelCache<Version>* model_cache;
        p.subsystem(model_cache);

        for(auto ent :
            p.template select<SubModel, DrawState, MeshTrackingData>())
        {
            if(!rendering_params->render_scenery &&
               (ent.tags() & ObjectSkybox) == 0)
                continue;
            auto [smodel, sm_draw, track] = ent.components();
            if(!followable(track.model_id))
                continue;
            Pass& pass = model_build()[sm_draw.current_pass];
            if(track.model_id.bucket >= pass.draws.size())
                continue;
            auto& bucket = pass.draws[track.model_id.bucket];
            if(bucket.empty() || track.model_id.draw >= bucket.size())
                continue;
            draw_data_t const& draw = bucket.at(track.model_id.draw);
            auto instance_id  = draw.instances.offset + track.model_id.instance;
            auto const* model = p.template get<Model>(smodel.parent);
            auto        functions =
                shader_cache.resolved_functions(model_context(*model));
            update_animations(
                model_material_of(sm_draw.current_pass, instance_id),
                smodel.shader,
                time,
                functions);
            if(static_cast<size_t>(instance_id) <
               pass.transparent_mapping.size())
            {
                /* A device's power is its A_out, and the scenario supplies a
                 * real one -- but an A_out set on the object itself is a
                 * deliberate override, so it wins. */
                auto const* obj = p.template get<ObjectSpawn>(smodel.parent);
                if(obj && obj->power >= 0.f && model->object_function[0] < 0.f)
                    functions[0] = obj->power;
                shader_cache.update_transparent_animations(
                    pass.transparent_of(instance_id),
                    smodel.shader,
                    time,
                    functions);
            }
        }

        for(auto ent : p.template select<BspReference, DrawState, Visibility>())
        {
            auto [bsp, bsp_draw, visibility] = ent.components();

            if(!visibility.visible_for(m_seat))
                continue;

            i32 instance_offset = bsp_draw.draw.data.front().instances.offset;
            update_animations(
                bsp_material_of(bsp_draw.current_pass, instance_offset),
                bsp.shader,
                time);
            Pass& pass = bsp_build()[bsp_draw.current_pass];
            if(instance_offset >= 0 && static_cast<size_t>(instance_offset) <
                                           pass.transparent_mapping.size())
                shader_cache.update_transparent_animations(
                    pass.transparent_of(instance_offset),
                    bsp.shader,
                    time,
                    shader_cache.resolved_functions({}));
        }
    }

    materials::shader_data& model_material_of(Passes which, size_t i)
    {
        return model_build()[which].material_of(i);
    }

    materials::shader_data& bsp_material_of(Passes which, size_t i)
    {
        return bsp_build()[which].material_of(i);
    }

    void populate_bsp_material(
        BspReference const& ref, Passes which, size_t i = 0)
    {
        Pass&                   pass     = bsp_build()[which];
        materials::shader_data& material = pass.material_of(i);
        shader_cache.populate_material(material, ref.shader, Vecf2{1, 1});
        bitm_cache.assign_atlas_data(material.lightmap, ref.lightmap);
        if(material.material.material == materials::id::sotr &&
           i < pass.transparent_mapping.size())
            shader_cache.populate_transparent_material(
                pass.transparent_of(i), ref.shader);
    }

    /* Always built: the object's own function values travel with it even
     * when change colours are off or its tag is not a unit. */
    std::optional<ShaderCache<halo_version>::material_context> model_context(
        Model const& model)
    {
        ShaderCache<halo_version>::material_context ctxt;
        ctxt.object_function = model.object_function;
        ctxt.meter_value     = model.meter_value;

        blam::tag_t const* tag = model.origin_object;
        if(m_render_params.color_changing && tag)
            switch(tag->tag_class())
            {
            case blam::tag_class_t::bipd:
            case blam::tag_class_t::vehi:
            case blam::tag_class_t::scen:
                ctxt.unit =
                    tag->template data<blam::scn::unit>(shader_cache.magic)
                        .value();
                break;
            default:
                break;
            }
        return ctxt;
    }

    void record_reflection_cube(
        Pass& pass, SubModel const& sub, u16 bucket, u16 draw)
    {
        auto refl = shader_cache.reflection_bitmap(sub.shader);
        if(!refl.valid())
            return;
        auto cube = bitm_cache.cube_texture(refl);
        if(!cube)
            return;
        pass.set_reflection(bucket, draw, std::move(cube));
    }

    void populate_mod2_material(
        SubModel const&                                            sub,
        Passes                                                     which,
        ModelItem<Version> const&                                  model,
        std::optional<ShaderCache<halo_version>::material_context> context,
        size_t                                                     i = 0,
        bool interior                                                = false)
    {
        Pass&                   pass     = model_build()[which];
        materials::shader_data& material = pass.material_of(i);
        shader_cache.populate_material(
            material, sub.shader, model.header->uvscale, context);
        if(interior)
            material.material.flags |= materials::flag_interior;
        if(material.material.material == materials::id::sotr &&
           i < pass.transparent_mapping.size())
            shader_cache.populate_transparent_material(
                pass.transparent_of(i), sub.shader);
    }

    void update_animations(
        materials::shader_data&   material,
        generation_idx_t const&   shader,
        time_point const&         time,
        std::array<f32, 4> const& functions = {{0.f, 0.f, 0.f, 0.f}})
    {
        shader_cache.update_uv_animations(material, shader, time, functions);
    }
};

template<typename Version = blam::xbox_version_t>
struct MeshRenderer
    : compo::RestrictedSubsystem<
          MeshRenderer<Version>,
          MeshRendererManifest<Version>>
{
    using type  = MeshRenderer;
    using Proxy = compo::proxy_of<MeshRendererManifest<Version>>;

    time_point last_update{};

    struct cached_player_t
    {
        u32   seat_idx;
        Matf4 matrix;
        Vecf3 position;
        Matf3 rotation;
    };

    gfx::api*            m_api;
    BlamResources&       m_resources;
    RenderingParameters& m_render_params;
    int                  m_render_flags{0x0};

    ShaderCache<Version>& shader_cache;
    BitmapCache<Version>& bitm_cache;
    BSPCache<Version>&    bsp_cache;

    std::vector<cached_player_t> m_players;

    std::shared_ptr<gfx::texture_2d_t> meow_tex;
    std::shared_ptr<gfx::sampler_t>    meow_sampler;

    std::shared_ptr<gfx::texture_2d_t> postprocess_tex;
    std::shared_ptr<gfx::sampler_t>    postprocess_sampler;

    struct pending_change_t
    {
        enum change_t
        {
            skybox,
        } type;

        union
        {
            i16 skybox_id;
        };
    };

    std::vector<pending_change_t> m_pending_changes;

    std::shared_ptr<GameEventBus::queue_type<ClusterChangedEvent>>
        m_cluster_events;

    i16 current_skybox{-1};

    MeshRenderer(
        gfx::api*             api,
        BlamResources&        resources,
        RenderingParameters&  render,
        ShaderCache<Version>& shader_cache,
        BitmapCache<Version>& bitm_cache,
        BSPCache<Version>&    bsp_cache)
        : m_api(api)
        , m_resources(resources)
        , m_render_params(render)
        , shader_cache(shader_cache)
        , bitm_cache(bitm_cache)
        , bsp_cache(bsp_cache)
    {
        this->priority = 3072;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    BSPItem const* get_bsp(generation_idx_t bsp)
    {
        if(!bsp.valid())
            return nullptr;
        auto it = bsp_cache.find(bsp);
        if(it == bsp_cache.m_cache.end())
            return nullptr;
        return &it->second;
    }

    size_t align_for_gpu_padding(size_t size) const
    {
        u32 padding = m_api->limits().buffers.ubo_alignment;
        if((size % padding) == 0)
            return size;
        u32 mask           = padding - 1;
        u32 unaligned_size = size & mask;
        u32 added_padding  = padding - unaligned_size;
        return size + added_padding;
    }

    void setup_textures(std::vector<gfx::sampler_definition_t>& samplers)
    {
        samplers.clear();

        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"lightmaps"sv, 4},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::RGB565))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_bc1"sv, 0},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        CompFmt(pix_fmt::BCn, comp_flags::BC1))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_bc2"sv, 1},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        CompFmt(pix_fmt::BCn, comp_flags::BC2))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_bc3"sv, 2},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        CompFmt(pix_fmt::BCn, comp_flags::BC3))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_rgb565"sv, 3},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::RGB565))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_r8"sv, 5},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::R8))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_rg8"sv, 6},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::RG8))
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_rgba4"sv, 7},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::RGBA4))
                    .sampler});

        if(m_api->limits().textures.texture_units <= 8)
            return;

        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_rgba8"sv, 8},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_2da_t>(
                        PixDesc(pix_fmt::RGBA8))
                    .sampler});
#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION_ES >= 0x320
        if(std::get<0>(m_api->api_version()) == 2)
            return;
        if(!m_api->feature_info().texture.cube_array)
            return;
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_cube_bc1"sv, 9},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_cube_array_t>(
                        CompFmt(pix_fmt::BCn, comp_flags::BC1),
                        blam::bitm::type_t::tex_cube)
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_cube_rgb565"sv, 10},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_cube_array_t>(
                        PixDesc(pix_fmt::RGB565), blam::bitm::type_t::tex_cube)
                    .sampler});
        samplers.push_back(
            gleam::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"source_cube_rgba8"sv, 11},
                bitm_cache
                    .template get_bucket<gfx::compat::texture_cube_array_t>(
                        PixDesc(pix_fmt::RGBA8), blam::bitm::type_t::tex_cube)
                    .sampler});
#endif
        for(auto& sampler : samplers)
        {
            std::get<2>(sampler)->set_lod_bias(m_render_params.tex_res);
        }
    }

    auto get_view_state(u32 player_idx)
    {
        using typing::vector_types::Vecd2;
        using typing::vector_types::Veci4;

        const auto depth = gfx::depth_state{
            .range    = Vecd2{0.0, 1.0},
            .reversed = true,
        };

        if(m_api->default_rendertarget() == m_resources.offscreen)
            return gfx::view_state{
                .depth = gfx::depth_state{
                    .range    = depth.range,
                    .reversed = true,
                }};

        auto view = Veci4{
            0,
            0,
            m_resources.offscreen_size.x,
            m_resources.offscreen_size.y,
        };

        switch(m_players.size())
        {
        case 1:
            view.y = m_resources.offscreen_size.y;
            view.z = m_resources.offscreen_size.x;
            view.w = m_resources.offscreen_size.y;
            break;
        case 2:
            view.y = (m_resources.offscreen_size.y / 2) * (player_idx + 1);
            view.z = m_resources.offscreen_size.x;
            view.w = m_resources.offscreen_size.y / 2;
            break;
        case 3:
        case 4:
            view.x = (m_resources.offscreen_size.x / 2) * (player_idx % 2);
            view.y = (m_resources.offscreen_size.y / 2) * (player_idx / 2 + 1);
            view.z = m_resources.offscreen_size.x / 2;
            view.w = m_resources.offscreen_size.y / 2;
            break;
        }
        view.y = m_resources.offscreen_size.y - view.y;

        return gfx::view_state{
#if !defined(COFFEE_EMSCRIPTEN)
            .view = view,
#endif
            .depth = depth,
        };
    }

    gfx::uniform_pair<const int> get_renderflag_uniform()
    {
        m_render_flags =
            (m_render_params.render_fog ? 0x1 : 0) |
            (m_render_params.render_lightmaps ? 0x2 : 0) |
            (m_render_params.render_reflection ? 0x4 : 0) |
            (m_render_params.render_model_bones ? 0x8 : 0) |
            (m_render_params.only_normals ? 0x10 : 0) |
            (m_render_params.only_normalmaps ? 0x20 : 0) |
            (m_render_params.only_lightmaps ? 0x40 : 0) |
            (m_render_params.only_reflections ? 0x80 : 0) |
            (m_render_params.only_multipurpose ? 0x100 : 0) |
            (m_render_params.only_multipurpose2 ? 0x200 : 0) |
            (m_render_params.only_diffuse ? 0x400 : 0) |
            (std::is_same_v<halo_version, blam::xbox_version_t> ? 0x800 : 0) |
            (m_render_params.only_detail ? 0x1000 : 0) |
            (m_render_params.only_micro ? 0x2000 : 0) |
            (m_render_params.only_aux_channels ? 0x4000 : 0) |
            (m_render_params.interior ? 0x8000 : 0);
        return gfx::uniform_pair{
            {"render_flags"sv, 31},
            semantic::SpanOne<const int>(m_render_flags),
        };
    }

    template<typename... Args>
    void render_pass(Proxy&, u32 idx, f32 t, Pass const& pass, Args&&... extra)
    {
        using namespace typing::vector_types;

        if(!const_config::supports_splitscreen && idx != 0)
            return;
        if(idx >= m_players.size())
            return;

        auto        _ = m_api->debug().scope(pass.name);
        ProfContext __;

        auto const& player = m_players[idx];

        auto vertex_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"camera"sv, 1}, semantic::SpanOne<const Matf4>(player.matrix)},
            get_renderflag_uniform());
        auto fragment_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{
                {"cameraRotation", 2},
                semantic::SpanOne(player.rotation),
            },
            gfx::uniform_pair{
                {"camera_position", 21},
                semantic::SpanOne(player.position),
            },
            gfx::uniform_pair{
                {"time", 22},
                semantic::SpanOne<const f32>(t),
            },
            get_renderflag_uniform());
        auto buffers = gfx::make_buffer_list(
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MatrixStore"sv, 0},
                pass.matrix_buffer,
                sizeof(PerInstanceData),
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"MaterialProperties"sv, 1},
                pass.material_buffer,
                sizeof(materials::shader_data),
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"WorldProperties"sv, 2},
                m_resources.world_store->slice(0),
                0,
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Vertex,
                {"BoneMatrices"sv, 3},
                /* Replaced per bucket below; each one binds its own window. */
                m_resources.bone_matrix_buf->slice(0),
                0,
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"TransparentProperties"sv, 4},
                pass.transparent_buffer,
                sizeof(materials::transparent_data),
            });

        std::vector<gfx::sampler_definition_t> samplers;
        setup_textures(samplers);

        const bool per_draw_cubes = !m_api->feature_info().texture.cube_array;
        std::shared_ptr<gfx::texture_t> cube_fallback;
        if(per_draw_cubes)
            for(auto const& bucket : pass.reflection_textures)
            {
                for(auto const& cube : bucket)
                    if(cube)
                    {
                        cube_fallback = cube;
                        break;
                    }
                if(cube_fallback)
                    break;
            }

        for(size_t bucket_idx = 0; bucket_idx < pass.draws.size(); bucket_idx++)
        {
            auto const& draw = pass.draws[bucket_idx];

            /* Point the bone block at this bucket's own window; bone_base was
             * handed out relative to it. */
            for(auto& buffer : buffers)
                if(buffer.key.name == "BoneMatrices"sv)
                {
                    size_t base =
                        bucket_idx < pass.bucket_bone_base.size()
                            ? pass.bucket_bone_base[bucket_idx] * sizeof(Matf4)
                            : 0;
                    size_t span = DrawListBuilder<Version>::kMaxBoneMatrices *
                                  sizeof(Matf4);
                    /* allocate_bones keeps every window inside the buffer and
                     * on an aligned offset; this only catches a bucket that
                     * was never given one. */
                    if(base + span > m_resources.bone_matrix_buf->size())
                        base = 0;
                    buffer.buffer =
                        m_resources.bone_matrix_buf->slice(base, span);
                }

            gfx::base_instance_sampler_list cube_slots;
            if(per_draw_cubes && cube_fallback)
                cube_slots.slots.push_back(
                    gfx::base_instance_sampler_t{
                        .stage    = typing::graphics::ShaderStage::Fragment,
                        .location = {"source_cube"sv, 19},
                        .sampler  = bitm_cache.cube_sampler(),
                        .textures = pass.reflections_for(bucket_idx),
                        .fallback = cube_fallback,
                    });

            auto res = m_api->submit(
                {
                    .program = pass.programs.for_classes(
                        bucket_idx < pass.bucket_classes.size()
                            ? pass.bucket_classes[bucket_idx]
                            : pass.material_classes),
                    .vertices      = pass.command.vertices,
                    .render_target = m_resources.offscreen,
                    .call          = pass.command.call,
                    .data          = draw,
                },
                vertex_u,
                fragment_u,
                buffers,
                get_view_state(idx),
                samplers,
                cube_slots,
                std::forward<Args&&>(extra)...);
            if(res)
                cFatal("submit error: {}", std::get<1>(*res));
        }
    }

    template<typename... Args>
    void render_bsp_pass(
        Proxy&, u32 idx, f32 t, Pass const& pass, Args&&... extra)
    {
        using namespace typing::vector_types;

        if(!const_config::supports_splitscreen && idx != 0)
            return;
        if(idx >= m_players.size())
            return;

        auto        _ = m_api->debug().scope(pass.name);
        ProfContext __;

        auto const& player = m_players[idx];

        /* Step 1: Set up shared uniform state + buffers */
        auto vertex_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair{
                {"camera"sv, 1}, semantic::SpanOne(player.matrix)},
            get_renderflag_uniform());
        auto fragment_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair{
                {"camera_position", 21},
                semantic::SpanOne<const Vecf3>(player.position),
            },
            gfx::uniform_pair{
                {"time", 22},
                semantic::SpanOne<const f32>(t),
            },
            get_renderflag_uniform());
        auto buffers = gfx::make_buffer_list(
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"MaterialProperties"sv, 1},
                pass.material_buffer,
                sizeof(materials::shader_data),
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"WorldProperties"sv, 2},
                m_resources.world_store->slice(0),
                0,
            },
            gfx::buffer_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"TransparentProperties"sv, 4},
                pass.transparent_buffer,
                sizeof(materials::transparent_data),
            });

        /* Step 2: Set up all the textures */
        std::vector<gfx::sampler_definition_t> samplers;
        setup_textures(samplers);

        for(size_t bucket_idx = 0; bucket_idx < pass.draws.size(); bucket_idx++)
        {
            auto const& draw = pass.draws[bucket_idx];
            /* Step 3: DRAW */
            auto res = m_api->submit(
                {
                    .program = pass.programs.for_classes(
                        bucket_idx < pass.bucket_classes.size()
                            ? pass.bucket_classes[bucket_idx]
                            : pass.material_classes),
                    .vertices      = pass.command.vertices,
                    .render_target = m_resources.offscreen,
                    .call          = pass.command.call,
                    .data          = draw,
                },
                vertex_u,
                fragment_u,
                buffers,
                get_view_state(idx),
                gfx::cull_state{
                    .front_face = true,
                },
                samplers,
                std::forward<Args&&>(extra)...);
            if(res)
                cFatal("submit error: {}", std::get<1>(*res));
        }
    }

    void render_debug_lines(Proxy& e)
    {
        if(m_api->workarounds().bugs.adreno)
            return;

        DebugMarkers* markers;
        e.subsystem(markers);
        if(!markers->available())
            return;

        ProfContext _;

        std::vector<gfx::draw_command::data_t> groups;
        RenderingParameters*                   params;
        e.subsystem(params);

        for(auto ent : e.template select<BspReference>())
        {
            if(!params->debug_portals && !params->debug_clusters)
                break;
            BspReference const& bsp_ = ent.template get<BspReference>();
            BSPItem const*      bsp  = get_bsp(bsp_.bsp);

            if(params->debug_portals)
            {
                groups.insert(
                    groups.end(), bsp->portals.begin(), bsp->portals.end());
            } else if(params->debug_clusters)
            {
                u32 cur = params->current_bsp_cluster;
                if(cur < bsp->clusters.size())
                {
                    for(auto const& sub : bsp->clusters[cur].sub)
                        if(sub.debug_color_idx < bsp->portals.size())
                            groups.push_back(bsp->portals[sub.debug_color_idx]);
                }
            }
        }
        for(auto ent : e.template select<DebugDraw>())
        {
            if(!params->debug_markers)
                continue;
            if(!params->debug_triggers && ent.tags() & ObjectTriggerVolume)
                continue;
            DebugDraw const& draw = ent.template get<DebugDraw>();
            if(draw.data.arrays.count == 0)
                continue;
            groups.push_back(draw.data);
            groups.back().instances.offset = draw.color_ptr;
        }

        markers->commit();

        u32 const vert_base  = markers->vert_region_base();
        u32 const color_base = markers->color_region_base();
        for(auto& group : groups)
        {
            group.arrays.offset += vert_base;
            group.instances.offset += color_base;
        }

        Matf4 debug_matrix =
            m_players.empty() ? glm::identity<Matf4>() : m_players[0].matrix;
        m_api->submit(
            {
                .program       = m_resources.debug_lines_pipeline,
                .vertices      = m_resources.debug_attr,
                .render_target = m_resources.offscreen,
                .call =
                    gfx::draw_command::call_spec_t{
                        .indexed = false,
                        .mode    = gfx::drawing::primitive::line_strip,
                    },
                .data = groups,
            },
            gfx::make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                gfx::uniform_pair{
                    {"camera"sv, 0}, semantic::SpanOne(debug_matrix)}),
            get_view_state(0));
    }

    void upload_draw_lists(DrawListBuilder<Version> const& builder)
    {
        if(!m_api->feature_info().program.buffer_binding)
            return;

        ProfContext _("MeshRenderer::upload_draw_lists");

        std::vector<gleam::buffer_t*> discarded;
        auto                          upload = [&discarded](
                          gfx::buffer_slice_t const& target, auto const& src) {
            using value_type = typename std::decay_t<decltype(src)>::value_type;
            if(src.empty() || !target.valid())
                return;
            auto slice = target;
            if(auto* parent = slice.parent();
               std::find(discarded.begin(), discarded.end(), parent) ==
               discarded.end())
            {
                slice.discard_parent();
                discarded.push_back(parent);
            }
            slice.update(Span<const value_type>(src));
        };

        auto upload_set = [&upload](auto const& passes) {
            for(auto const& pass : passes)
            {
                upload(pass.material_buffer, pass.material_staging);
                upload(pass.transparent_buffer, pass.transparent_staging);
                upload(pass.matrix_buffer, pass.matrix_staging);
            }
        };

        for(u32 vp = 0; vp < builder.view_count(); vp++)
        {
            upload_set(builder.bsp_submit(vp));
            upload_set(builder.model_submit(vp));
        }

        if(!builder.bone_upload().empty())
            m_resources.bone_matrix_buf->update(
                0, Span<const Matf4>(builder.bone_upload()));
    }

    void start_restricted(Proxy& p, time_point const& time)
    {
        ProfContext _;

        if(m_cluster_events)
            m_cluster_events->poll();

        upload_draw_lists(p.template subsystem<DrawListBuilder<Version>>());

        /* Collect active local players sorted by seat_idx */
        m_players.clear();
        for(auto const entity : p.template select<PlayerCamera, PlayerInfo>())
        {
            auto const& [cam, info] = entity.components();
            if(info.is_remote() || !cam.is_active())
                continue;
            m_players.push_back({
                .seat_idx = info.seat_idx,
                .matrix   = cam.matrix,
                .position = cam.camera.position,
                .rotation = glm::mat3_cast(cam.camera.rotation),
            });
        }
        std::sort(
            m_players.begin(),
            m_players.end(),
            [](auto const& a, auto const& b) {
                return a.seat_idx < b.seat_idx;
            });

        // Performance is terrible on Emscripten when updating every frame
        // We need a more efficient way to update the buffer in that case
        // Also we don't use this data when rendering using legacy codepath
        // We create our own batching there based on different rules

        RenderingParameters const* rendering_props;
        p.subsystem(rendering_props);

        if(rendering_props->debug_clear)
            m_resources.offscreen->clear(
                Vecf4(rendering_props->clear_color, 1.f));
        else
        {
            bool skybox_found{false};
            for(auto const& skybox : p.template select<WorldInfo>())
            {
                skybox_found           = true;
                WorldInfo const& world = skybox.template get<WorldInfo>();
                if(!world.skybox)
                    continue;
                m_resources.offscreen->clear(
                    Vecf4(world.skybox->outdoor_fog.color, 1));
            }
            if(!skybox_found)
                m_resources.offscreen->clear(Vecf4{0, 0, 0, 1});
        }

        // Check if shaders are compiled
        do
        {
            LoadingStatus* loading_state;
            p.subsystem(loading_state);

            auto shader_state = m_resources.check_shaders_ready();
            if(shader_state.has_error())
                return;
            loading_state->set_shader_progress(
                shader_state.value().ready, shader_state.value().total);
            loading_state->check_all_loaded(true);
            if(loading_state->loading)
                return;
        } while(false);

        /* to_f32 gives seconds-since-epoch (~1.7e9). At that magnitude
         * float32 precision is ~128 s, so adjacent frames are identical
         * and UV animations freeze. Wrap to a shorter cycle. */
        f32 t = std::fmod(stl_types::chrono::to_f32(time), 3600.f);

        gfx::system& system       = p.template subsystem<gfx::system>();
        auto         render_timer = system.gpu_timer("MeshRenderer full pass");

        auto blend_for_pass = [](Passes pass) -> gfx::blend_state {
            switch(pass)
            {
            case Pass_SkyAdditive:
            case Pass_Additive:
                return {.additive = true};
            case Pass_SkyMultiply:
            case Pass_Multiply:
                return {.multiply = true};
            case Pass_Max:
                return {.maximum = true};
            default:
                return {};
            }
        };

        /* Primary player is always the first one (seat_idx == 0) */
        u32 primary_player = 0;

        /* Read-only view of what the builder produced this frame */
        auto const& builder = p.template subsystem<DrawListBuilder<Version>>();

        // Opaque world geometry — all players, depth write + stencil write.
        gfx::stencil_state opaque_stencil{
            .depth_pass = gfx::stencil_state::operation_t::write,
            .mask       = 0x1,
            .reference  = 0x1,
        };
        gfx::cull_state cull_front{
            .front_face = true,
        };
        for(i32 pi = Pass_Opaque; pi <= Pass_LastOpaque; ++pi)
        {
            auto pass = static_cast<Passes>(pi);
            for(auto i : stl_types::range<u32>(m_players.size()))
            {
                render_bsp_pass(
                    p, i, t, builder.bsp_submit(i)[pass], opaque_stencil);
                render_pass(
                    p,
                    i,
                    t,
                    builder.model_submit(i)[pass],
                    opaque_stencil,
                    cull_front);
            }
        }

        // Special case for 3 players; black out the 4th quadrant
        if(m_players.size() == 3 && !compile_info::platform::is_emscripten)
        {
            ScreenClear* clear;
            p.subsystem(clear);

            while(!meow_tex)
            {
                auto pause_tex =
                    ktx::load_from("textures/meow.0.etc2"_rsc.data());
                if(!pause_tex)
                {
                    cWarning("Failed to load blanking texture");
                    break;
                }
                auto        meow_data = std::move(pause_tex.value());
                auto const& meow_size = meow_data.mips.at(0).size;
                meow_tex              = m_api->alloc_texture(
                    gfx::textures::d2,
                    CompFmt(
                        comp_app::pix_fmt::ETC2, typing::pixels::pix_flags::RG),
                    1);
                meow_tex->alloc(size_3d<u32>{meow_size.x, meow_size.y, 1u});
                meow_tex->upload(
                    meow_data.mips.at(0).data,
                    Veci2{},
                    Veci2{meow_size.x, meow_size.y});
                meow_sampler = meow_tex->sampler();
                meow_sampler->alloc();
                meow_sampler->set_edge_policy(
                    1, typing::WrapPolicy::MirrorClamp);
                meow_tex->set_swizzle(
                    gfx::textures::swizzle_t::red,
                    gfx::textures::swizzle_t::red,
                    gfx::textures::swizzle_t::red,
                    gfx::textures::swizzle_t::alpha);
                break;
            }

            auto size =
                p.template service<comp_app::GraphicsFramebuffer>()->size();
            clear->extra_quads.push_back({
                .position    = Vecf2(size.w / 2.f, 0),
                .size        = Vecf2(size.w / 2.f, size.h / 2.f),
                .atlas_scale = Vecf2{1.f, -1.f},
                .sampler     = meow_sampler,
            });
        }

        {
            gfx::depth_extended_state sky_depth{.depth_write = false};
            for(i32 pi = Pass_SkyOpaque; pi <= Pass_LastSky; ++pi)
            {
                auto pass  = static_cast<Passes>(pi);
                auto blend = blend_for_pass(pass);
                for(auto i : stl_types::range<u32>(m_players.size()))
                    render_pass(
                        p,
                        i,
                        t,
                        builder.model_submit(i)[pass],
                        blend,
                        sky_depth);
            }
        }

        // Transparent world geometry — primary player, no depth write.
        gfx::depth_extended_state transparent_depth{.depth_write = false};
        for(i32 pi = Pass_LastOpaque + 1; pi <= Pass_Max; ++pi)
        {
            auto pass = static_cast<Passes>(pi);
            for(auto i : stl_types::range<u32>(m_players.size()))
            {
                auto blend = blend_for_pass(pass);
                render_pass(
                    p,
                    i,
                    t,
                    builder.model_submit(i)[pass],
                    blend,
                    transparent_depth);
                render_bsp_pass(
                    p,
                    i,
                    t,
                    builder.bsp_submit(i)[pass],
                    blend,
                    transparent_depth);
            }
        }

        if(!postprocess_tex)
        {
            postprocess_tex = m_api->alloc_texture(
                gfx::textures::d2,
                m_resources.color->m_format,
                1);
            postprocess_tex->alloc(m_resources.color->size());
            postprocess_sampler = postprocess_tex->sampler();
            postprocess_sampler->alloc();
        }
        auto post_size = m_resources.color->size();
        if(postprocess_tex->size() != m_resources.color->size())
        {
            postprocess_tex->alloc({
                post_size.w,
                post_size.h,
                1,
            });
        }
        m_api->perform_copy(m_resources.color, postprocess_tex);

        PostProcessParameters const* postproc;
        p.subsystem(postproc);

        // Draws on top of framebuffer
        for(i32 pi = Pass_Postprocess; pi < Pass_Count; ++pi)
        {
            auto pass = static_cast<Passes>(pi);
            for(auto i : stl_types::range<u32>(m_players.size()))
            {
                /* UV units of displacement at the silhouette. */
                render_pass(
                    p,
                    i,
                    t,
                    builder.model_submit(i)[pass],
                    transparent_depth,
                    gfx::make_uniform_list(
                        typing::graphics::ShaderStage::Fragment,
                        gfx::uniform_pair{
                            gfx::uniform_key{"camo_scale"sv, 24},
                            semantic::SpanOne(postproc->camo_scale)
                        }),
                    gfx::make_sampler_list(
                        gfx::sampler_definition_t{
                            typing::graphics::ShaderStage::Fragment,
                            gfx::uniform_key{"source_scene"sv, 12},
                            postprocess_sampler,
                        }));
            }
        }

        render_debug_lines(p);
    }
};

template<typename Ver = blam::xbox_version_t>
struct LegacyMeshRenderer
    : compo::RestrictedSubsystem<
          LegacyMeshRenderer<Ver>,
          LegacyMeshRendererManifest<Ver>>
{
    using type  = LegacyMeshRenderer<Ver>;
    using Proxy = compo::proxy_of<LegacyMeshRendererManifest<Ver>>;

    struct LegacyBatch
    {
        gfx::draw_command::call_spec_t         call;
        std::vector<gfx::draw_command::data_t> data;
        std::shared_ptr<gfx::texture_t>        lightmap;
        std::shared_ptr<gfx::sampler_t>        lightmap_sampler;
        std::vector<gfx::texture_t*>           base_map;
        std::shared_ptr<gfx::sampler_t>        base_sampler;
        std::vector<Vecf2>                     base_map_scale;
        std::vector<gfx::texture_t*>           micro_map;
        std::shared_ptr<gfx::sampler_t>        micro_sampler;
        std::vector<Vecf2>                     micro_map_scale;
    };

    void start_restricted(Proxy& p, time_point const& time)
    {
        f32 t = std::fmod(stl_types::chrono::to_f32(time), 3600.f);
        legacy_render(p, t);
    }

    void legacy_render(Proxy& p, f32 t)
    {
        using typing::vector_types::Vecd2;

        BlamResources const& resources = p.template subsystem<BlamResources>();

        do
        {
            LoadingStatus* loading_state;
            p.subsystem(loading_state);

            auto shader_state = resources.check_shaders_ready();
            if(shader_state.has_error())
                return;
            loading_state->set_shader_progress(
                shader_state.value().ready, shader_state.value().total);
            loading_state->check_all_loaded(true);
            if(loading_state->loading)
                return;
        } while(false);

        RenderingParameters const* rendering_props;
        gfx::system*               api;
        p.subsystem(rendering_props);
        p.subsystem(api);
        BitmapCache<Ver>& bitm_cache = p.template subsystem<BitmapCache<Ver>>();
        ShaderCache<Ver>& shader_cache =
            p.template subsystem<ShaderCache<Ver>>();

        PlayerCamera const* player_cam{};
        for(auto player : p.template select<PlayerInfo, PlayerCamera>())
        {
            auto [info, camera] = player.components();
            if(info.seat_idx != 0)
                continue;
            player_cam = &camera;
        }

        if(!player_cam)
            cWarning("No player camera");

        ProfContext                       _;
        std::map<cache_id_t, LegacyBatch> batches;
        for(auto const& ent :
            p.template select<BspReference, DrawState, Visibility>())
        {
            auto const& [bsp_ref, bsp_draw, vis] = ent.components();
            if(!vis.visible_for(0))
                continue;
            if(!bsp_ref.shader.valid() || !bsp_ref.lightmap.valid())
                continue;
            ShaderItem const& shader =
                shader_cache.find(bsp_ref.shader)->second;
            if(shader.tag_class != blam::tag_class_t::senv)
                continue;
            BitmapItem const& lightm =
                bitm_cache.find(bsp_ref.lightmap)->second;
            // blam::shader::shader_env const* senv =
            //     reinterpret_cast<blam::shader::shader_env const*>(
            //         shader.header);
            LegacyBatch& batch = batches[bsp_ref.lightmap.i];
            auto         light_bucket =
                bitm_cache.template get_bucket<gfx::compat::texture_2da_t>(
                    lightm.image.fmt);

            auto setup_texture = [&](generation_idx_t                 ref,
                                     std::vector<gfx::texture_t*>&    array,
                                     std::shared_ptr<gfx::sampler_t>& sampler) {
                if(!ref.valid())
                {
                    array.push_back(nullptr);
                    return;
                }
                BitmapItem const& bitm = bitm_cache.find(ref)->second;
                auto              bucket =
                    bitm_cache.template get_bucket<gfx::compat::texture_2da_t>(
                        bitm.image.fmt);
                array.push_back(
                    bucket.template texture_as<gfx::compat::texture_2da_t>()
                        .subtexture(bitm.image.layer)
                        .get());
                sampler = bucket.sampler;
            };

            // Texture setup
            batch.lightmap =
                light_bucket.template texture_as<gfx::compat::texture_2da_t>()
                    .subtexture(lightm.image.layer);
            batch.lightmap_sampler = light_bucket.sampler;

            setup_texture(
                shader.senv.base_bitm, batch.base_map, batch.base_sampler);
            setup_texture(
                shader.senv.micro_bitm, batch.micro_map, batch.micro_sampler);

            // Draw call setup
            batch.call = bsp_draw.draw.call;
            batch.data.push_back(bsp_draw.draw.data.front());
        }

        Coffee::Profiler::PushContext("legacy_render: Opaque BSP");
        for(auto const& [_, light_group] : batches)
        {
            Vecf2 base_map_scale{1, 1};
            Vecf2 micro_map_scale{8, 8};
            auto  vertex_u = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                gfx::uniform_pair{
                     {"camera"sv},
                    semantic::SpanOne<const Matf4>(player_cam->matrix)});
            auto fragment_u = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Fragment,
                gfx::uniform_pair{
                    {"base_map_scale"sv},
                    semantic::SpanOne<const Vecf2>(base_map_scale)},
                gfx::uniform_pair{
                    {"micro_map_scale"sv},
                    semantic::SpanOne<const Vecf2>(micro_map_scale)});
            light_group.lightmap_sampler->rebind(light_group.lightmap);
            std::vector<gfx::sampler_definition_t> samplers;
            samplers.push_back(
                gleam::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"lightmap"sv, 0},
                    light_group.lightmap_sampler});
            auto texture_lists = gfx::make_instance_textures(
                gfx::instance_texture_t{
                    .stage    = typing::graphics::ShaderStage::Fragment,
                    .uniform  = gfx::uniform_key{"base_map"sv, 1},
                    .sampler  = light_group.base_sampler,
                    .textures = light_group.base_map,
                },
                gfx::instance_texture_t{
                    .stage    = typing::graphics::ShaderStage::Fragment,
                    .uniform  = gfx::uniform_key{"micro_map"sv, 2},
                    .sampler  = light_group.micro_sampler,
                    .textures = light_group.micro_map,
                });
            ProfContext __("Draw submission");
            api->submit(
                {
                    .program       = resources.bsp_pipeline,
                    .vertices      = resources.bsp_attr,
                    .render_target = resources.offscreen,
                    .call =
                        gfx::draw_command::call_spec_t{
                            .indexed = true,
                            .mode    = gfx::drawing::primitive::triangle,
                        },
                    .data = light_group.data,
                },
                vertex_u,
                fragment_u,
                samplers,
                texture_lists,
                gfx::cull_state{.front_face = true},
                gfx::view_state{
                    .depth =
                        gfx::depth_state{
                            .range    = Vecd2{0.0, 1.0},
                            .reversed = true,
                        },
                });
        }
        Coffee::Profiler::PopContext();

        Coffee::Profiler::PushContext("legacy_render: Water BSP");
        for(auto const& ref : p.template select<BspReference, DrawState>())
        {
            auto const& [bsp_ref, bsp_draw] = ref.components();
            if(!bsp_ref.shader.valid())
                continue;
            auto sh_it = shader_cache.find(bsp_ref.shader);
            if(sh_it == shader_cache.end())
                continue;
            if(sh_it->second.tag_class != blam::tag_class_t::swat)
                continue;
            generation_idx_t base_id = sh_it->second.swat.base;
            if(!base_id.valid())
                continue;
            auto b_it = bitm_cache.find(base_id);
            if(b_it == bitm_cache.end())
                continue;
            BitmapItem const& b = b_it->second;
            auto              base_tex =
                bitm_cache.template get_bucket<gfx::compat::texture_2da_t>(
                    b.image.fmt);
            base_tex.sampler->rebind(
                base_tex.template texture_as<gfx::compat::texture_2da_t>()
                    .subtexture(b.image.layer));

            std::vector<gfx::sampler_definition_t> samplers;
            samplers.push_back(
                gleam::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"base"sv, 0},
                    base_tex.sampler});

            /* Ripple layer (swat bump/ripple map) */
            i32 has_ripple = 0;
            if(auto rid = sh_it->second.swat.bump; rid.valid())
            {
                if(auto r_it = bitm_cache.find(rid); r_it != bitm_cache.end())
                {
                    BitmapItem const& rb = r_it->second;
                    auto              rtex =
                        bitm_cache
                            .template get_bucket<gfx::compat::texture_2da_t>(
                                rb.image.fmt);
                    rtex.sampler->rebind(
                        rtex.template texture_as<gfx::compat::texture_2da_t>()
                            .subtexture(rb.image.layer));
                    samplers.push_back(
                        gleam::sampler_definition_t{
                            typing::graphics::ShaderStage::Fragment,
                            {"ripple"sv, 1},
                            rtex.sampler});
                    has_ripple = 1;
                }
            }

            /* Lightmap modulation */
            i32   has_light = 0;
            Vecf2 light_scale{1, 1}, light_offset{0, 0};
            if(bsp_ref.lightmap.valid())
            {
                if(auto l_it = bitm_cache.find(bsp_ref.lightmap);
                   l_it != bitm_cache.end())
                {
                    BitmapItem const& lm = l_it->second;
                    auto              ltex =
                        bitm_cache
                            .template get_bucket<gfx::compat::texture_2da_t>(
                                lm.image.fmt);
                    ltex.sampler->rebind(
                        ltex.template texture_as<gfx::compat::texture_2da_t>()
                            .subtexture(lm.image.layer));
                    samplers.push_back(
                        gleam::sampler_definition_t{
                            typing::graphics::ShaderStage::Fragment,
                            {"lightmap"sv, 2},
                            ltex.sampler});
                    light_scale  = lm.image.scale;
                    light_offset = lm.image.offset;
                    has_light    = 1;
                }
            }

            auto vtx_u = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                gfx::uniform_pair{
                    {"camera"sv},
                    semantic::SpanOne<const Matf4>(player_cam->matrix)});
            auto frg_u = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Fragment,
                gfx::uniform_pair{{"time"sv}, semantic::SpanOne<const f32>(t)},
                gfx::uniform_pair{
                    {"has_ripple"sv}, semantic::SpanOne<const i32>(has_ripple)},
                gfx::uniform_pair{
                    {"has_light"sv}, semantic::SpanOne<const i32>(has_light)},
                gfx::uniform_pair{
                    {"light_scale"sv},
                    semantic::SpanOne<const Vecf2>(light_scale)},
                gfx::uniform_pair{
                    {"light_offset"sv},
                    semantic::SpanOne<const Vecf2>(light_offset)});
            api->submit(
                {
                    .program       = resources.water_pipeline,
                    .vertices      = resources.bsp_attr,
                    .render_target = resources.offscreen,
                    .call =
                        gfx::draw_command::call_spec_t{
                            .indexed = true,
                            .mode    = gfx::drawing::primitive::triangle,
                        },
                    .data = bsp_draw.draw.data,
                },
                vtx_u,
                frg_u,
                samplers,
                gfx::blend_state{}, /* standard src-alpha transparency */
                gfx::view_state{
                    .depth =
                        gfx::depth_state{
                            .range    = Vecd2{0.0, 1.0},
                            .reversed = true,
                        },
                });
        }
        Coffee::Profiler::PopContext();

        if(!rendering_props->render_scenery)
            return;

        ModelCache<Ver>* model_cache;
        p.subsystem(model_cache);

        Coffee::Profiler::PushContext("legacy_render: Scenery");
        Matf4 const& camera = player_cam->matrix;

        constexpr u32 model_batch_size = 32;

        struct ModelBatch
        {
            generation_idx_t          shader;
            gfx::draw_command::data_t geom;
            Vecf2                     base_scale{1, 1};
            std::vector<Matf4>        transforms;
        };

        std::map<std::pair<cache_id_t, u64>, ModelBatch> soso_batches;

        /* Model lives on the parent entity, SubModel on its children —
         * they are never on the same entity, so Model cannot be part of
         * the fused select here */
        for(auto ent : p.template select<SubModel, DrawState>())
        {
            auto const& [sm, sm_draw] = ent.components();
            auto         parent       = p.template ref<Proxy>(sm.parent);
            Model const& mod          = parent.template get<Model>();
            if(!parent.template get<Visibility>().visible_for(0) ||
               !sm.shader.valid())
                continue;

            auto shader_it = shader_cache.find(sm.shader);
            if(shader_it == shader_cache.end())
                continue;
            ShaderItem const& shitem   = shader_it->second;
            auto              model_it = model_cache->find(mod.model);
            if(model_it == model_cache->end())
                continue;
            ModelItem<Ver> const& mitem = model_it->second;

            /* schi/scex (incl. the sky dome) — simplified multi-map combiner.
             * Distinct per-object map sets and few instances; drawn inline as a
             * single-instance batch (models[0], glw_InstanceID==0). */
            if(shitem.tag_class == blam::tag_class_t::schi ||
               shitem.tag_class == blam::tag_class_t::scex)
            {
                auto vtx_u = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair{
                        {"camera"sv, 1},
                        semantic::SpanOne<const Matf4>(camera)},
                    gfx::uniform_pair{
                        {"models"sv, 2},
                        semantic::SpanOne<const Matf4>(mod.transform)});
                auto const& maps = shitem.tag_class == blam::tag_class_t::schi
                                       ? shitem.schi.maps
                                       : shitem.scex.maps;
                static constexpr std::array<std::string_view, 4> map_names = {
                    {"map0"sv, "map1"sv, "map2"sv, "map3"sv}};
                std::vector<gfx::sampler_definition_t> samplers;
                Vecf2                                  scale0{1, 1};
                i32                                    count = 0;
                for(i32 i = 0; i < 4; i++)
                {
                    if(!maps[i].valid())
                        break; /* chicago maps are contiguous from 0 */
                    auto it = bitm_cache.find(maps[i]);
                    if(it == bitm_cache.end())
                        break;
                    BitmapItem const& b = it->second;
                    auto              bkt =
                        bitm_cache
                            .template get_bucket<gfx::compat::texture_2da_t>(
                                b.image.fmt);
                    auto sub =
                        bkt.template texture_as<gfx::compat::texture_2da_t>()
                            .subtexture(b.image.layer);
                    bkt.sampler->rebind(sub);
                    samplers.push_back(
                        gleam::sampler_definition_t{
                            typing::graphics::ShaderStage::Fragment,
                            {map_names[i], i},
                            bkt.sampler});
                    if(i == 0)
                        scale0 = b.image.scale;
                    count++;
                }
                if(count == 0)
                    continue;
                auto frg_u = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Fragment,
                    gfx::uniform_pair{
                        {"map_count"sv, 4},
                        semantic::SpanOne<const i32>(count)},
                    gfx::uniform_pair{
                        {"base_map_scale"sv, 5},
                        semantic::SpanOne<const Vecf2>(scale0)});
                api->submit(
                    {
                        .program       = resources.chicago_pipeline,
                        .vertices      = resources.model_attr,
                        .render_target = resources.offscreen,
                        .call =
                            gfx::draw_command::call_spec_t{
                                .indexed = true,
                                .mode = gfx::drawing::primitive::triangle_strip,
                            },
                        .data = sm_draw.draw.data,
                    },
                    vtx_u,
                    frg_u,
                    samplers,
                    gfx::cull_state{.front_face = true},
                    gfx::view_state{
                        .depth =
                            gfx::depth_state{
                                .range          = Vecd2{0.0, 0.0},
                                .reversed       = true,
                                .strict_greater = false,
                            },
                    },
                    gfx::depth_extended_state{.depth_write = false});
                continue;
            }

            /* soso — standard single-diffuse scenery; accumulate identical
             * instances into a batch keyed by shader + part geometry. */
            generation_idx_t base_bitm = shitem.soso.base_bitm;
            if(!base_bitm.valid())
                continue;
            auto bitm_it = bitm_cache.find(base_bitm);
            if(bitm_it == bitm_cache.end())
                continue;
            if(sm_draw.draw.data.empty())
                continue;

            auto const& geom = sm_draw.draw.data.front();
            auto& batch = soso_batches[{sm.shader.i, geom.elements.offset}];
            if(batch.transforms.empty())
            {
                batch.shader                = sm.shader;
                batch.geom                  = geom;
                batch.geom.instances.count  = 1;
                batch.geom.instances.offset = 0;
                batch.base_scale =
                    bitm_it->second.image.scale * mitem.header->uvscale;
            }
            batch.transforms.push_back(mod.transform);
        }

        /* Emit one batched draw per identical group (splitting runs longer than
         * the uniform-array cap). The program, base texture, sampler and frag
         * uniform are bound once per group; only the transform array changes.
         */
        for(auto const& [key, batch] : soso_batches)
        {
            auto shader_it = shader_cache.find(batch.shader);
            if(shader_it == shader_cache.end())
                continue;
            generation_idx_t base_bitm = shader_it->second.soso.base_bitm;
            auto             bitm_it   = bitm_cache.find(base_bitm);
            if(bitm_it == bitm_cache.end())
                continue;

            BitmapItem const& base = bitm_it->second;
            auto              base_tex =
                bitm_cache.template get_bucket<gfx::compat::texture_2da_t>(
                    base.image.fmt);
            auto base_sub =
                base_tex.template texture_as<gfx::compat::texture_2da_t>()
                    .subtexture(base.image.layer);
            base_tex.sampler->rebind(base_sub);

            auto frg_u = gfx::make_uniform_list(
                typing::graphics::ShaderStage::Fragment,
                gfx::uniform_pair{
                    {"base_map_scale"sv, 3},
                    semantic::SpanOne<const Vecf2>(batch.base_scale)});

            std::vector<gfx::sampler_definition_t> samplers;
            samplers.push_back(
                gleam::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"diffuse"sv, 0},
                    base_tex.sampler});

            for(size_t off = 0; off < batch.transforms.size();
                off += model_batch_size)
            {
                size_t n = std::min<size_t>(
                    model_batch_size, batch.transforms.size() - off);
                auto vtx_u = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair{
                        {"camera"sv, 1},
                        semantic::SpanOne<const Matf4>(camera)},
                    gfx::uniform_pair{
                        {"models"sv, 2},
                        semantic::Span<const Matf4>(
                            batch.transforms.data() + off, n)});
                std::vector<gfx::draw_command::data_t> data(n, batch.geom);
                api->submit(
                    {
                        .program       = resources.model_pipeline,
                        .vertices      = resources.model_attr,
                        .render_target = resources.offscreen,
                        .call =
                            gfx::draw_command::call_spec_t{
                                .indexed = true,
                                .mode = gfx::drawing::primitive::triangle_strip,
                            },
                        .data = data,
                    },
                    vtx_u,
                    frg_u,
                    samplers,
                    gfx::cull_state{.front_face = true},
                    gfx::view_state{
                        .depth =
                            gfx::depth_state{
                                .range          = Vecd2{0.0, 1.0},
                                .reversed       = true,
                                .strict_greater = true,
                            },
                    });
            }
        }
        Coffee::Profiler::PopContext();
    }
};

void ScreenClear::start_restricted(Proxy& e, const time_point&)
{
    auto& api = e.subsystem<gfx::system>();
    auto  _   = api.debug().scope("ScreenClear::start_restricted");
    auto  fb  = e.subsystem<gfx::system>().default_rendertarget();
    e.subsystem<BlamResources>().offscreen->clear(0.0);
}

void ScreenClear::end_restricted(Proxy& e, const time_point&)
{
    auto& api       = e.subsystem<gfx::system>();
    auto& resources = e.subsystem<BlamResources>();

    if(api.default_rendertarget() == resources.offscreen)
        return;

    auto _            = api.debug().scope("ScreenClear::end_restricted");
    auto render_timer = api.gpu_timer("ScreenClear Full screen render");

    if(!quad_program)
        load_resources(api, e.subsystem<BlamResources>());

    auto& postprocess   = e.subsystem<PostProcessParameters>();
    f32   display_scale = postprocess.scale;

    Matf4 transform = glm::scale(
        glm::translate(glm::identity<Matf4>(), Vecf3{-1, -1, 0}),
        Vecf3{2, 2, 1});
    transform = glm::scale(transform, Vecf3{display_scale, display_scale, 1.f});
    Vecf2 uvscale{1.f, 1.f};
    Vecf2 offset{0, 0};

    std::shared_ptr<gfx::sampler_t> blur_source = offscreen_sampler;
    Vecf2                           blur_spacing{0.f, 0.f};

    if(postprocess.blur > 0.00001f && offscreen_sampler && blur_chain[0])
    {
        /* blur is a fraction of screen height; the kernel in the composite
         * reaches ~3 texels of whichever level the chain stops at */
        Veci2 level_size = resources.offscreen_size;
        f32   radius     = postprocess.blur * static_cast<f32>(level_size.y);

        std::weak_ptr<gfx::texture_t> source = offscreen_sampler->m_source;
        std::size_t                   level  = 0;

        do
        {
            level_size.x = std::max(level_size.x / 2, 1);
            level_size.y = std::max(level_size.y / 2, 1);
            api.perform_downscale(
                source,
                blur_chain[level],
                size_2d<u32>{
                    static_cast<u32>(level_size.x),
                    static_cast<u32>(level_size.y)});
            source = blur_chain[level];
            level++;
        } while(level < blur_chain.size() &&
                radius > 3.f * static_cast<f32>(1u << level));

        blur_source  = blur_chain_samplers[level - 1];
        blur_spacing = Vecf2{
            1.f / static_cast<f32>(level_size.x),
            1.f / static_cast<f32>(level_size.y)};
    }

    int effect_mode = postprocess.rgb_comp.length() > 0 ? 2
                      : postprocess.blur > 0            ? 1
                                                        : 0;

    auto params_v = gfx::make_uniform_list(
        typing::graphics::ShaderStage::Vertex,
        gfx::uniform_pair{{"transform"sv}, semantic::SpanOne(transform)});
    auto params_f = gfx::make_uniform_list(
        typing::graphics::ShaderStage::Fragment,
        gfx::uniform_pair{{"gamma"sv}, semantic::SpanOne(postprocess.gamma)},
        gfx::uniform_pair{{"scale"sv}, semantic::SpanOne(uvscale)},
        gfx::uniform_pair{{"offset"sv}, semantic::SpanOne(offset)},
        gfx::uniform_pair{
            {"exposure"sv}, semantic::SpanOne(postprocess.exposure)},
        gfx::uniform_pair{{"blur_distance"}, semantic::SpanOne(blur_spacing)},
        gfx::uniform_pair{
            {"rgb_comp_defocus"}, semantic::SpanOne(postprocess.rgb_comp)},
        gfx::uniform_pair{{"mode"}, semantic::SpanOne(effect_mode)});

    // clang-format off
    auto composite_result = api.submit(gfx::draw_command{
                    .program = quad_program,
                    .vertices = quad_vao,
                    .call = {
                         .indexed = false,
                         .mode = gfx::drawing::primitive::triangle_fan,
                    },
                    .data = {{ .arrays = { .count = 4, }}},
                },
                gfx::make_sampler_list(gfx::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"source"sv},
                    blur_source,
                }),
                params_v,
                std::move(params_f));
    // clang-format on
    if(composite_result.has_value())
        cWarning(
            "ScreenClear composite failed: {}", std::get<1>(*composite_result));

    comp_app::interfaces::GraphicsFramebuffer* framebuffer =
        e.service<comp_app::GraphicsFramebuffer>();

    Vecf2 item_scale{2.f / framebuffer->size().w, 2.f / framebuffer->size().h};
    f32   one = 1.f;
    Vecf2 no_blur{0.f, 0.f};
    effect_mode = 0;

    params_f = gfx::make_uniform_list(
        typing::graphics::ShaderStage::Fragment,
        gfx::uniform_pair{{"gamma"sv}, semantic::SpanOne(one)},
        gfx::uniform_pair{{"scale"sv}, semantic::SpanOne(uvscale)},
        gfx::uniform_pair{{"offset"sv}, semantic::SpanOne(offset)},
        gfx::uniform_pair{{"exposure"sv}, semantic::SpanOne(one)},
        gfx::uniform_pair{{"blur_distance"}, semantic::SpanOne(no_blur)},
        gfx::uniform_pair{
            {"rgb_comp_defocus"}, semantic::SpanOne(postprocess.rgb_comp)},
        gfx::uniform_pair{{"mode"}, semantic::SpanOne(effect_mode)});

    for(screen_quad_t const& draw : extra_quads)
    {
        if(draw.sampler.expired())
            continue;

        Vecf3 translation(Vecf2(draw.position) * item_scale - 1.f, 0.f);
        Vecf3 scale(
            draw.size.x * item_scale.x, draw.size.y * item_scale.y, 1.f);

        uvscale = draw.atlas_scale;
        offset  = draw.atlas_offset;

        transform = glm::scale(
            glm::translate(glm::identity<Matf4>(), translation), scale);
        // clang-format off
        api.submit(gfx::draw_command{
                    .program = quad_program,
                    .vertices = quad_vao,
                    .call = {
                         .indexed = false,
                         .mode = gfx::drawing::primitive::triangle_fan,
                    },
                    .data = {{ .arrays = { .count = 4 }}},
                },
                gfx::make_sampler_list(gfx::sampler_definition_t{
                    typing::graphics::ShaderStage::Fragment,
                    {"source"sv},
                    draw.sampler.lock()
                }),
                params_v,
                params_f,
                gfx::blend_state{});
        // clang-format on
    }

    extra_quads.clear();
}

void ScreenClear::load_resources(gleam::system& api, BlamResources& resources)
{
    using vecb4 = typing::vectors::tvector<libc_types::i8, 4>;
    using vecb2 = typing::vectors::tvector<libc_types::i8, 2>;

    struct vertex_t
    {
        vecb2 pos;
        vecb2 tex;
    };

    quad_vbo = api.alloc_buffer(gfx::buffers::vertex, RSCA::ReadOnly);
    quad_vbo->alloc();
    std::array<vecb4, 6> vbo = {{
        vecb4{0, 0, 0, 0},
        vecb4{127, 0, 127, 0},
        vecb4{127, 127, 127, 127},
        vecb4{0, 127, 0, 127},

    }};
    quad_vbo->commit(vbo);
    quad_vao = api.alloc_vertex_array();
    quad_vao->alloc();
    quad_vao->add(
        gfx::vertex_attribute::from_member(
            &vertex_t::pos, gfx::vertex_float_type));
    auto tex = gfx::vertex_attribute::from_member(
        &vertex_t::tex, gfx::vertex_float_type);
    tex.index = 1;
    quad_vao->add(tex);
    quad_vao->set_buffer(gfx::buffers::vertex, quad_vbo, 0);
    quad_vao->set_attribute_names({
        {"pos", 0},
        {"tex", 1},
    });
    quad_vao->force_attribute_names();

    if(resources.color)
    {
        for(std::size_t i = 0; i < blur_chain.size(); i++)
        {
            blur_chain[i] = api.alloc_texture(
                gfx::textures::d2, resources.color->m_format, 1);
            blur_chain[i]->alloc(size_3d<u32>{16, 16, 1});

            auto& sampler = blur_chain_samplers[i];
            sampler       = blur_chain[i]->sampler();
            sampler->alloc();
            sampler->set_filtering(
                typing::Filtering::Linear, typing::Filtering::Linear);
        }
    }

    quad_program = api.alloc_program();
    quad_program->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::screen_clear_vert)));
    quad_program->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader(
            semantic::mem_chunk<const char>::ofContainer(
                blam::loading::screen_clear_frag)));
    if(auto res = quad_program->compile(); res.has_error())
        cDebug("Error compiling quad shader: {0}", res.error());

    auto compile_blur_stage =
        [&api](std::string_view fragment, std::string_view name) {
            auto program = api.alloc_program();
            program->add(
                gfx::program_t::stage_t::Vertex,
                api.alloc_shader(
                    semantic::mem_chunk<const char>::ofContainer(
                        blam::loading::screen_clear_vert)));
            program->add(
                gfx::program_t::stage_t::Fragment,
                api.alloc_shader(
                    semantic::mem_chunk<const char>::ofContainer(fragment)));
            if(auto res = program->compile(); res.has_error())
                cDebug("Error compiling {0} shader: {1}", name, res.error());
            return program;
        };

    blur_down_program =
        compile_blur_stage(blam::loading::blur_down_frag, "blur downsample");
    blur_up_program =
        compile_blur_stage(blam::loading::blur_up_frag, "blur upsample");

    if(resources.color)
    {
        offscreen_sampler = resources.color->sampler();
        offscreen_sampler->alloc();
        offscreen_sampler->set_filtering(
            typing::Filtering::Linear, typing::Filtering::Linear);
        offscreen_sampler->set_edge_policy(0, typing::WrapPolicy::Clamp);
        offscreen_sampler->set_edge_policy(1, typing::WrapPolicy::Clamp);
    }
}

void alloc_renderer(EntityContainer& container)
{
    ProfContext _;
    auto&       api = container.subsystem_cast<gfx::system>();

    container.register_subsystem_inplace<ScreenClear>();
    if(api.api_version() == std::make_tuple<u32, u32>(2, 0))
    {
        container
            .register_subsystem_inplace<LegacyMeshRenderer<halo_version>>();
    } else
    {
        container.register_subsystem_inplace<DrawListBuilder<halo_version>>(
            &api,
            std::ref(container.subsystem_cast<BlamResources>()),
            std::ref(container.subsystem_cast<RenderingParameters>()),
            std::ref(container.subsystem_cast<ShaderCache<halo_version>>()),
            std::ref(container.subsystem_cast<BitmapCache<halo_version>>()));
        container.register_subsystem_inplace<MeshRenderer<halo_version>>(
            &api,
            std::ref(container.subsystem_cast<BlamResources>()),
            std::ref(container.subsystem_cast<RenderingParameters>()),
            std::ref(container.subsystem_cast<ShaderCache<halo_version>>()),
            std::ref(container.subsystem_cast<BitmapCache<halo_version>>()),
            std::ref(container.subsystem_cast<BSPCache<halo_version>>()));
#ifndef DISABLE_LOADING_SCREEN
        container.register_subsystem_inplace<LoadingScreen>();
#endif
    }
}
