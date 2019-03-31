#pragma once

#include <coffee/components/types.h>

namespace Coffee {
namespace Components {
namespace Allocators {

template<
    typename ComponentType,
    typename AllocationType = typename ComponentType::type>
struct VectorBaseContainer : ComponentContainer<ComponentType>
{
    using typename ComponentContainer<ComponentType>::type;

    using vector_type = Vector<AllocationType>;
    using map_type    = Map<u64, size_t>;

    map_type    m_mapping;
    vector_type m_data;

    virtual void register_entity(u64 id) override;
    virtual void unregister_entity(u64 id) override;
    virtual void prealloc(szptr count) override;
    virtual bool contains_entity(u64 id) const override;

    virtual vector_type&       get_storage();
    virtual vector_type const& get_storage() const;
};

template<typename ComponentType, typename AllocationType>
inline void VectorBaseContainer<ComponentType, AllocationType>::register_entity(
    u64 id)
{
    m_mapping.insert({id, m_data.size()});
    m_data.push_back(typename vector_type::value_type());
}

template<typename ComponentType, typename AllocationType>
inline void VectorBaseContainer<ComponentType, AllocationType>::
    unregister_entity(u64 id)
{
    m_mapping.erase(id);
}

template<typename ComponentType, typename AllocationType>
inline void VectorBaseContainer<ComponentType, AllocationType>::prealloc(
    szptr count)
{
    m_data.reserve(m_data.size() + count);
}

template<typename ComponentType, typename AllocationType>
inline bool VectorBaseContainer<ComponentType, AllocationType>::contains_entity(
    u64 id) const
{
    return m_mapping.find(id) != m_mapping.end();
}

template<typename ComponentType, typename AllocationType>
typename VectorBaseContainer<ComponentType, AllocationType>::vector_type&
VectorBaseContainer<ComponentType, AllocationType>::get_storage()
{
    return m_data;
}

template<typename ComponentType, typename AllocationType>
const typename VectorBaseContainer<ComponentType, AllocationType>::vector_type&
VectorBaseContainer<ComponentType, AllocationType>::get_storage() const
{
    return m_data;
}

template<
    typename ComponentType,
    typename AllocationType = typename ComponentType::type>
struct VectorContainer : VectorBaseContainer<ComponentType, AllocationType>
{
    virtual typename ComponentType::type* get(u64 id);
};

template<typename ComponentType, typename AllocationType>
inline typename ComponentType::type* VectorContainer<
    ComponentType,
    AllocationType>::get(u64 id)
{
    auto it = this->m_mapping.find(id);

    if(it == this->m_mapping.end())
        return nullptr;

    return &this->m_data.at((*it).second);
}

} // namespace Allocators
} // namespace Components
} // namespace Coffee
