/*
 * Cache-kind detection + common loader front-end.
 *
 * Looks at a cache directory and decides, in broad strokes, what it holds:
 *   - rs2       — dat-era RS2, numbered index dirs (0/2.dat …), ~b234-377
 *   - rs2_beta  — RS2 beta named-archive layout ("config", "models", maps/),
 *                 ~b194-234
 *   - rsc       — RuneScape Classic (JAG archives, possibly scrambled
 *                 filenames, identified by config string.dat)
 *   - js5       — post-2006 js5 caches (dirs 0-15 + 255) — NOT supported
 *
 * rs::AnyLoader wraps the matching RegionLoader behind one interface so
 * callers (CLI, engine streaming) stay parser-agnostic. Both loaders share
 * the rs2 mesh types (Vertex/Mesh/TriClass/RegionGeometry), so everything
 * downstream — repack, rendering, physics — is identical either way.
 */
#pragma once

#include "rs2_geometry.h"
#include "rsc_geometry.h"

#include <variant>

namespace rs {

enum class CacheKind
{
    unknown,
    rs2,      // numbered index dirs (b234-377)
    rs2_beta, // named archives + maps/ dir (b194-234)
    rsc,      // RuneScape Classic
    js5,      // 2006+ format — unsupported
};

inline const char* to_string(CacheKind k)
{
    switch(k)
    {
    case CacheKind::rs2: return "rs2";
    case CacheKind::rs2_beta: return "rs2-beta";
    case CacheKind::rsc: return "rsc";
    case CacheKind::js5: return "js5 (unsupported)";
    default: return "unknown";
    }
}

inline CacheKind detect_cache(const std::string& dir)
{
    namespace fs = std::filesystem;

    if(fs::exists(dir + "/0/2.dat"))
        return CacheKind::rs2;
    if(fs::is_directory(dir + "/255")) // js5 master index
        return CacheKind::js5;

    // named "config" archive: RS2 beta has loc.dat inside, RSC has
    // string.dat
    if(fs::exists(dir + "/config"))
    {
        try
        {
            auto body =
                rs2::jag_load_archive(rs2::read_file(dir + "/config"));
            if(rs2::jag_extract(body, "loc.dat"))
                return CacheKind::rs2_beta;
            if(rs2::jag_extract(body, "string.dat"))
                return CacheKind::rsc;
        }
        catch(const std::exception&) {}
    }

    // scrambled filenames: probe every file for a JAG archive holding
    // string.dat (RSC config)
    std::error_code ec;
    for(const auto& entry : fs::directory_iterator(dir, ec))
    {
        if(!entry.is_regular_file())
            continue;
        try
        {
            auto body = rs2::jag_load_archive(
                rs2::read_file(entry.path().string()));
            if(rs2::jag_extract(body, "string.dat"))
                return CacheKind::rsc;
            if(rs2::jag_extract(body, "loc.dat"))
                return CacheKind::rs2_beta;
        }
        catch(const std::exception&) {}
    }
    return CacheKind::unknown;
}

// One loader, any supported cache. The shared surface mirrors
// rs2::RegionLoader; kind-specific extras stay reachable through rs2()/rsc().
class AnyLoader
{
  public:
    explicit AnyLoader(const std::string& dir) : m_kind(detect_cache(dir))
    {
        switch(m_kind)
        {
        case CacheKind::rs2:
        case CacheKind::rs2_beta:
            m_impl.emplace<rs2::RegionLoader>(dir);
            break;
        case CacheKind::rsc:
            m_impl.emplace<rsc::RegionLoader>(dir);
            break;
        case CacheKind::js5:
            throw std::runtime_error(
                "js5 caches (2006+) are not supported");
        default:
            throw std::runtime_error(
                "unrecognized cache layout: " + dir);
        }
    }

    CacheKind kind() const { return m_kind; }

  private:
    // defined before use: deduced-return-type members must be seen before
    // the methods below call them
    template<typename F>
    decltype(auto) visit(F&& f)
    {
        if(auto* a = std::get_if<rs2::RegionLoader>(&m_impl))
            return f(*a);
        return f(*std::get_if<rsc::RegionLoader>(&m_impl));
    }
    template<typename F>
    decltype(auto) visit(F&& f) const
    {
        if(auto* a = std::get_if<rs2::RegionLoader>(&m_impl))
            return f(*a);
        return f(*std::get_if<rsc::RegionLoader>(&m_impl));
    }

  public:
    std::optional<rs2::RegionGeometry> load(int rx, int ry, int plane)
    {
        return visit([&](auto& l) { return l.load(rx, ry, plane); });
    }
    const std::vector<rs2::RegionRef>& regions() const
    {
        return visit([](auto& l) -> auto& { return l.regions(); });
    }
    const rs2::MapBounds& bounds() const
    {
        return visit([](auto& l) -> auto& { return l.bounds(); });
    }
    const rs2::MapBounds& surface_bounds() const
    {
        return visit([](auto& l) -> auto& { return l.surface_bounds(); });
    }
    const rs2::RegionRef* find_region(int rx, int ry) const
    {
        return visit([&](auto& l) { return l.find_region(rx, ry); });
    }
    const std::string& cache_signature() const
    {
        return visit([](auto& l) -> auto& { return l.cache_signature(); });
    }
    rs2::FloorClass floor_class(rs2::i32 overlay_id) const
    {
        return visit([&](auto& l) { return l.floor_class(overlay_id); });
    }
    rs2::Color face_color(rs2::u32 col)
    {
        return visit([&](auto& l) { return l.face_color(col); });
    }
    std::optional<rs2::SpriteRgba> texture_sprite(int id)
    {
        return visit([&](auto& l) { return l.texture_sprite(id); });
    }
    std::vector<rs2::MapLink> find_links()
    {
        return visit([](auto& l) { return l.find_links(); });
    }

    // kind-specific access (nullptr when the other kind is loaded)
    rs2::RegionLoader* rs2_loader()
    {
        return std::get_if<rs2::RegionLoader>(&m_impl);
    }
    rsc::RegionLoader* rsc_loader()
    {
        return std::get_if<rsc::RegionLoader>(&m_impl);
    }

  private:
    CacheKind m_kind;
    std::variant<std::monostate, rs2::RegionLoader, rsc::RegionLoader> m_impl;
};

} // namespace rs
