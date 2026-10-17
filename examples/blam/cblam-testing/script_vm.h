#pragma once

#include <blam/volta/blam_strings.h>
#include <blam/volta/hsc/bytecode_common_v12.h>

#include <functional>
#include <map>
#include <random>
#include <string_view>
#include <vector>

/* Runs HaloScript straight off the scenario's syntax nodes. A call node's
 * value names its function-name node, and arguments follow that as siblings;
 * each script thread walks the tree with its own stack of frames, so it can
 * sleep in the middle of an expression and pick up there. */
namespace script_vm {

using libc_types::f32;
using libc_types::i32;
using libc_types::i64;
using libc_types::u16;
using libc_types::u32;
using libc_types::u64;

using blam::hsc::expression_t;
using blam::hsc::script_type_t;
using blam::hsc::type_t;

/* A script value: numbers, an index (object name, trigger volume, encounter,
 * tag datum, ...) or a list of objects */
struct value_t
{
    type_t           type{type_t::void_};
    f32              real{0.f};
    i32              integer{0};
    u32              name{0}; /* literals: where their source text is */
    std::vector<u64> objects;
    std::vector<u32> pending; /* net ids of objects still being spawned */

    static value_t of(type_t type)
    {
        return {.type = type};
    }

    static value_t boolean(bool v)
    {
        return {.type = type_t::bool_, .integer = v ? 1 : 0};
    }

    static value_t number(type_t type, f32 v)
    {
        return {.type = type, .real = v, .integer = static_cast<i32>(v)};
    }

    bool truthy() const
    {
        return type == type_t::real_ ? real != 0.f : integer != 0;
    }

    f32 as_real() const
    {
        return type == type_t::real_ ? real : static_cast<f32>(integer);
    }

    u64 object() const
    {
        return objects.empty() ? 0 : objects.front();
    }
};

enum class status_t
{
    dormant,
    running,
    sleeping,
    finished,
};

template<typename BC>
struct vm
{
    using node_t = blam::hsc::opcode_layout<BC>;

    struct frame_t
    {
        u16                  node;
        u16                  cursor{blam::hsc::terminator}; /* next argument */
        bool                 started{false};
        std::vector<value_t> args;
        std::vector<u16>     order; /* begin_random */
    };

    struct thread_t
    {
        blam::hsc::function_declaration const* script{nullptr};
        status_t                               status{status_t::dormant};
        std::vector<frame_t>                   stack;

        /* Sleeping: until this tick, or until `until` is true, looked at
         * every `every` ticks and given up on at `timeout` */
        i64 wake_at{0};
        u16 until{blam::hsc::terminator};
        u16 every{1};
        i64 timeout{-1};
    };

    /* Everything but the language itself */
    using native_t = std::function<value_t(
        BC opcode, node_t const& call, std::vector<value_t>& args, thread_t& self)>;

    void load(
        semantic::Span<node_t const>                       syntax,
        blam::string_segment_ref const&                    strings,
        semantic::Span<blam::hsc::function_declaration const> scripts,
        semantic::Span<blam::hsc::global const>            globals,
        native_t                                           natives)
    {
        m_nodes   = syntax;
        m_strings = strings;
        m_scripts = scripts;
        m_natives = std::move(natives);
        m_threads.clear();
        m_globals.clear();
        m_now = 0;

        for(auto const& script : scripts)
        {
            if(!is_scheduled(script.schedule))
                continue;
            m_threads.push_back({
                .script = &script,
                .status = script.schedule == script_type_t::dormant ? status_t::dormant
                                                                    : status_t::running,
            });
        }
        /* Initial values come from each global's own expression */
        for(auto const& global : globals)
        {
            auto& slot = m_globals[name_of(global.name)];
            slot       = value_t::of(global.type);
            if(global.index != blam::hsc::terminator)
                slot = evaluate_now(global.index);
            slot.type = global.type;
        }
    }

    /* One simulation tick: runs every thread that is awake */
    void tick()
    {
        m_now++;
        for(auto& thread : m_threads)
        {
            if(thread.status == status_t::sleeping && !awake(thread))
                continue;
            if(thread.status != status_t::running)
                continue;
            run(thread);
        }
    }

    /* (wake name) */
    void wake(std::string_view name)
    {
        for(auto& thread : m_threads)
            if(name_of(thread.script->name) == name &&
               (thread.status == status_t::dormant || thread.status == status_t::sleeping))
            {
                thread.status = status_t::running;
                thread.until  = blam::hsc::terminator;
            }
    }

    i64 now() const
    {
        return m_now;
    }

    std::string_view text(u32 offset) const
    {
        if(offset >= m_strings.data.size() ||
           (offset > 0 && m_strings.data[offset - 1] != '\0'))
            return {};
        return m_strings.data.substr(offset, m_strings.data.find('\0', offset) - offset);
    }

    node_t const* node(u16 index) const
    {
        u32 const pos = static_cast<u32>(index) + 1;
        if(index == blam::hsc::terminator || pos >= m_nodes.size())
            return nullptr;
        return &m_nodes[pos];
    }

    std::vector<thread_t> const& threads() const
    {
        return m_threads;
    }

    value_t* global(std::string_view name)
    {
        auto it = m_globals.find(name);
        return it != m_globals.end() ? &it->second : nullptr;
    }

  private:
    static bool is_scheduled(script_type_t type)
    {
        return type == script_type_t::startup || type == script_type_t::dormant ||
               type == script_type_t::continuous;
    }

    static std::string_view name_of(blam::bl_string const& name)
    {
        return name.str();
    }

    bool awake(thread_t& thread)
    {
        if(thread.until == blam::hsc::terminator)
        {
            if(m_now < thread.wake_at)
                return false;
            thread.status = status_t::running;
            return true;
        }
        /* sleep_until: its condition, every so often */
        if(m_now < thread.wake_at)
            return false;
        bool const timed_out = thread.timeout >= 0 && m_now >= thread.timeout;
        if(!timed_out && !evaluate_now(thread.until).truthy())
        {
            thread.wake_at = m_now + thread.every;
            return false;
        }
        thread.until  = blam::hsc::terminator;
        thread.status = status_t::running;
        return true;
    }

    /* An expression to its end on a scratch thread; sleeps don't apply */
    value_t evaluate_now(u16 index)
    {
        thread_t scratch{.status = status_t::running};
        scratch.stack.push_back({.node = index});
        value_t out;
        for(u32 guard = 0; !scratch.stack.empty() && guard < max_steps; guard++)
            if(auto done = step(scratch))
                out = std::move(*done);
        return out;
    }

    void run(thread_t& thread)
    {
        if(thread.stack.empty())
            thread.stack.push_back({.node = thread.script->index});
        for(u32 guard = 0; guard < max_steps; guard++)
        {
            if(thread.status != status_t::running)
                return;
            if(thread.stack.empty())
            {
                /* The script ran to its end */
                switch(thread.script->schedule)
                {
                case script_type_t::continuous:
                    return; /* again next tick */
                case script_type_t::dormant:
                    thread.status = status_t::dormant;
                    return;
                default:
                    thread.status = status_t::finished;
                    return;
                }
            }
            step(thread);
        }
    }

    /* Advances the top frame once; the value of the outermost expression
     * when it completes */
    std::optional<value_t> step(thread_t& thread)
    {
        frame_t&      frame = thread.stack.back();
        node_t const* n     = node(frame.node);
        if(!n)
            return finish(thread, {});

        switch(n->exp_type)
        {
        case expression_t::expression:
            return finish(thread, literal(*n));
        case expression_t::global_ref: {
            auto const* value = global(text(n->data_ptr));
            return finish(thread, value ? *value : value_t::of(n->ret_type));
        }
        case expression_t::script_ref:
            return call_script(thread, frame, *n);
        case expression_t::group:
            return call(thread, frame, *n);
        default:
            return finish(thread, value_t::of(n->ret_type));
        }
    }

    value_t literal(node_t const& n) const
    {
        switch(n.param_type)
        {
        case type_t::bool_:
            return value_t::boolean(n.bytes[0] != 0);
        case type_t::real_:
            return value_t::number(type_t::real_, n.real);
        case type_t::short_:
            return value_t::number(type_t::short_, n.shorts[0]);
        case type_t::long_:
            return value_t::number(type_t::long_, static_cast<f32>(n.long_));
        default:
            /* Indices and tag data: the node's value */
            return {.type = n.param_type, .integer = n.long_, .name = n.data_ptr};
        }
    }

    /* Pops the frame, handing its value to the frame below */
    std::optional<value_t> finish(thread_t& thread, value_t value)
    {
        thread.stack.pop_back();
        if(thread.stack.empty())
            return value;
        thread.stack.back().args.push_back(std::move(value));
        return std::nullopt;
    }

    /* The first argument of a call: its function-name node's sibling */
    u16 first_argument(node_t const& n) const
    {
        auto const* name = node(static_cast<u16>(n.long_));
        return name ? name->next_op.ip : blam::hsc::terminator;
    }

    bool next_argument(frame_t& frame, node_t const& n)
    {
        if(!frame.started)
        {
            frame.started = true;
            frame.cursor  = first_argument(n);
        }
        if(frame.cursor == blam::hsc::terminator)
            return false;
        u16 const at  = frame.cursor;
        auto const* a = node(at);
        frame.cursor  = a ? a->next_op.ip : blam::hsc::terminator;
        /* The caller's frame may move as the stack grows */
        m_pending = at;
        return true;
    }

    std::optional<value_t> descend(thread_t& thread)
    {
        thread.stack.push_back({.node = m_pending});
        return std::nullopt;
    }

    std::optional<value_t> call_script(thread_t& thread, frame_t& frame, node_t const& n)
    {
        if(!frame.started)
        {
            frame.started  = true;
            auto const* fn = script_named(text(node(static_cast<u16>(n.long_))->data_ptr));
            if(!fn)
                return finish(thread, value_t::of(n.ret_type));
            thread.stack.push_back({.node = fn->index});
            return std::nullopt;
        }
        value_t out = frame.args.empty() ? value_t::of(n.ret_type) : frame.args.back();
        return finish(thread, std::move(out));
    }

    blam::hsc::function_declaration const* script_named(std::string_view name) const
    {
        for(auto const& script : m_scripts)
            if(name_of(script.name) == name)
                return &script;
        return nullptr;
    }

    std::optional<value_t> call(thread_t& thread, frame_t& frame, node_t const& n)
    {
        using op = BC;
        switch(n.opcode)
        {
        case op::begin:
            if(next_argument(frame, n))
                return descend(thread);
            return finish(
                thread, frame.args.empty() ? value_t::of(n.ret_type) : frame.args.back());
        case op::begin_random: {
            /* Each statement, in a random order */
            if(!frame.started)
            {
                for(u16 a = first_argument(n); a != blam::hsc::terminator;)
                {
                    frame.order.push_back(a);
                    a = node(a) ? node(a)->next_op.ip : blam::hsc::terminator;
                }
                std::shuffle(frame.order.begin(), frame.order.end(), m_random);
                frame.started = true;
            }
            if(frame.args.size() < frame.order.size())
            {
                m_pending = frame.order[frame.args.size()];
                return descend(thread);
            }
            return finish(thread, value_t::of(n.ret_type));
        }
        case op::if_: {
            /* condition, then, else */
            if(frame.args.empty())
            {
                if(next_argument(frame, n))
                    return descend(thread);
                return finish(thread, value_t::of(n.ret_type));
            }
            if(frame.args.size() == 1)
            {
                bool const taken = frame.args[0].truthy();
                bool const has_then = next_argument(frame, n);
                if(taken && has_then)
                    return descend(thread);
                bool const has_else = !taken && next_argument(frame, n);
                frame.args.push_back(value_t::of(n.ret_type));
                if(has_else)
                    return descend(thread);
                return finish(thread, value_t::of(n.ret_type));
            }
            return finish(thread, frame.args.back());
        }
        case op::cond: {
            /* (cond (test body...) ...): each clause is a call node whose
             * function name is its test */
            if(next_argument(frame, n))
                return descend(thread);
            return finish(thread, value_t::of(n.ret_type));
        }
        case op::set_: {
            /* The global's name node, then its value */
            if(!frame.started)
            {
                frame.started = true;
                frame.cursor  = first_argument(n);
                auto const* target = node(frame.cursor);
                frame.cursor = target ? target->next_op.ip : blam::hsc::terminator;
                frame.args.push_back({.integer = target ? static_cast<i32>(target->data_ptr) : 0});
            }
            if(frame.args.size() == 1)
            {
                if(next_argument(frame, n))
                    return descend(thread);
                return finish(thread, value_t::of(n.ret_type));
            }
            value_t value = frame.args.back();
            if(auto* slot = global(text(static_cast<u32>(frame.args[0].integer))))
            {
                value.type = slot->type;
                if(slot->type == type_t::real_)
                    value.real = value.as_real();
                else if(value.type != type_t::real_)
                    value.integer = value.real != 0.f && value.integer == 0
                                        ? static_cast<i32>(value.real)
                                        : value.integer;
                *slot = value;
            }
            return finish(thread, std::move(value));
        }
        case op::and_:
        case op::or_: {
            bool const is_and = n.opcode == op::and_;
            if(!frame.args.empty() && frame.args.back().truthy() != is_and)
                return finish(thread, value_t::boolean(!is_and));
            if(next_argument(frame, n))
                return descend(thread);
            return finish(thread, value_t::boolean(is_and));
        }
        case op::sleep_until: {
            /* (sleep_until condition [every [timeout]]): the condition is
             * evaluated by the scheduler, not here */
            if(!frame.started)
            {
                frame.started  = true;
                u16 const cond = first_argument(n);
                u16       rest = node(cond) ? node(cond)->next_op.ip : blam::hsc::terminator;
                frame.cursor   = rest;
                frame.args.push_back({.integer = cond});
            }
            if(next_argument(frame, n))
                return descend(thread);
            u16 const cond  = static_cast<u16>(frame.args[0].integer);
            thread.every    = frame.args.size() > 1 ? std::max<u16>(static_cast<u16>(frame.args[1].as_real()), 1) : 1;
            thread.timeout  = frame.args.size() > 2 ? m_now + static_cast<i64>(frame.args[2].as_real()) : -1;
            if(evaluate_now(cond).truthy())
                return finish(thread, value_t::of(n.ret_type));
            auto out = finish(thread, value_t::of(n.ret_type));
            thread.until   = cond;
            thread.wake_at = m_now + thread.every;
            thread.status  = status_t::sleeping;
            return out;
        }
        default:
            break;
        }

        /* Everything else evaluates its arguments first */
        if(next_argument(frame, n))
            return descend(thread);

        switch(n.opcode)
        {
        case op::not_:
            return finish(thread, value_t::boolean(!arg(frame, 0).truthy()));
        case op::equal:
        case op::nequal:
        case op::greater:
        case op::less:
        case op::gequals:
        case op::lequals:
            return finish(thread, value_t::boolean(compare(n.opcode, frame.args)));
        case op::add_:
        case op::sub_:
        case op::mul_:
        case op::div_:
        case op::min_:
        case op::max_:
            return finish(thread, arithmetic(n, frame.args));
        case op::sleep: {
            /* (sleep ticks [script]) */
            i64 const ticks = static_cast<i64>(arg(frame, 0).as_real());
            if(frame.args.size() > 1)
            {
                for(auto& other : m_threads)
                    if(name_of(other.script->name) == text(frame.args[1].name))
                    {
                        other.wake_at = m_now + ticks;
                        other.status  = ticks < 0 ? status_t::dormant : status_t::sleeping;
                    }
                return finish(thread, value_t::of(n.ret_type));
            }
            auto out = finish(thread, value_t::of(n.ret_type));
            thread.until   = blam::hsc::terminator;
            thread.wake_at = ticks < 0 ? std::numeric_limits<i64>::max() : m_now + ticks;
            thread.status  = status_t::sleeping;
            return out;
        }
        case op::wake: {
            auto const* name = node(first_argument(n));
            if(name)
                wake(text(name->data_ptr));
            return finish(thread, value_t::of(n.ret_type));
        }
        case op::random_range: {
            std::uniform_int_distribution<i32> dist(
                static_cast<i32>(arg(frame, 0).as_real()),
                std::max(static_cast<i32>(arg(frame, 0).as_real()),
                         static_cast<i32>(arg(frame, 1).as_real())));
            return finish(thread, value_t::number(type_t::short_, static_cast<f32>(dist(m_random))));
        }
        case op::real_random_range: {
            std::uniform_real_distribution<f32> dist(
                arg(frame, 0).as_real(),
                std::max(arg(frame, 0).as_real(), arg(frame, 1).as_real()));
            return finish(thread, value_t::number(type_t::real_, dist(m_random)));
        }
        default: {
            value_t out = m_natives ? m_natives(n.opcode, n, frame.args, thread)
                                    : value_t::of(n.ret_type);
            if(out.type == type_t::void_ && n.ret_type != type_t::void_)
                out.type = n.ret_type;
            return finish(thread, std::move(out));
        }
        }
    }

    static value_t const& arg(frame_t const& frame, size_t i)
    {
        static value_t const none{};
        return i < frame.args.size() ? frame.args[i] : none;
    }

    bool compare(BC opcode, std::vector<value_t> const& args) const
    {
        if(args.size() < 2)
            return false;
        auto const& a = args[0];
        auto const& b = args[1];
        bool const numeric = a.type == type_t::real_ || b.type == type_t::real_;
        f32 const  x = numeric ? a.as_real() : static_cast<f32>(a.integer);
        f32 const  y = numeric ? b.as_real() : static_cast<f32>(b.integer);
        bool const same = a.objects.empty() && b.objects.empty() ? x == y : a.objects == b.objects;
        switch(opcode)
        {
        case BC::equal:
            return same;
        case BC::nequal:
            return !same;
        case BC::greater:
            return x > y;
        case BC::less:
            return x < y;
        case BC::gequals:
            return x >= y;
        default:
            return x <= y;
        }
    }

    static value_t arithmetic(node_t const& n, std::vector<value_t> const& args)
    {
        if(args.empty())
            return value_t::of(n.ret_type);
        f32 acc = args[0].as_real();
        for(size_t i = 1; i < args.size(); i++)
        {
            f32 const v = args[i].as_real();
            switch(n.opcode)
            {
            case BC::add_:
                acc += v;
                break;
            case BC::sub_:
                acc -= v;
                break;
            case BC::mul_:
                acc *= v;
                break;
            case BC::div_:
                acc = v != 0.f ? acc / v : 0.f;
                break;
            case BC::min_:
                acc = std::min(acc, v);
                break;
            default:
                acc = std::max(acc, v);
                break;
            }
        }
        return value_t::number(n.ret_type == type_t::real_ ? type_t::real_ : n.ret_type, acc);
    }

    static constexpr u32 max_steps = 1u << 16;

    semantic::Span<node_t const>                          m_nodes{};
    blam::string_segment_ref                              m_strings{};
    semantic::Span<blam::hsc::function_declaration const> m_scripts{};
    native_t                                              m_natives;

    std::vector<thread_t>                         m_threads;
    std::map<std::string_view, value_t, std::less<>> m_globals;
    u16                                           m_pending{blam::hsc::terminator};
    i64                                           m_now{0};
    std::mt19937                                  m_random{std::random_device{}()};
};

} // namespace script_vm
