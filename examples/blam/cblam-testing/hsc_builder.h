#pragma once

#include <blam/volta/blam_strings.h>
#include <blam/volta/hsc/bytecode_common_v12.h>

#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/* HaloScript built in memory, in the same node graph a map's scripts are
 * stored as, so script_vm::vm runs it unchanged. Calls name their
 * function-name node, whose siblings are the arguments; names (globals,
 * scripts) are found by their text in the string segment. */
namespace hsc_builder {

using libc_types::f32;
using libc_types::i16;
using libc_types::i32;
using libc_types::u16;
using libc_types::u32;

using blam::hsc::expression_t;
using blam::hsc::script_type_t;
using blam::hsc::type_t;

template<typename BC>
struct program_t
{
    using node_t = blam::hsc::opcode_layout<BC>;

    /* Node N sits at N + 1, as in a map's syntax data */
    std::vector<node_t>                                     nodes;
    std::string                                             strings;
    std::unique_ptr<blam::hsc::function_declaration[]>      script_storage;
    size_t                                                  script_count{0};
    std::vector<blam::hsc::global>                          globals;

    blam::string_segment_ref segment() const
    {
        return {.data = strings};
    }

    semantic::Span<blam::hsc::function_declaration const> scripts() const
    {
        return {script_storage.get(), script_count};
    }
};

template<typename BC>
class builder
{
  public:
    using node_t = blam::hsc::opcode_layout<BC>;
    using index  = u16;

    builder()
    {
        m_out.nodes.emplace_back();
        name("");
    }

    /* ---- Values ---- */

    index boolean(bool v)
    {
        node_t n = literal(type_t::bool_);
        n.bytes  = {static_cast<libc_types::u8>(v ? 1 : 0), 0, 0, 0};
        return add(n);
    }

    index short_(i16 v)
    {
        node_t n = literal(type_t::short_);
        n.shorts = {v, 0};
        return add(n);
    }

    index real(f32 v)
    {
        node_t n = literal(type_t::real_);
        n.real   = v;
        return add(n);
    }

    index string(std::string_view v)
    {
        node_t n   = literal(type_t::string_);
        n.data_ptr = name(v);
        return add(n);
    }

    /* A script's name, as wake and sleep take it */
    index script_name(std::string_view v)
    {
        node_t n   = literal(type_t::script);
        n.data_ptr = name(v);
        return add(n);
    }

    index global(std::string_view global_name, type_t type)
    {
        node_t n{};
        n.exp_type    = expression_t::global_ref;
        n.param_type  = type;
        n.ret_type    = type;
        n.data_ptr    = name(global_name);
        n.next_op.ip  = blam::hsc::terminator;
        return add(n);
    }

    /* ---- Calls ---- */

    index call(BC opcode, type_t ret, std::initializer_list<index> args)
    {
        node_t fn{};
        fn.exp_type   = expression_t::expression;
        fn.opcode     = opcode;
        fn.ret_type   = type_t::branch_val;
        fn.next_op.ip = chain(args);
        index const name_node = add(fn);

        node_t n{};
        n.exp_type   = expression_t::group;
        n.opcode     = opcode;
        n.ret_type   = ret;
        n.long_      = name_node;
        n.next_op.ip = blam::hsc::terminator;
        return add(n);
    }

    /* Statements one after another, the last one's value */
    index begin(std::initializer_list<index> body, type_t ret = type_t::void_)
    {
        return call(BC::begin, ret, body);
    }

    index if_(index condition, index then, index otherwise = blam::hsc::terminator)
    {
        if(otherwise == blam::hsc::terminator)
            return call(BC::if_, type_t::void_, {condition, then});
        return call(BC::if_, type_t::void_, {condition, then, otherwise});
    }

    index set(std::string_view global_name, type_t type, index value)
    {
        return call(BC::set_, type, {global(global_name, type), value});
    }

    /* A static script of this program, called like a function */
    index call_script(std::string_view script_name_, type_t ret)
    {
        node_t fn{};
        fn.exp_type   = expression_t::expression;
        fn.ret_type   = type_t::branch_val;
        fn.data_ptr   = name(script_name_);
        fn.next_op.ip = blam::hsc::terminator;
        index const name_node = add(fn);

        node_t n{};
        n.exp_type   = expression_t::script_ref;
        n.ret_type   = ret;
        n.long_      = name_node;
        n.next_op.ip = blam::hsc::terminator;
        return add(n);
    }

    /* ---- Declarations ---- */

    void declare_global(std::string_view global_name, type_t type, index initial)
    {
        m_globals.push_back({std::string(global_name), type, initial});
    }

    void script(std::string_view script_name_, script_type_t schedule, index body,
                type_t ret = type_t::void_)
    {
        m_scripts.push_back({std::string(script_name_), schedule, ret, body});
    }

    program_t<BC> build() &&
    {
        for(auto const& g : m_globals)
        {
            blam::hsc::global out{};
            out.name  = *blam::bl_string::from(g.name);
            out.type  = g.type;
            out.index = g.initial;
            m_out.globals.push_back(out);
        }
        m_out.script_count   = m_scripts.size();
        m_out.script_storage = std::make_unique<blam::hsc::function_declaration[]>(m_scripts.size());
        for(size_t i = 0; i < m_scripts.size(); i++)
        {
            auto& out    = m_out.script_storage[i];
            out.name     = *blam::bl_string::from(m_scripts[i].name);
            out.schedule = m_scripts[i].schedule;
            out.type     = m_scripts[i].ret;
            out.index    = m_scripts[i].body;
        }
        return std::move(m_out);
    }

  private:
    struct global_t
    {
        std::string name;
        type_t      type;
        index       initial;
    };
    struct script_t
    {
        std::string   name;
        script_type_t schedule;
        type_t        ret;
        index         body;
    };

    static node_t literal(type_t type)
    {
        node_t n{};
        n.exp_type   = expression_t::expression;
        n.param_type = type;
        n.ret_type   = type;
        n.next_op.ip = blam::hsc::terminator;
        return n;
    }

    index add(node_t const& n)
    {
        m_out.nodes.push_back(n);
        return static_cast<index>(m_out.nodes.size() - 2);
    }

    node_t& at(index i)
    {
        return m_out.nodes[static_cast<size_t>(i) + 1];
    }

    /* Links the arguments as siblings; the first one starts the list */
    index chain(std::initializer_list<index> args)
    {
        index previous = blam::hsc::terminator, first = blam::hsc::terminator;
        for(index a : args)
        {
            if(previous == blam::hsc::terminator)
                first = a;
            else
                at(previous).next_op.ip = a;
            at(a).next_op.ip = blam::hsc::terminator;
            previous         = a;
        }
        return first;
    }

    /* Each text once, NUL-terminated, where the VM reads it */
    u32 name(std::string_view text)
    {
        auto key = std::string(text);
        if(auto it = m_names.find(key); it != m_names.end())
            return it->second;
        u32 const offset = static_cast<u32>(m_out.strings.size());
        m_out.strings += key;
        m_out.strings += '\0';
        m_names.emplace(std::move(key), offset);
        return offset;
    }

    program_t<BC>               m_out;
    std::vector<global_t>       m_globals;
    std::vector<script_t>       m_scripts;
    std::map<std::string, u32>  m_names;
};

} // namespace hsc_builder
