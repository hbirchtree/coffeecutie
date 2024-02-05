#pragma once

#include <coffee/components/types.h>

#include <unordered_map>
#include <vector>

namespace compo::alloc {

template<
    typename ComponentType,
    typename AllocationType = typename ComponentType::value_type>
struct VectorBaseContainer : ComponentContainer<ComponentType>
{
    using typename ComponentContainer<ComponentType>::type;

    using vector_type = std::vector<AllocationType>;
    using map_type    = std::unordered_map<u64, size_t>;
    using value_type  = AllocationType;

    map_type    m_mapping;
    vector_type m_data;

    virtual void register_entity(u64 id) override
    {
        m_mapping.insert({id, m_data.size()});
        m_data.push_back(typename vector_type::value_type());
    }
    virtual void unregister_entity(u64 id) override
    {
        m_mapping.erase(id);
    }
    virtual void prealloc(szptr count) override
    {
        m_data.reserve(m_data.size() + count);
    }
    virtual bool contains_entity(u64 id) const override
    {
        return m_mapping.find(id) != m_mapping.end();
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
        auto it = this->m_mapping.find(id);

        if(it == this->m_mapping.end())
            return nullptr;

        return &this->m_data.at((*it).second);
    }
};

} // namespace alloc
