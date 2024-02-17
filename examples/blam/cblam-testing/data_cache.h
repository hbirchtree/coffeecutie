#pragma once

#include <coffee/components/restricted_subsystem.h>
#include <coffee/components/subsystem.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/type_list.h>
#include <peripherals/stl/type_safety.h>

#include <map>
#include <stdexcept>

using cache_id_t = libc_types::u64;
using type_safety::empty_list_t;
using type_safety::type_list_t;

struct generation_idx_t
{
    cache_id_t      i{0};
    libc_types::u32 gen{0};

    inline bool valid() const
    {
        return i != 0 && gen != 0;
    }
};

struct data_cache_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

template<typename T, typename IdType, typename... IType>
struct DataCache
{
    DataCache()                 = default;
    DataCache(DataCache&&)      = default;
    DataCache(const DataCache&) = delete;

    static constexpr cache_id_t invalid_id = 0;

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
                Throw(data_cache_error("corrupt key cache"));
        }

        cache_id_t out  = ++counter;
        T          item = predict_impl(param...);

        if(!item.valid())
        {
            //            Throw(data_cache_error("prediction failed"));
            return {invalid_id, 0};
        }

        m_cache.insert({out, std::move(item)});
        m_cache_key.emplace(item_id, out);

        return {out, generation};
    }

    auto find(generation_idx_t id)
    {
        if(id.gen < generation)
            Throw(data_cache_error("stale reference"));

        return m_cache.find(id.i);
    }

    auto end()
    {
        return m_cache.end();
    }

    void evict_all()
    {
        evict_impl();

        m_cache.clear();
        m_cache_key.clear();
        counter = 0;
        generation++;
    }

    cache_id_t                   counter{0};
    std::map<cache_id_t, T>      m_cache;
    std::map<IdType, cache_id_t> m_cache_key;
    libc_types::u32              generation{1};

    virtual T      predict_impl(IType... param) = 0;
    virtual IdType get_id(IType... args)        = 0;

    virtual void evict_impl()
    {
    }
};
