#pragma once

#include "cblam_structures.h"

namespace blam {
namespace hsc {

constexpr u16 terminator = 0xFFFF;

enum class opcode_t : u16
{
    noop = 0x0,
};

struct script_header
{
    u16 unknown[7];
};

struct opcode_layout
{
    u16      salt;
    opcode_t opcode;
};

struct script_ref;

struct opcode_iterator_end_t
{
};

constexpr opcode_iterator_end_t opcode_iterator_end;

struct opcode_iterator
    : stl_types::Iterator<stl_types::ForwardIteratorTag, opcode_layout>
{
    opcode_iterator(semantic::mem_chunk<u8 const>&& script_base);
    opcode_iterator(opcode_iterator_end_t end) :
        m_script(nullptr), m_data(), m_offset(0), m_is_end(true)
    {
    }

    inline opcode_iterator& operator++()
    {
        auto code      = m_data.at(m_offset);
        auto separator = terminator;
        auto loc =
            ::memmem(code.data, code.size, &separator, sizeof(separator));

        if(loc)
        {
            auto new_op = C_RCAST<byte_t const*>(loc);
            auto offset = new_op - code.data;
            m_offset = offset;
        }else
        {
            m_is_end = true;
            m_offset = std::numeric_limits<u32>::max();
        }

        return *this;
    }

    inline bool operator==(opcode_iterator const& other) const
    {
        return (m_is_end && m_is_end == other.m_is_end) ||
               m_offset == other.m_offset;
    }

    inline bool operator!=(opcode_iterator const& other) const
    {
        return !(*this == other);
    }

    inline opcode_layout const& operator*() const
    {
        auto out = m_data.at(m_offset).as<opcode_layout const>();
        if(!out)
            Throw(undefined_behavior("invalid iterator"));

        return out[0];
    }

    script_ref const*             m_script;
    semantic::mem_chunk<u8 const> m_data;
    u32                           m_offset;
    bool                          m_is_end;
};

struct script_ref
{
    bl_string     name;
    script_header header; /* Questionmark? */

    using container_type = stl_types::quick_container<opcode_iterator>;

    inline container_type opcodes(u32 count) const
    {
        return container_type(
            [this, count]() { return opcode_iterator(this->dump(count)); },
            []() { return opcode_iterator(opcode_iterator_end); });
    }

    inline semantic::mem_chunk<u8 const> dump(u32 count) const
    {
        return semantic::mem_chunk<u8 const>::From(
            C_RCAST<u8 const*>(&this[1]), count);
    }
};

inline opcode_iterator::opcode_iterator(
    semantic::mem_chunk<u8 const>&& script_base) :
    m_script(C_RCAST<script_ref const*>(script_base.data)),
    m_data(script_base.at(sizeof(script_ref))), m_offset(0), m_is_end(false)
{
}

} // namespace hsc
} // namespace blam
