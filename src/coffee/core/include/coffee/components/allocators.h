#pragma once

#include <coffee/components/types.h>

#include <limits>
#include <vector>

namespace compo::alloc {

/*!
 * \brief Sparse-set component storage.
 *
 * m_data / m_dense are parallel dense arrays (payload / entity id),
 * m_sparse maps entity id -> dense index (ids are sequential, handed out
 * by EntityContainer's counter, so a flat vector works as the index).
 *
 * contains/get are O(1) array lookups; iteration over all instances of a
 * component walks m_dense linearly. Removal swap-erases: the last element
 * is moved into the removed slot, so pointers from get() are only valid
 * until the next mutation of the same container (they already were only
 * that stable before, since registration could reallocate the vector).
 */
template<
    typename ComponentType,
    typename AllocationType = typename ComponentType::value_type>
struct VectorBaseContainer : ComponentContainer<ComponentType>
{
    using typename ComponentContainer<ComponentType>::type;

    using vector_type = std::vector<AllocationType>;
    using value_type  = AllocationType;

    static constexpr u32 sparse_npos = std::numeric_limits<u32>::max();

    vector_type      m_data;   /*!< dense payload */
    std::vector<u64> m_dense;  /*!< dense entity ids, parallel to m_data */
    std::vector<u32> m_sparse; /*!< entity id -> dense index */

    virtual void register_entity(u64 id) override
    {
        if(contains_entity(id))
            return;
        if(m_sparse.size() <= id)
            m_sparse.resize(id + 1, sparse_npos);
        m_sparse[id] = static_cast<u32>(m_dense.size());
        m_dense.push_back(id);
        m_data.push_back(typename vector_type::value_type());
    }

    virtual void unregister_entity(u64 id) override
    {
        if(!contains_entity(id))
            return;
        const u32 idx  = m_sparse[id];
        const u32 last = static_cast<u32>(m_dense.size() - 1);
        if(idx != last)
        {
            m_data[idx]            = std::move(m_data[last]);
            m_dense[idx]           = m_dense[last];
            m_sparse[m_dense[idx]] = idx;
        }
        m_data.pop_back();
        m_dense.pop_back();
        m_sparse[id] = sparse_npos;
    }

    virtual void prealloc(szptr count) override
    {
        m_data.reserve(m_data.size() + count);
        m_dense.reserve(m_dense.size() + count);
    }

    /* final: lets the per-entity membership test in component_query
     * devirtualize + inline */
    virtual bool contains_entity(u64 id) const final
    {
        return id < m_sparse.size() && m_sparse[id] != sparse_npos;
    }

    virtual std::vector<u64> const* dense_ids() const final
    {
        return &m_dense;
    }

    virtual vector_type& get_storage()
    {
        return m_data;
    }

    virtual vector_type const& get_storage() const
    {
        return m_data;
    }
};

template<
    typename ComponentType,
    typename AllocationType = typename ComponentType::value_type>
struct VectorContainer : VectorBaseContainer<ComponentType, AllocationType>
{
    using container_type = VectorContainer<ComponentType, AllocationType>;
    using type           = container_type;

    static constexpr size_t frame_count_v = 1;

    virtual typename ComponentType::value_type* get(u64 id)
    {
        if(!this->contains_entity(id))
            return nullptr;

        return &this->m_data[this->m_sparse[id]];
    }

    /*!
     * \brief Payload for a dense index, ignoring the frame offset.
     *
     * Single-buffered, so every offset is the same storage. Queries call
     * this instead of reaching into m_data so buffered and unbuffered
     * containers present the same shape.
     */
    typename ComponentType::value_type* resolve(u64 id, size_t /*offset*/)
    {
        return &this->m_data[this->m_sparse[id]];
    }
};

/*!
 * \brief Sparse-set storage keeping Frames copies of the payload.
 */
template<
    typename ComponentType,
    size_t   Frames         = 2,
    typename AllocationType = typename ComponentType::value_type>
struct BufferedContainer : VectorBaseContainer<ComponentType, AllocationType>
{
    using base = VectorBaseContainer<ComponentType, AllocationType>;
    using container_type =
        BufferedContainer<ComponentType, Frames, AllocationType>;
    using type        = container_type;
    using vector_type = typename base::vector_type;

    static constexpr size_t frame_count_v = Frames;

    static_assert(Frames >= 2, "a buffered component needs at least 2 frames");
    static_assert(
        std::is_copy_assignable_v<AllocationType>,
        "buffered components must be self-contained values: buffers are "
        "copied forward each frame, so a payload holding unique_ptr/shared_ptr "
        "would alias shared state instead of being replicated");

    /* base::m_data is buffer 0; the remainder live here */
    std::array<vector_type, Frames - 1> m_extra;
    size_t                              m_read{0};

    vector_type& buffer(size_t index)
    {
        index %= Frames;
        return index == 0 ? this->m_data : m_extra[index - 1];
    }

    size_t frame_index(size_t offset) const
    {
        return (m_read + offset) % Frames;
    }

    virtual void register_entity(u64 id) override
    {
        if(this->contains_entity(id))
            return;
        base::register_entity(id);
        /* every buffer stays parallel to m_dense, including for entities
         * created mid-frame — a reader of the current frame would otherwise
         * index past the end */
        for(auto& extra : m_extra)
            extra.push_back(AllocationType());
    }

    virtual void unregister_entity(u64 id) override
    {
        if(!this->contains_entity(id))
            return;
        const u32 idx  = this->m_sparse[id];
        const u32 last = static_cast<u32>(this->m_dense.size() - 1);
        for(auto& extra : m_extra)
        {
            if(idx != last)
                extra[idx] = std::move(extra[last]);
            extra.pop_back();
        }
        base::unregister_entity(id);
    }

    virtual void prealloc(szptr count) override
    {
        base::prealloc(count);
        for(auto& extra : m_extra)
            extra.reserve(extra.size() + count);
    }

    virtual size_t frame_count() const override
    {
        return Frames;
    }

    virtual void advance_frame() override
    {
        m_read = frame_index(1);
        /* the buffer about to be written holds frame N-1; carry the current
         * values into it so partial writers do not resurrect stale data */
        buffer(frame_index(1)) = buffer(m_read);
    }

    virtual typename ComponentType::value_type* get(u64 id) override
    {
        if(!this->contains_entity(id))
            return nullptr;

        return &buffer(m_read)[this->m_sparse[id]];
    }

    virtual typename ComponentType::value_type* get_at(u64 id, size_t offset) override
    {
        if(!this->contains_entity(id))
            return nullptr;

        return resolve(id, offset);
    }

    typename ComponentType::value_type* resolve(u64 id, size_t offset)
    {
        return &buffer(frame_index(offset))[this->m_sparse[id]];
    }
};

} // namespace compo::alloc
