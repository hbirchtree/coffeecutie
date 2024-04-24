#pragma once

#include "rhi.h"
#include "rhi_texture.h"

namespace gleam {

template<typename Tex, typename Allocator>
struct texture_atlas_t
{
    using format_desc_t =
        std::pair<typing::pixels::PixFmt, typing::pixels::CompFlags>;

    struct atlas_reference_t
    {
        format_desc_t               format;
        typing::vector_types::Vecf2 offset{0, 0};
        typing::vector_types::Vecf2 scale{1, 1};
        u32                         layer{0};
        u32                         mipmaps;
        bool                        resident;
    };

  private:
    struct tex_storage_t
    {
        format_desc_t                      format{};
        std::vector<gsl::span<const char>> mipmaps;
        Veci2                              l0_size;
    };

  public:
    texture_atlas_t(Allocator& allocator, u32 max_mipmaps = 1)
        : m_allocator(allocator)
        , m_features(allocator.feature_info())
        , m_max_mipmaps(max_mipmaps)
    {
    }

    template<class T, class SizeT>
    u32 add(
        PixDesc const&                       format,
        std::vector<gsl::span<const char>>&& data,
        SizeT const&                         size)
    {
        const auto fmt    = std::make_pair(format.pixfmt, format.c.c_flags);
        auto       tex_it = m_textures.find(fmt);
        if(tex_it == m_textures.end())
        {
            tex_it = m_textures
                         .emplace(
                             fmt,
                             m_allocator.alloc_texture(
                                 textures::d2_array, format, m_max_mipmaps))
                         .first;
        }
        atlas_reference_t ref = {
            .format   = fmt,
            .mipmaps  = data.size(),
            .resident = false,
        };
        const auto it = m_references.emplace(++m_id, ref).first;
        m_non_resident.emplace(
            it.first,
            tex_storage_t{
                .format  = fmt,
                .mipmaps = std::move(data),
                .l0_size = size,
            });
        return it;
    }

    void build()
    {
        // If we don't have 2D array support, we don't need to build anything
        if(!m_features.texture.texture_3d)
            return;
        std::map<format_desc_t, std::vector<tex_storage_t*>> pending;
        for(auto& [id, storage] : m_non_resident)
            pending[storage.format].push_back(&storage);
        for(auto& [fmt, tex] : m_textures)
        {
            auto const& textures = pending[fmt];
            Veci2       max_size{0, 0};
            // Determine max single texture size
            for(auto const& texture : textures)
                max_size = glm::max(max_size, texture.l0_size);

            // Stack the textures in the layers to reduce amount of layers for
            // small textures
            u32 num_layers       = 0;
            u32 horizontal_shift = 0;

            std::vector<std::pair<u32, u32>> layer_offsets;
            for(auto const& texture : textures)
            {
                if(horizontal_shift + texture.l0_size.x > max_size.x)
                {
                    num_layers++;
                    horizontal_shift = 0;
                }
                layer_offsets.push_back({num_layers, horizontal_shift});
                horizontal_shift += texture.l0_size.x;
            }
            tex.alloc(size_3d<u32>{
                .w = static_cast<u32>(max_size.x),
                .h = static_cast<u32>(max_size.y),
                .d = num_layers,
            });
            u32 i = 0;
            for(auto const& texture : textures)
            {
                auto [layer, offset] = layer_offsets[i++];
                auto size            = texture.l0_size;
                u32  mip_level       = 0;
                for(auto const& mip : texture.mipmaps)
                {
                    tex.upload(mip, Veci2{offset, 0}, size, mip_level++);
                    offset >>= 2;
                    size >>= 2;
                }
            }
        }
    }

    atlas_reference_t reference_of(u32 id)
    {
        return m_references.at(id);
    }

    void clear()
    {
        for(auto& [fmt, tex] : m_textures)
            tex.dealloc();

        m_id = 0;
        m_textures.clear();
        m_non_resident.clear();
    }

  private:
    std::map<format_desc_t, std::shared_ptr<Tex>> m_textures;
    std::map<u32, atlas_reference_t>              m_references;

    api&            m_allocator;
    features const& m_features;
    u32             m_max_mipmaps;
    u32             m_id{0};

    /*! Support for non-resident textures to be uploaded in FIFO manner
     *  TODO: Needs a policy for making them non-resident
     */
    std::map<u32, tex_storage_t> m_non_resident;
};

} // namespace gleam
