#pragma once

#include <blam/dimeter/map.h>
#include <coffee/components/subsystem.h>

#include <memory>
#include <optional>
#include <variant>

namespace h2 {

/*! The loaded Halo 2 map, kept whole for as long as it is the world: what
 * gameplay looks tags up in */
struct H2Map : compo::SubsystemBase
{
    using type  = H2Map;
    using xbox  = blam::dimeter::map_container<blam::dimeter::xbox_version_t>;
    using vista = blam::dimeter::map_container<blam::dimeter::vista_version_t>;

    template<typename V>
    void hold(
        blam::dimeter::map_container<V>                   map,
        std::shared_ptr<void const>                        file,
        std::optional<blam::dimeter::map_container<V>>&& shared = std::nullopt)
    {
        map.keep = std::move(file);
        if constexpr(std::is_same_v<V, blam::dimeter::vista_version_t>)
        {
            m_shared = std::move(shared);
            map.shared = m_shared ? &*m_shared : nullptr;
        }
        m_map = std::move(map);
    }

    void release()
    {
        m_map = std::monostate{};
        m_shared.reset();
    }

    bool loaded() const
    {
        return !std::holds_alternative<std::monostate>(m_map);
    }

    /*! f(map) with the map as its version's container, if one is loaded */
    template<typename F>
    void visit(F&& f) const
    {
        if(auto const* map = std::get_if<xbox>(&m_map))
            f(*map);
        else if(auto const* map = std::get_if<vista>(&m_map))
            f(*map);
    }

  private:
    std::variant<std::monostate, xbox, vista> m_map;
    std::optional<vista>                      m_shared;
};

} // namespace h2
