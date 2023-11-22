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
using Coffee::cWarning;
using Coffee::cFatal;
using Coffee::ProfContext;

using libc_types::f32;
using libc_types::i16;
using libc_types::i32;
using libc_types::u16;
using libc_types::u32;
using semantic::Span;
using stl_types::range;
using typing::PixCmp;
using typing::geometry::size_2d;
using typing::geometry::size_3d;
using typing::pixels::BitFmt;
using typing::pixels::CompFlags;
using typing::pixels::PixDesc;
using typing::pixels::PixFmt;
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

using bitm_format_hash
    = std::tuple<blam::bitm::type_t, PixFmt, PixCmp, BitFmt, CompFlags>;

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
