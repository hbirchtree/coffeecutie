#pragma once

#include "script_opcodes.h"
#include "structures.h"

namespace blam::dimeter::geo {
struct structure_bsp_ref_t;
} // namespace blam::dimeter::geo

#include <array>
#include <cstring>
#include <string>

namespace blam::dimeter::hsc {

/*!
 * Halo 2 HaloScript as compiled into the scenario: a script and global
 * table, one expression node table (0x14 bytes each, datum-indexed) and a
 * string table the nodes point into by map address. Layout from Assembly's
 * Halo2Xbox scnr.xml; same node graph shape as Halo 1 (blam::hsc).
 */

/*! Same values as Halo 1's blam::hsc::expression_t */
enum class expression_type_t : u16
{
    group            = 0x8, /*!< (function args...), data = first child */
    expression       = 0x9, /*!< a literal or name */
    script_reference = 0xA, /*!< (script args...), opcode = script index */
    global_reference = 0xD, /*!< opcode = global index, 0x8000 = scenario's */
};

struct alignas(4) script_t
{
    bl_string   name;
    u16         type_raw;
    u16         return_type_raw;
    datum_index root;

    inline script_type_t type() const
    {
        return static_cast<script_type_t>(from_le(type_raw));
    }

    inline value_type_t return_type() const
    {
        return static_cast<value_type_t>(from_le(return_type_raw));
    }
};

static_assert(sizeof(script_t) == 0x28);

struct alignas(4) global_t
{
    bl_string   name;
    u16         type_raw;
    u16         unknown_0;
    datum_index initializer;

    inline value_type_t type() const
    {
        return static_cast<value_type_t>(from_le(type_raw));
    }
};

static_assert(sizeof(global_t) == 0x28);

struct alignas(4) expression_t
{
    u16               salt_raw;
    u16               opcode_raw;
    u16               value_type_raw;
    u16               type_raw;
    datum_index       next;
    u32               string_address_raw;
    std::array<u8, 4> data; /*!< child datum, or the literal, little-endian */

    inline u16 salt() const
    {
        return from_le(salt_raw);
    }

    inline u16 opcode() const
    {
        return from_le(opcode_raw);
    }

    inline value_type_t value_type() const
    {
        return static_cast<value_type_t>(from_le(value_type_raw));
    }

    inline expression_type_t type() const
    {
        return static_cast<expression_type_t>(from_le(type_raw));
    }

    /*! Offset into the scenario's script string data */
    inline u32 string_offset() const
    {
        return from_le(string_address_raw);
    }

    inline u32 data_u32() const
    {
        u32 v;
        std::memcpy(&v, data.data(), 4);
        return from_le(v);
    }

    inline datum_index data_datum() const
    {
        return datum_index{.raw = data_u32()};
    }

    inline f32 data_f32() const
    {
        f32 v;
        std::memcpy(&v, data.data(), 4);
        return from_le(v);
    }

    inline i16 data_i16() const
    {
        i16 v;
        std::memcpy(&v, data.data(), 2);
        return from_le(v);
    }
};

static_assert(sizeof(expression_t) == 0x14);

struct alignas(4) script_reference_t
{
    tag_reference<> reference;
};

static_assert(sizeof(script_reference_t) == 8);

/*! Named point of a scripting point set */
struct alignas(4) point_t
{
    bl_string name;
    Vecf3     position;
    i16       reference_frame_raw;
    i16       unknown_0;
    i32       surface_index_raw;
    Vecf2     facing; /*!< yaw, pitch in degrees */
};

static_assert(sizeof(point_t) == 0x3C);

enum class point_set_flags_t : u32
{
    none                   = 0x0,
    manual_reference_frame = 0x1,
    turret_deployment      = 0x2,
};
C_FLAGS(point_set_flags_t, u32);

struct alignas(4) point_set_t
{
    bl_string                             name;
    tag_block<point_t>                    points;
    block_index<geo::structure_bsp_ref_t> bsp_index;
    i16                                   manual_reference_frame_raw;
    point_set_flags_t                     flags;
};

static_assert(sizeof(point_set_t) == 0x30);

struct alignas(4) scripting_data_t
{
    tag_block<point_set_t> point_sets;
    u8                     unknown_0[0x80 - 8];
};

static_assert(sizeof(scripting_data_t) == 0x80);

/*! The expression table and string table of one scenario, resolved */
struct syntax_t
{
    Span<expression_t const> nodes;
    Span<byte_t const>       strings;
    Span<script_t const>     scripts;
    Span<global_t const>     globals;

    /*! The node a datum names, salt-checked */
    inline expression_t const* at(datum_index d) const
    {
        if(!d.valid() || d.index() >= nodes.size())
            return nullptr;
        auto const& n = nodes[d.index()];
        return n.salt() == d.salt() ? &n : nullptr;
    }

    /*! The token the node was compiled from, empty when unresolvable */
    inline std::string_view string(expression_t const& e) const
    {
        u32 offset = e.string_offset();
        if(offset >= strings.size())
            return {};
        auto const* start =
            reinterpret_cast<char const*>(strings.data()) + offset;
        return {start, strnlen(start, strings.size() - offset)};
    }
};

namespace detail {

inline bool multiline_function(std::string_view name)
{
    return name == "begin" || name == "begin_random" || name == "if" ||
           name == "cond" || name == "sleep_until" || name == "when" ||
           name == "ai_place" || name == "unit_set_animation_mode";
}

inline void decompile_node(
    syntax_t const&     syn,
    expression_t const& e,
    std::string&        out,
    int                 indent,
    u32&                budget);

/*! The function name and its arguments; the first child is the name */
inline void decompile_call(
    syntax_t const&     syn,
    expression_t const& e,
    std::string&        out,
    int                 indent,
    u32&                budget)
{
    auto const* head = syn.at(e.data_datum());
    std::string name(head ? syn.string(*head) : std::string_view{});
    if(name.empty())
    {
        if(e.type() == expression_type_t::group)
            name = to_string(static_cast<bc::v8>(e.opcode()));
        if(name.empty() || name == "?")
            name = "op_" + std::to_string(e.opcode());
    }
    out += '(';
    out += name;
    bool        multiline = multiline_function(name);
    auto const* arg       = head ? syn.at(head->next) : nullptr;
    if(!multiline)
        for(auto const* a = arg; a; a = syn.at(a->next))
            if(a->type() != expression_type_t::expression)
            {
                multiline = true;
                break;
            }
    for(; arg; arg = syn.at(arg->next))
    {
        if(multiline)
        {
            out += '\n';
            out.append(static_cast<size_t>(indent + 2), ' ');
        } else
            out += ' ';
        decompile_node(syn, *arg, out, indent + 2, budget);
    }
    out += ')';
}

inline void decompile_node(
    syntax_t const&     syn,
    expression_t const& e,
    std::string&        out,
    int                 indent,
    u32&                budget)
{
    if(budget == 0)
    {
        out += "...";
        return;
    }
    budget--;
    switch(e.type())
    {
    case expression_type_t::group:
    case expression_type_t::script_reference:
        decompile_call(syn, e, out, indent, budget);
        return;
    case expression_type_t::global_reference: {
        auto name = syn.string(e);
        if(!name.empty())
            out += name;
        else if(e.opcode() & 0x8000)
        {
            u16 index = e.opcode() & 0x7FFF;
            if(index < syn.globals.size())
                out += syn.globals[index].name.str();
            else
                out += "global_" + std::to_string(index);
        } else
            out += to_string(static_cast<bc::global_v8>(e.opcode()));
        return;
    }
    case expression_type_t::expression:
        break;
    default:
        out += "<node type " + std::to_string(static_cast<u16>(e.type())) + ">";
        return;
    }
    switch(e.value_type())
    {
    case value_type_t::boolean:
        out += e.data[0] ? "true" : "false";
        return;
    case value_type_t::real:
        out += std::to_string(e.data_f32());
        return;
    case value_type_t::short_:
        out += std::to_string(e.data_i16());
        return;
    case value_type_t::long_:
        out += std::to_string(static_cast<i32>(e.data_u32()));
        return;
    case value_type_t::string: {
        out += '"';
        out += syn.string(e);
        out += '"';
        return;
    }
    default:
        break;
    }
    /* Names and enums keep their source token in the string table */
    auto token = syn.string(e);
    if(token.empty() && e.value_type() == value_type_t::script &&
       e.data_i16() >= 0 &&
       static_cast<size_t>(e.data_i16()) < syn.scripts.size())
        token = syn.scripts[e.data_i16()].name.str();
    if(!token.empty())
        out += token;
    else if(e.value_type() == value_type_t::string_id && e.data_u32() == 0)
        out += "\"\"";
    else
        out += "<" + std::string(to_string(e.value_type())) + " " +
               std::to_string(e.data_u32()) + ">";
}

} // namespace detail

/*! Appends the expression tree at `root` as source, indented from
 * `indent`; `budget` caps the nodes visited so cycles cannot hang it */
inline void decompile(
    syntax_t const& syn,
    datum_index     root,
    std::string&    out,
    int             indent = 0,
    u32             budget = 1u << 16)
{
    auto const* node = syn.at(root);
    if(!node)
    {
        out += "<missing>";
        return;
    }
    detail::decompile_node(syn, *node, out, indent, budget);
}

/*! One script as it would appear in source */
inline std::string decompile_script(syntax_t const& syn, script_t const& script)
{
    std::string out = "(script ";
    out += to_string(script.type());
    out += ' ';
    out += to_string(script.return_type());
    out += ' ';
    out += script.name.str();
    out += "\n  ";
    decompile(syn, script.root, out, 2);
    out += ")\n";
    return out;
}

} // namespace blam::dimeter::hsc
