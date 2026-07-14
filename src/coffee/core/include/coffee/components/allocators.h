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

    virtual typename ComponentType::value_type* get(u64 id)
    {
        if(!this->contains_entity(id))
            return nullptr;

        return &this->m_data[this->m_sparse[id]];
    }
};

} // namespace compo::alloc
