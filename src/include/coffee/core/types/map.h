#pragma once

#include "tdef/stltypes.h"

namespace Coffee {

template<typename KeyType, typename ValueType>
struct _cbasic_pair
{
    KeyType   key;
    ValueType value;
};

template<typename KeyType, typename ValueType, size_t Size>
using _cbasic_static_map = _cbasic_pair<KeyType, ValueType>[Size];

template<typename KeyType, typename ValueType>
class _cbasic_map
{
  public:
    /*!
     * \brief Empty constructor
     */
    _cbasic_map()
    {
    }
    /*!
     * \brief Copy contents of otherMap into this map
     * \param otherMap
     */
    _cbasic_map(const _cbasic_pair<KeyType, ValueType>* otherMap)
    {
        (void)otherMap;
    }
    /*!
     * \brief Reserve st amount of elements
     * \param st
     */
    _cbasic_map(size_t st)
    {
        (void)st;
    }
};

template<typename KeyType, typename ValueType, size_t Size>
ValueType get_flags(
    const KeyType& key, const _cbasic_static_map<KeyType, ValueType, Size>& map)
{
    ValueType val = ValueType();

    for(size_t i = 0; i < Size; i++)
        if(feval(map[i].key & key))
            val = val | map[i].value;

    return val;
}
template<typename KeyType, typename ValueType, size_t Size>
ValueType get_value(
    const KeyType& key, const _cbasic_static_map<KeyType, ValueType, Size>& map)
{
    for(size_t i = 0; i < Size; i++)
        if(map[i].key == key)
            return map[i].value;

    return ValueType();
}

template<typename KeyType, typename ValueType, size_t Size>
class StaticMap
{
    _cbasic_static_map<KeyType, ValueType, Size> m_map;

  public:
    C_DELETE_COPY_CONSTRUCTOR(StaticMap);

    StaticMap(_cbasic_pair<KeyType, ValueType> map[Size])
    {
        (void)map;
    }

    size_t size()
    {
        return Size;
    }

    template<typename IKeyType, typename IValueType, size_t ISize>
    class static_map_iterator : public Iterator<ForwardIteratorTag, KeyType>
    {
    };

    using iterator = static_map_iterator<KeyType, ValueType, Size>;
};

} // namespace Coffee
