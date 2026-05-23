#pragma once

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_bsp_structures.h>
#include <blam/volta/blam_font.h>
#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_ui.h>

#include "data_cache.h"
#include "graphics_api.h"

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

using bitm_format_hash = std::
    tuple<blam::bitm::type_t, pix_fmt, pix_components, bit_fmt, comp_flags>;

struct BSPItem
{
    struct Mesh
    {
        blam::bsp::material const*  mesh{nullptr};
        gleam::draw_command::data_t draw;
        generation_idx_t            light_bitm;
        generation_idx_t            shader;
        u32                         cluster_idx{std::numeric_limits<u32>::max()};
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

    struct Cluster
    {
        blam::bsp::cluster const*                     cluster{nullptr};
        std::vector<Subcluster>                       sub{};
        std::vector<blam::bsp::cluster_portal const*> portals{};
    };

    blam::bsp::header const*                 mesh{nullptr};
    blam::tag_t const*                       tag{nullptr};
    std::vector<Group>                       groups;
    std::vector<Cluster>                     clusters;
    std::vector<gleam::draw_command::data_t> portals;
    std::vector<u32>                         portal_color_ptrs;

    /* PVS (Potentially Visible Set) data: one bit per cluster per row,
     * row i says which clusters are visible from cluster i */
    Span<libc_types::byte_t const> pvs_data;
    u32                            pvs_row_stride{0};

    inline bool valid() const
    {
        return mesh;
    }

    /* BFS through the portal graph from from_idx up to max_depth hops.
     * Returns a bitset (one bool per cluster) of reachable clusters.
     * max_depth=1 gives the camera cluster + its immediate portal neighbours. */
    inline std::vector<bool> portal_visible_set(
        u32 from_idx, u32 max_depth = std::numeric_limits<u32>::max()) const
    {
        std::vector<bool> visible(clusters.size(), false);
        if(from_idx >= clusters.size())
            return visible;

        std::vector<u32> frontier = {from_idx};
        visible[from_idx]        = true;

        for(u32 depth = 0; depth < max_depth && !frontier.empty(); depth++)
        {
            std::vector<u32> next;
            for(u32 ci : frontier)
            {
                for(auto const* portal : clusters[ci].portals)
                {
                    i32 adj = (portal->front_cluster == static_cast<i16>(ci))
                                  ? portal->back_cluster
                                  : portal->front_cluster;
                    if(adj >= 0 && static_cast<u32>(adj) < clusters.size()
                       && !visible[static_cast<u32>(adj)])
                    {
                        visible[static_cast<u32>(adj)] = true;
                        next.push_back(static_cast<u32>(adj));
                    }
                }
            }
            frontier = std::move(next);
        }
        return visible;
    }

    /* Returns true if cluster to_idx is visible from cluster from_idx
     * according to the PVS. Falls back to true if no PVS is available. */
    inline bool cluster_visible_from(u32 from_idx, u32 to_idx) const
    {
        if(pvs_data.empty() || pvs_row_stride == 0 || clusters.empty())
            return true;
        if(from_idx >= clusters.size() || to_idx >= clusters.size())
            return true;
        auto const* row = pvs_data.data() + from_idx * pvs_row_stride;
        return (row[to_idx / 8] >> (to_idx % 8)) & 1;
    }

    inline std::optional<std::pair<u32, u32>> find_cluster(
        Vecf3 const& point) const
    {
        /* Pick the smallest-volume subcluster AABB that contains the point.
         * BSP subclusters can overlap; the most specific (smallest) one is
         * most likely to give the correct cluster assignment. */
        std::optional<std::pair<u32, u32>> best;
        float                              best_vol = std::numeric_limits<float>::max();

        u32 cluster_id = 0;
        for(auto const& cluster : clusters)
        {
            u32 sub_id = 0;
            for(auto const& sub : cluster.sub)
            {
                if(sub.cluster->bounds.contains(point))
                {
                    auto [bmin, bmax] = sub.cluster->bounds.points();
                    auto  diag        = glm::abs(bmax - bmin);
                    float vol         = diag.x * diag.y * diag.z;
                    if(vol < best_vol)
                    {
                        best_vol = vol;
                        best     = std::pair{cluster_id, sub_id};
                    }
                }
                sub_id++;
            }
            cluster_id++;
        }
        return best;
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

    struct sotr_t
    {
        std::array<generation_idx_t, 4> maps;
        std::array<generation_idx_t, 4> layers;
    };

    union
    {
        generation_idx_t color_bitm;

        scex_t scex;
        schi_t schi;
        senv_t senv;
        soso_t soso;
        sotr_t sotr;
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
