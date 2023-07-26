#pragma once

#include <peripherals/error/result.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/types.h>
#include <peripherals/typing/pixels/rgba.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <optional>

namespace blam {

using namespace libc_types;
using semantic::Span;
using std::optional;
using stl_types::result;

using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;

using error_msg = std::string_view;

/*!
 * \brief Blam, at least for Halo 1, uses int16 to store bitmap sizes
 */
using bl_size_t = typing::vectors::tvector<i16, 2>;
static_assert(sizeof(bl_size_t) == 4, "bl_size_t must be 4 bytes");
/*!
 * \brief As with blam_size, int16 is standard size for Halo 1.
 */
using bl_point_t = typing::vectors::tvector<i16, 2>;
static_assert(sizeof(bl_point_t) == 4, "bl_point_t must be 4 bytes");

/*!
 * \brief The blam_bounding_box struct
 */
struct bounding_box
{
    Vecf2 x_range;
    Vecf2 y_range;
    Vecf2 z_range;

    inline bool contains(Vecf3 const& point) const
    {
        auto [o, e] = points();

        auto origin_diff = point - o;
        auto extent_diff = e - point;
        bool within_origin
            = origin_diff.x < 0 && origin_diff.y < 0 && origin_diff.z < 0;
        bool within_extents
            = extent_diff.x > 0 && extent_diff.y > 0 && extent_diff.z > 0;
        return within_origin && within_extents;
    }

    std::pair<Vecf3, Vecf3> points() const
    {
        return {
            Vecf3(x_range.x, y_range.x, z_range.x),
            Vecf3(x_range.y, y_range.y, z_range.y),
        };
    }
};
static_assert(sizeof(bounding_box) == 24, "bounding_box must be 24 bytes");

/*!
 * \brief Blam maptypes. Names being obvious, the UI type does not give a
 * playable map.
 */
enum class maptype_t : i32
{
    singleplayer
    = 0,             /*!< A single-player map, typically with cutscenes and AI*/
    multiplayer = 1, /*!< A multi-player map, typically with up to 16 players*/
    ui          = 2, /*!< A UI map, used only in the main menu*/
};

enum class game_difficulty_t : u16
{
    easy       = 0,
    normal     = 1,
    hard       = 2,
    impossible = 3,
};

/*!
 * \brief Possible Blam versions which we may encounter. The different formats
 * work in different ways. For instance, Xbox stores bitmaps and other data in a
 * single file while PC spreads it across "bitmaps.map" and "sounds.map". Both
 * ways are necessary for parsing to happen correctly.
 */
enum class version_t : i32
{
    xbox = 5, /*!< The 2001 version of Halo: Combat Evolved for Xbox*/
    pc   = 7, /*!< The 2004 version of Halo: Combat Evolved for PC*/
    mcc  = 13,

    custom_edition = 609,
};

using bl_rgba_t = typing::pixels::rgba_t;
static_assert(sizeof(bl_rgba_t) == 4, "bl_rgba_t must be 4 bytes");

template<size_t Size>
struct bl_string_var
{
    static constexpr size_t size = Size;

    std::array<char, Size> data{};

    template<typename Dummy = void>
    requires(Size == 4) inline std::string str() const
    {
        if(data.at(0) == 0)
            return {};

        auto out = std::string(data.data(), Size);
        std::reverse(out.begin(), out.end());
        return out;
    }

    template<typename Dummy = void>
    requires(Size != 4) inline std::string_view str() const
    {
        return std::string_view(data.data());
    }

    template<typename Dummy = void>
    requires(Size == 4) explicit operator u32() const
    {
        u32 const* idata = C_RCAST<u32 const*>(data.data());
        return *idata;
    }

    template<typename Dummy = void>
    requires(Size != 4) inline operator std::string_view() const
    {
        return str();
    }

    static std::optional<bl_string_var> from(std::string_view from)
    {
        if(from.size() > Size - 1)
            return std::nullopt;

        bl_string_var out;
        std::copy(from.begin(), from.end(), out.data.begin());
        return out;
    }
};

using bl_tag    = bl_string_var<4>;
using bl_string = bl_string_var<32>;
using bl_header = std::array<char, 4>;
using bl_footer = std::array<char, 4>;

} // namespace blam
