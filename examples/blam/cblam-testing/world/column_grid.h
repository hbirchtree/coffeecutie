#pragma once

#include "../components.h"

#include <array>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace world {

/* Triangles binned into vertical columns on a coarse xy grid, for finding
 * what lies above or below a point */
template<typename T>
struct ColumnGrid
{
    static constexpr f32 cell = 4.f;

    std::vector<T>                            items;
    std::unordered_map<u64, std::vector<u32>> cells;

    /* Adds `item` to every column its `corners` span */
    void add(T item, std::array<Vecf3, 3> const& corners)
    {
        Vecf2 const lo    = glm::min(glm::min(Vecf2(corners[0]), Vecf2(corners[1])), Vecf2(corners[2]));
        Vecf2 const hi    = glm::max(glm::max(Vecf2(corners[0]), Vecf2(corners[1])), Vecf2(corners[2]));
        auto const  index = static_cast<u32>(items.size());
        items.push_back(std::move(item));
        for(i32 x = column(lo.x); x <= column(hi.x); x++)
            for(i32 y = column(lo.y); y <= column(hi.y); y++)
                cells[key(x, y)].push_back(index);
    }

    /* Calls `fn` with every item in the column holding `at` */
    template<typename Fn>
    void for_each_at(Vecf3 const& at, Fn&& fn) const
    {
        auto it = cells.find(key(column(at.x), column(at.y)));
        if(it == cells.end())
            return;
        for(u32 index : it->second)
            fn(items[index]);
    }

    void clear()
    {
        items.clear();
        cells.clear();
    }

  private:
    static i32 column(f32 v)
    {
        return static_cast<i32>(std::floor(v / cell));
    }
    static u64 key(i32 x, i32 y)
    {
        return (static_cast<u64>(static_cast<u32>(x)) << 32) | static_cast<u32>(y);
    }
};

} // namespace world
