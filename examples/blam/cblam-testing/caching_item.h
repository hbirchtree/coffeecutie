#pragma once

#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_bsp_structures.h>
#include <blam/volta/blam_font.h>
#include <blam/volta/blam_mod2.h>
#include <blam/common/frustum.h>
#include <blam/common/portal_walk.h>
#include <blam/volta/blam_ui.h>

#include "data_cache.h"
#include "graphics_api_fwd.h"

#include <coffee/core/debug/formatting.h>

using Coffee::cDebug;
using Coffee::cFatal;
using Coffee::cWarning;
using Coffee::ProfContext;

using libc_types::f32;
using libc_types::i16;
using libc_types::i32;
using libc_types::u16;
using libc_types::u32;
using libc_types::u64;
using semantic::Span;
using stl_types::range;
using typing::pix_components;
using typing::geometry::size_2d;
using typing::geometry::size_3d;
using typing::pixels::bit_fmt;
using typing::pixels::comp_flags;
using typing::pixels::pix_fmt;
using typing::pixels::PixDesc;
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

using bitm_format_hash = std::tuple<
    blam::bitm::type_t,
    pix_fmt,
    pix_components,
    bit_fmt,
    comp_flags,
    typing::pixels::pix_flags>;

using blam::Frustum;

struct BSPItem
{
    struct Mesh
    {
        blam::bsp::material const* mesh{nullptr};
        gfx::draw_command::data_t  draw;
        generation_idx_t           light_bitm;
        generation_idx_t           shader;
        u32 cluster_idx{std::numeric_limits<u32>::max()};
        u32 subcluster_idx{std::numeric_limits<u32>::max()};
        /* All clusters whose leaves reference this chunk's faces. Chunks made
         * of boundary-straddling faces carry every owner cluster (instead of
         * being unconditionally visible), so they can be culled when none of
         * their owners is in the PVS. Empty = no cluster info, never culled. */
        std::vector<u16> clusters{};
        /* Chunk AABB in BSP space, computed from the chunk's own vertices —
         * tighter than the subcluster AABB and available for every chunk. */
        Vecf3 bmin{};
        Vecf3 bmax{};
        bool  has_bounds{false};
    };

    struct Group
    {
        blam::bsp::lightmap const* group{nullptr};
        std::vector<Mesh>          meshes{};
    };

    struct Subcluster
    {
        blam::bsp::subcluster const* cluster{nullptr};
        Span<u32 const>              indices;
        u32                          debug_color_idx{0};
    };

    struct Portal
    {
        blam::bsp::cluster_portal const* data{nullptr};
        std::vector<Vecf3>               vertices{};

        inline i16 front_cluster() const
        {
            return data->front_cluster;
        }

        inline i16 back_cluster() const
        {
            return data->back_cluster;
        }
    };

    struct Cluster
    {
        blam::bsp::cluster const* cluster{nullptr};
        std::vector<Subcluster>   sub{};
        std::vector<Portal>       portals{};
    };

    /* Flat entry for fast position-to-cluster lookup.
     * Sorted by volume ascending so find_cluster() returns on the first hit.
     * bmin/bmax stored inline to avoid pointer chasing. Struct is 32 bytes. */
    struct FlatSubcluster
    {
        Vecf3 bmin, bmax;  /* 12+12 bytes */
        u32   cluster_idx; /* 4 bytes */
        u32   sub_idx;     /* 4 bytes */
    };

    static_assert(sizeof(FlatSubcluster) == 32);

    /* Lightmapped triangles of one material, which light the objects above
     * them: the lightmap texel and the radiosity's incident direction */
    struct LightProbe
    {
        struct triangle_t
        {
            std::array<Vecf3, 3> position;
            std::array<Vecf2, 3> uv;
            std::array<Vecf3, 3> direction; /*!< length = how directional */
        };

        generation_idx_t        light_bitm;
        Vecf3                   bmin{std::numeric_limits<f32>::max()};
        Vecf3                   bmax{std::numeric_limits<f32>::lowest()};
        std::vector<triangle_t> triangles;
    };

    struct light_sample_t
    {
        generation_idx_t light_bitm;
        Vecf2            uv;
        Vecf3            direction;
    };

    /* The lightmapped surface straight below a point */
    std::optional<light_sample_t> light_below(Vecf3 const& point) const;

    blam::bsp::header const* mesh{nullptr};
    blam::tag_t const*       tag{nullptr};
    /* BSP-section magic for resolving reflexives inside mesh on demand
     * (cluster_for_point, raycast) */
    blam::bsp_ptr bsp_magic{};
    /* Index into the scenario's structure BSP list (bsp_info order); used to
     * match against bsp_switch_trigger source/destination. */
    libc_types::i16                                         section_idx{-1};
    std::vector<Group>                                      groups;
    std::vector<Cluster>                                    clusters;
    std::vector<FlatSubcluster>                             sorted_subclusters;
    std::vector<gfx::draw_command::data_t>                  portals;
    std::vector<u32>                                        portal_color_ptrs;
    std::vector<blam::bsp::background_sound_palette const*> bg_sound_palette;
    std::vector<blam::bsp::sound_environment_palette const*> sound_env_palette;
    std::vector<LightProbe>                                  light_probes;

    /* Collision surface mesh (winged-edge) from the collision BSP;
     * consumed by the physics subsystem for triangle soup generation.
     * Point→cluster and hitscan queries resolve the collision tree on
     * demand via mesh + bsp_magic (see find_cluster_tree/raycast). */
    Span<blam::collision::surface const> coll_surfaces;
    Span<blam::collision::edge const>    coll_edges;
    Span<blam::collision::vertex const>  coll_vertices;

    inline bool valid() const
    {
        return mesh;
    }

    using portal_scratch = blam::portal_walk::scratch_t;

    /* Portal-flow visibility from a cluster, see portal_walk::visible_set */
    inline std::vector<bool> const& portal_visible_set(
        u32 from_idx,
        Vecf3 const& /*camera_pos*/,
        Matf4 const&    mvp,
        portal_scratch& scratch,
        u32             max_depth = 64) const
    {
        return blam::portal_walk::visible_set(
            clusters, from_idx, mvp, scratch, max_depth);
    }

    /* Exact point→cluster lookup, see blam::bsp::header::cluster_for_point.
     * Bounds the result against the decoded cluster list. */
    inline std::optional<u32> find_cluster_tree(Vecf3 const& point) const
    {
        if(!mesh)
            return std::nullopt;
        auto cluster = mesh->cluster_for_point(point, bsp_magic);
        if(!cluster || *cluster >= clusters.size())
            return std::nullopt;
        return cluster;
    }

    /* Native hitscan against the collision BSP, see
     * blam::collision::bsp::raycast. */
    using ray_hit = blam::collision::ray_hit;

    inline std::optional<ray_hit> raycast(
        Vecf3 const& start, Vecf3 const& end) const
    {
        if(!mesh)
            return std::nullopt;
        return mesh->raycast(start, end, bsp_magic);
    }

    inline std::optional<std::pair<u32, u32>> find_cluster(
        Vecf3 const& point) const
    {
        /* Prefer the exact BSP-tree lookup; subcluster AABBs overlap and do
         * not cover the full cluster volume, so they are only a fallback
         * (and provide the subcluster index for debug visualisation). */
        if(auto ci = find_cluster_tree(point); ci.has_value())
        {
            auto const& subs = clusters[*ci].sub;
            for(u32 si = 0; si < static_cast<u32>(subs.size()); si++)
            {
                auto [p1, p2] = subs[si].cluster->bounds.points();
                Vecf3 lo = glm::min(p1, p2), hi = glm::max(p1, p2);
                if(lo.x <= point.x && point.x <= hi.x && lo.y <= point.y &&
                   point.y <= hi.y && lo.z <= point.z && point.z <= hi.z)
                    return std::pair{*ci, si};
            }
            return std::pair{*ci, 0u};
        }

        /* sorted_subclusters is sorted by volume ascending, so the first
         * hit is the smallest-volume (most specific) containing subcluster. */
        for(auto const& fs : sorted_subclusters)
        {
            if(fs.bmin.x <= point.x && point.x <= fs.bmax.x &&
               fs.bmin.y <= point.y && point.y <= fs.bmax.y &&
               fs.bmin.z <= point.z && point.z <= fs.bmax.z)
                return std::pair{fs.cluster_idx, fs.sub_idx};
        }
        return std::nullopt;
    }
};

template<typename V>
struct ModelItem
{
    struct SubModel
    {
        blam::mod2::part const*   header{nullptr};
        gfx::draw_command::data_t draw;
        generation_idx_t          shader;
    };

    struct LOD
    {
        blam::mod2::geometry_header<V> const* header{nullptr};
        std::vector<SubModel>                 sub;
    };

    blam::mod2::header<V> const* header{nullptr};
    blam::tag_t const*           tag{nullptr};
    LOD                          mesh;

    /* Inverse bind-world matrices from the mod2 bone axes. The only bone
     * data a model keeps; posed matrices are per-instance. */
    std::vector<Matf4> inv_bind;

    inline u32 bone_count() const
    {
        return static_cast<u32>(inv_bind.size());
    }

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
        u32   array_level{0}; /*!< mip level the image occupies in its bucket */
        Veci2 pixel_offset{}; /*!< where the upload writes */
        Vecf2 offset{};       /*!< normalized, what the shader samples with */
        Vecf2 scale{};
        Veci2 gutter{}; /*!< wrap padding reserved around the tile */
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

    struct sotr_t
    {
        std::array<generation_idx_t, 4> maps;
        std::array<generation_idx_t, 4> layers;
    };

    struct smet_t
    {
        generation_idx_t map;
    };

    struct sgla_t
    {
        generation_idx_t diffuse;
        generation_idx_t reflection_cube;
        generation_idx_t bump;
    };

    struct spla_t
    {
        generation_idx_t primary_noise;
        generation_idx_t secondary_noise;
    };

    union
    {
        scex_t scex;
        schi_t schi;
        senv_t senv;
        soso_t soso;
        sotr_t sotr;
        swat_t swat;
        smet_t smet;
        sgla_t sgla;
        spla_t spla;
    };

    inline bool valid() const
    {
        return header;
    }
};
