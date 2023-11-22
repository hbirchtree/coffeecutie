#pragma once

#include <deque>

#include "bytecode_common_v12.h"
#include "bytecode_signatures_v12.h"

#include <blam/volta/blam_strings.h>

#include <peripherals/stl/iterator.h>
#include <peripherals/stl/quick_container.h>

namespace blam::hsc {

constexpr bool bypass_conditions = true;

inline bool is_number(type_t t)
{
    return t == type_t::short_ || t == type_t::long_ || t == type_t::real_
           || t == type_t::number;
}

inline bool is_any(type_t t)
{
    return t == type_t::any;
}

template<typename Opcode>
inline bool is_operator(Opcode op)
{
    switch(op)
    {
    case Opcode::equal:
    case Opcode::nequal:
    case Opcode::less:
    case Opcode::greater:
    case Opcode::lequals:
    case Opcode::gequals:
    case Opcode::add_:
    case Opcode::sub_:
    case Opcode::mul_:
    case Opcode::div_:
        return true;
    default:
        return false;
    }
}

inline bool is_value(type_t t)
{
    switch(t)
    {
    case type_t::void_:
    case type_t::unevaluated:
    case type_t::branch_val:
    case type_t::passthrough:
    case type_t::any:
    case type_t::func_name:
        return false;
    default:
        return true;
    }
}

inline bool match_type(type_t t1, type_t t2)
{
    using t = type_t;

    if(is_any(t1) || is_any(t2))
        return true;

    if(t1 == t::number || t2 == t::number)
        return is_number(t1) && is_number(t2);

    return t1 == t2;
}

template<typename BC, typename T>
inline opcode_layout<BC> get_as(T const& src, type_t from, type_t t)
{
    opcode_layout<BC> out = opcode_layout<BC>::typed_(t);

    /* TODO: Prettify this */
    switch(from)
    {
    case type_t::short_: {
        switch(t)
        {
        case type_t::real_:
            out.set(C_CAST<f32>(src.shorts[0]));
            break;
        default:
            out.set(src.shorts[0]);
            break;
        }
        break;
    }
    case type_t::long_: {
        switch(t)
        {
        case type_t::real_:
            out.set(C_CAST<f32>(src.long_));
            break;
        default:
            out.set(src.long_);
            break;
        }
        break;
    }
    case type_t::real_: {
        switch(t)
        {
        case type_t::real_:
            out.set(src.real);
            break;
        default:
            out.set(C_CAST<i32>(src.real));
            break;
        }
        break;
    }
    case type_t::bool_:
        if(t == type_t::bool_)
            out.set(src.bytes[0]);
        break;
    default:
        if(from != t)
            Throw(script_error("invalid conversion for global"));
        out.set_ptr(src.to_ptr());
        break;
    }
    return out;
}

struct opcode_iterator_end_t
{
};

constexpr opcode_iterator_end_t opcode_iterator_end;

template<typename BC>
struct opcode_iterator
    : std::iterator<std::forward_iterator_tag, opcode_layout<BC>>
{
    opcode_iterator(semantic::mem_chunk<u8 const>&& script_base);
    opcode_iterator(opcode_iterator_end_t) :
        m_script(nullptr), m_data(), m_offset(0), m_is_end(true)
    {
    }

    inline opcode_iterator& operator++()
    {
        auto code      = *m_data.at(m_offset);
        auto separator = terminator;
#if !defined(COFFEE_WIN32)
        auto loc
            = ::memmem(code.data, code.size, &separator, sizeof(separator));
#else
        void const* loc = nullptr;
#endif

        if(loc)
        {
            auto new_op = C_RCAST<byte_t const*>(loc);
            auto offset = new_op - code.data;
            m_offset    = offset;
        } else
        {
            m_is_end = true;
            m_offset = std::numeric_limits<u32>::max();
        }

        return *this;
    }

    inline bool operator==(opcode_iterator const& other) const
    {
        return (m_is_end && m_is_end == other.m_is_end)
               || m_offset == other.m_offset;
    }

    inline bool operator!=(opcode_iterator const& other) const
    {
        return !(*this == other);
    }

    inline opcode_layout<BC> const& operator*() const
    {
        auto out
            = (*m_data.at(m_offset)).template as<opcode_layout<BC> const>();
        if(!out)
            Throw(undefined_behavior("invalid iterator"));

        return out[0];
    }

    script_ref<BC> const*         m_script;
    semantic::mem_chunk<u8 const> m_data;
    u32                           m_offset;
    bool                          m_is_end;
};

template<typename BC>
struct script_ref : stl_types::non_copy
{
    bl_string name;
    u32       sentinel_value;

    using container_type = stl_types::quick_container<opcode_iterator<BC>>;

    inline u8 const* opcode_base() const
    {
        return C_RCAST<u8 const*>(&this[1]);
    }

    inline opcode_layout<BC> const& opcode_first() const
    {
        return *C_RCAST<opcode_layout<BC> const*>(opcode_base());
    }

    inline semantic::mem_chunk<u8 const> dump(u32 count) const
    {
        return semantic::mem_chunk<u8 const>::of(opcode_base(), count);
    }
};

template<typename BC>
inline opcode_iterator<BC>::opcode_iterator(
    semantic::mem_chunk<u8 const>&& script_base) :
    m_script(C_RCAST<script_ref<BC> const*>(script_base.data)),
    m_data(*script_base.at(sizeof(script_ref<BC>))), m_offset(0),
    m_is_end(false)
{
}

template<typename T>
inline std::string to_string(T val)
{
#if USE_MAGIC_ENUM
    auto out = magic_enum::enum_name(val);
    if(!out.size())
        return "[invalid(" + std::to_string(C_CAST<i16>(val)) + ")]";
    return std::string(out) + "(" + std::to_string(C_CAST<i16>(val)) + ")";
#else
    return std::to_string(C_CAST<i16>(val));
#endif
}

template<typename BC>
struct bytecode_pointer;

struct script_environment
{
    string_segment_ref const*                  strings;
    semantic::Span<function_declaration const> scripts;
    semantic::Span<global const>               globals;
};

enum class sleep_condition
{
    undefined,
    timer,
    expression,
    expression_timer,
};

enum class script_status
{
    running,  /*!< Continuous or startup script */
    ready,    /*!< Signaled for running */
    sleeping, /*!< Waiting on timer or event */
    dormant,  /*!< Scripts put to sleep forever */
    finished, /*!< Startup scripts end up here */
};

struct global_value
{
    global_value(global const* desc) : desc(desc), type(desc->type)
    {
        long_ = 0;
    }

    global const* desc;
    union
    {
        f32                real;
        i32                long_;
        u32                ptr;
        std::array<i16, 2> shorts;
        std::array<u8, 4>  bytes;
    };
    type_t type;

    template<typename BC>
    void set(opcode_layout<BC> const& op)
    {
        type = op.ret_type;
        switch(type)
        {
        case type_t::bool_:
            bytes[0] = op.to_bool();
            break;
        case type_t::short_:
            shorts[0] = op.to_u16();
            break;
        case type_t::long_:
            long_ = op.to_u32();
            break;
        case type_t::real_:
            real = op.to_real();
            break;
        default:
            ptr = op.to_ptr();
            break;
        }
    }

    template<typename BC>
    auto get_as(type_t to)
    {
        return hsc::get_as<BC>(*this, type, to);
    }

    u32 to_ptr() const
    {
        return ptr;
    }
};

struct wait_condition
{
    sleep_condition           condition{sleep_condition::undefined};
    u16                       expression{terminator};
    std::chrono::milliseconds time{};
    u16                       tickrate{0};
};

template<typename Bytecode>
struct script_context
{
    struct object
    {
    };

    struct script_state
    {
        script_state(function_declaration const* func) : function(func)
        {
            switch(func->schedule)
            {
            case script_type_t::startup:
            case script_type_t::continuous:
                status = script_status::running;
                break;
            default:
                status = script_status::dormant;
                break;
            }

            ip             = func->index;
            current_params = 0;
            script_start = script_end = terminator;
        }

        function_declaration const*          function;
        script_status                        status;
        u16                                  ip;
        u16                                  script_start, script_end;
        u32                                  current_params;
        std::deque<u16>                      link_register;
        std::vector<opcode_layout<Bytecode>> value_stack;
        std::vector<u16>                     value_ptr;
        std::deque<u32>                      param_counts;
        wait_condition                       condition;

        inline bool is_ready() const
        {
            return status == script_status::ready
                   || status == script_status::running;
        }
        inline bool is_inactive() const
        {
            return status == script_status::sleeping
                   || status == script_status::dormant
                   || status == script_status::finished;
        }
        inline std::string_view name() const
        {
            return function->name.str();
        }
    };

    struct procedure_data
    {
        function_declaration const* function;
        u16                         script_start, script_end;
    };

    std::map<u32, procedure_data> procedures;
    std::map<std::string, object> objects;
    std::map<u32, global_value>   globals;
    std::map<u32, script_state>   scripts;

    inline std::optional<function_declaration const*> function_by_ptr(
        u32 ptr) const
    {
        auto it = procedures.find(ptr);

        if(it == procedures.end())
            return std::nullopt;

        return (*it).second;
    }

    inline std::optional<script_state*> script_by_ptr(u32 ptr)
    {
        auto it = scripts.find(ptr);

        if(it == scripts.end())
            return std::nullopt;

        return {&(*it).second};
    }

    inline std::optional<global_value*> global_by_ptr(u32 ptr)
    {
        auto it = globals.find(ptr);

        if(it == globals.end())
            return std::nullopt;

        return {&(*it).second};
    }

    inline std::optional<object*> object_by_name(u32 ptr)
    {
        auto it = objects.find(ptr);

        if(it == objects.end())
            return std::nullopt;

        return {&(*it).second};
    }

    inline object& create_object(u32 ptr)
    {
        return objects.insert({ptr, object{}});
    }

    inline void set_script_state(
        u32 ptr, script_status state, wait_condition /*cond*/)
    {
        auto script = script_by_ptr(ptr).value();

        script->status = state;
    }
};

enum class script_eval_result
{
    running,
    sleeping,
    branching,
};

template<typename BC>
struct bytecode_pointer
{
    using ptr_type        = u16;
    using bytecode_t      = BC;
    using opcode_layout_t = opcode_layout<BC>;
    using context_t       = script_context<bytecode_t>;
    using eval            = script_eval_result;
    using script_state_t  = typename context_t::script_state;
    using bytecode_ptr    = bytecode_pointer<BC>;

    struct result_t
    {
        opcode_layout_t result;
        u16             next;
        eval            state;

        /* For sleep conditions */
        wait_condition condition{};

        u16 end_addr{0};

        static result_t empty()
        {
            return {
                .result = end_(),
                .next   = terminator,
                .state  = eval::running,
            };
        }

        static opcode_layout_t end_()
        {
            opcode_layout<BC> out = {};
            out.next_op.ip        = terminator;
            out.next_op.salt      = terminator;
            return out;
        }
        static result_t return_(opcode_layout_t const& out = {})
        {
            return {
                .result = out,
                .next   = {},
                .state  = eval::running,
            };
        }
        static result_t sleep_timeout(u32 time)
        {
            return {
                end_(),
                terminator,
                eval::sleeping,
                {sleep_condition::timer,
                 terminator,
                 std::chrono::seconds(time),
                 0}};
        }
        static result_t sleep_condition(u16 expr, u16 tick = 1, u32 timeout = 0)
        {
            return {
                end_(),
                terminator,
                eval::sleeping,
                {timeout > 0 ? sleep_condition::expression_timer
                             : sleep_condition::expression,
                 expr,
                 std::chrono::seconds(timeout),
                 tick}};
        }
        static result_t branch_to(u16 addr)
        {
            return {
                .result = end_(),
                .next   = addr,
                .state  = eval::branching,
            };
        }

        opcode_layout<BC> operator*() const
        {
            return result;
        }

        result_t end_at(u16 addr)
        {
            end_addr = addr;
            return *this;
        }
    };

    using opcode_handler_t
        = std::function<result_t(bytecode_ptr&, opcode_layout_t const&)>;

    struct opcode_handlers
    {
        opcode_handler_t cmd, pre{}, post{};
    };

    static bytecode_ptr start_from(
        script_environment const& env,
        opcode_layout_t const*    base,
        ptr_type                  ip = 0x0)
    {
        bytecode_pointer out;
        out.base           = base;
        out.current        = &base[ip]; /* TODO: Add safety here */
        out.current_ip     = ip;
        out.current_params = 0;

        /* Initialize script context */
        for(auto const& script : env.scripts)
        {
            auto idx = env.strings->get_index(script.name);

            if(idx == 0)
                idx = (script.salt << 16) + script.index;

            u16 script_start = base[script.index + 2].next_op.ip + 1,
                script_end   = script.index;

            if(script.is_callable())
                out.context.procedures.emplace(
                    idx,
                    typename context_t::procedure_data{
                        &script, script_start, script_end});
            else if(script.is_scheduled())
            {
                out.context.scripts.emplace(
                    idx, typename context_t::script_state(&script));

                auto it = out.context.scripts.find(idx);

                if(it == out.context.scripts.end())
                    continue;

                /* Rewind the instruction pointer to start of script */
                typename context_t::script_state& state = (*it).second;

                state.ip           = script_start;
                state.script_start = script_start;
                state.script_end   = script_end;
            }
        }

        for(auto const& global : env.globals)
        {
            auto idx = env.strings->get_index(global.name);
            out.context.globals.emplace(idx, global_value(&global));
        }

        return out;
    }

    context_t              context;
    opcode_layout_t const* base;
    opcode_layout_t const* current;
    ptr_type               current_ip;
    u32                    current_params;
    ptr_type               script_start, script_end;

    std::deque<u32> param_counts;
    /*!< Param counts for incomplete groups */
    std::deque<ptr_type> link_register;
    /*!< Return addresses, for calling procedures */

    std::vector<opcode_layout_t> value_stack;
    /*!< Values for consumption by operations */
    std::vector<ptr_type> value_ptr;

    inline std::optional<opcode_layout_t const*> op_at(u16 ip)
    {
        if(ip == terminator)
            return std::nullopt;

        return {&base[ip]};
    }

    inline u16 num_params() const
    {
        return opcode_signature(*current).num_params;
    }
    inline opcode_layout_t const& param(type_t type, ptr_type i = 0) const
    {
        ptr_type first_param = current_ip + 1;
        ptr_type last_param  = current->next_op.ip;

        if(i > (last_param - first_param + 1))
            Throw(script_error("param out of bounds"));

        auto out = &value_stack.at(value_stack.size() - i - 1);

        if(!match_type(type, out->ret_type))
        {
            auto type1 = magic_enum::enum_name(type);
            auto type2 = magic_enum::enum_name(out->ret_type);

            auto type1_ = std::string(type1.begin(), type1.end());
            auto type2_ = std::string(type2.begin(), type2.end());

            Throw(mismatch_param_type(
                "param has wrong type: " + type1_ + " != " + type2_));
        }

        return *out;
    }
    inline ptr_type param_addr(ptr_type i = 0) const
    {
        ptr_type first_param = current_ip + 1;
        ptr_type last_param  = current->next_op.ip;

        if(i > (last_param - first_param + 1))
            Throw(script_error("param out of bounds"));

        return value_ptr.at(value_ptr.size() - i - 1);
    }
    inline type_t param_type(ptr_type i = 0) const
    {
        ptr_type first_param = current_ip + 1;
        ptr_type last_param  = current->next_op.ip;

        if(i > (last_param - first_param + 1))
            Throw(script_error("param out of bounds"));

        auto out = &value_stack.at(value_stack.size() - i - 1);

        return out->ret_type;
    }

    result_t evaluate(
        bytecode_pointer<BC>::opcode_layout_t const& op,
        opcode_handlers const&                       handler);

    inline result_t evaluate_params(
        opcode_handlers const& handler, opcode_layout_t const& op)
    {
        u16 num = 0;

        try
        {
            num = param_count(op);
        } catch(missing_signature const& e)
        {
            num = unknown_opcode_signature;
        }

        if(num == 0)
            return result_t::return_({}).end_at(current_ip + 1);

        u16 param_i = 0;
        u16 last_op = terminator;

        u32 start_param = value_stack.size();

        std::function<bool()> condition
            = [&]() { return (value_stack.size() - start_param) < num; };

        if(num == variable_length_params || num == unknown_opcode_signature)
        {
            /* Functions like sleep_until() may take an arbitrary amount of
             * parameters, but points to the last parameter */
            auto end_addr = current->next_op.ip;
            if(end_addr == terminator)
                end_addr = script_end;

            condition = [&, end_addr]() { return current_ip <= end_addr; };
        }

        u16 jump_offset = 2;

        if(current->template get<u16>() > current_ip)
        {
            auto offset = current->template get<u16>() - current_ip;
            jump_offset += offset;
        }

        jump(current_ip + jump_offset);

        do
        {
            if(param_i < current_params)
            {
                param_i++;
                continue;
            }

            jump(current_ip);
            auto val = evaluate(*current, handler);
            return_();

            if(val.state != eval::running)
            {
                current_params = param_i + 1;
                return val;
            }

            value_stack.push_back(val.result);
            value_ptr.push_back(current_ip);
            last_op = val.end_addr;
            param_i++;
            advance();
        } while(condition());

        current_params = 0;

        if(num == unknown_opcode_signature)
        {
            auto        opname      = magic_enum::enum_name(op.opcode);
            auto        ret_type    = magic_enum::enum_name(op.ret_type);
            std::string param_types = {};
            for(auto it = value_stack.end() - param_i; it != value_stack.end();
                ++it)
            {
                param_types.append(magic_enum::enum_name(it->ret_type));
                param_types.append(" ");
            }
            std::string signature;
            ((((signature += opname) += " ") += ret_type) += ": ")
                += param_types;
            Throw(missing_signature(signature));
        }

        return_();
        return result_t::return_({}).end_at(last_op);
    }
    inline void pop_params(ptr_type num)
    {
        value_stack.erase(value_stack.end() - num, value_stack.end());
        value_ptr.erase(value_ptr.end() - num, value_ptr.end());
    }

    inline f32 deref_real(opcode_layout_t const& from)
    {
        switch(from.exp_type)
        {
        case expression_t::expression:
            return from.to_real();
        case expression_t::global_ref:
            return context.global_by_ptr(from.data_ptr).value()->real;
        case expression_t::script_ref:
        case expression_t::param_ref:
            break;
        }
        __builtin_unreachable();
    }

    inline i32 deref_i32(opcode_layout_t const& from)
    {
        switch(from.exp_type)
        {
        case expression_t::expression:
            return from.to_u32();
        case expression_t::global_ref:
            return context.global_by_ptr(from.data_ptr).value()->long_;
        case expression_t::script_ref:
        case expression_t::param_ref:
            break;
        }
        __builtin_unreachable();
    }

    inline i16 deref_i16(opcode_layout_t const& from)
    {
        switch(from.exp_type)
        {
        case expression_t::expression:
            return from.to_u16();
        case expression_t::global_ref:
            return context.global_by_ptr(from.data_ptr).value()->shorts[0];
        case expression_t::script_ref:
        case expression_t::param_ref:
            break;
        }
        __builtin_unreachable();
    }

    inline void advance()
    {
        if(current->branching())
            current_ip = current->next_op.ip + 1;
        else
            current_ip = current->next_op.ip;
        update_opcode();
    }
    NO_DISCARD inline bool update_opcode()
    {
        auto new_op = op_at(current_ip);

        if(!new_op)
            return false;

        current = new_op.value();
        return true;
    }
    inline bool finished() const
    {
        return current_ip == terminator;
    }
    inline void call(u16 ip)
    {
        /* Function table points to the last opcode of a function, followd
         * by two opcodes with begin(). The last begin() opcode contains the
         * start of the function */
        jump(op_at(ip + 2).value()->next_op.ip + 1);
    }
    inline void jump(opcode_layout_t const& opcode)
    {
        auto end = &opcode;

        if((end - base) < 0)
            Throw(undefined_behavior("invalid jump"));

        jump(C_FCAST<ptr_type>(end - base));
    }
    inline void jump(ptr_type ip)
    {
        link_register.push_back(current_ip);
        current_ip = ip;
        param_counts.push_back(current_params);
        current_params = 0;
        update_opcode();
    }
    inline void return_()
    {
        current_ip = link_register.back();
        link_register.pop_back();
        current_params = param_counts.back();
        param_counts.pop_back();
        update_opcode();
    }

    NO_DISCARD inline bool restore_state(script_state_t& state)
    {
        link_register  = std::move(state.link_register);
        current_ip     = state.ip;
        script_start   = state.script_start;
        script_end     = state.script_end;
        value_stack    = std::move(state.value_stack);
        value_ptr      = std::move(state.value_ptr);
        param_counts   = std::move(state.param_counts);
        current_params = state.current_params;
        return update_opcode();
    }
    inline void stash_state(script_state_t& state)
    {
        state.link_register  = std::move(link_register);
        state.ip             = current_ip;
        state.value_stack    = std::move(value_stack);
        state.value_ptr      = std::move(value_ptr);
        state.current_params = current_params;
        state.param_counts   = std::move(param_counts);
    }

    inline void init_globals(
        semantic::Span<global const> const& globals,
        string_segment_ref const&           string_segment,
        opcode_handlers const&              handler)
    {
        for(auto const& global : globals)
        {
            auto key = string_segment.get_index(global.name);
            context.globals.insert({key, blam::hsc::global_value(&global)});
            auto glob = context.global_by_ptr(key).value();

            jump(global.index + 1);

            glob->set(*evaluate(**op_at(global.index + 1), handler));

            return_();
        }

        static std::array<global, 3> engine_props = {{
            global{
                .name = *bl_string::from("rasterizer_near_clip_distance"),
                .type = type_t::real_,
            },
            global{
                .name = *bl_string::from("weather"),
                .type = type_t::bool_,
            },
            global{
                .name = *bl_string::from("debug"),
                .type = type_t::bool_,
            },
        }};

        auto& all_globals = context.globals;

        for(auto& prop : engine_props)
        {
            auto idx = string_segment.get_index(prop.name);

            if(!idx)
                continue;

            all_globals.insert({idx, global_value(&prop)});
        }
    }

    inline void update_sleepers(
        std::chrono::milliseconds delta, opcode_handlers const& handler)
    {
        using std::chrono::milliseconds;

        for(auto& script : context.scripts)
        {
            script_state_t& state = script.second;

            /* Running? Nothing to do */
            if(state.status != script_status::sleeping)
                continue;

            wait_condition& condition = state.condition;

            switch(condition.condition)
            {
            case sleep_condition::timer: {
                condition.time -= delta;

                if(condition.time <= milliseconds::zero() || bypass_conditions)
                {
                    condition    = {};
                    state.status = script_status::running;
                    restore_state(state);
                    advance();
                    stash_state(state);
                }
                break;
            }
            case sleep_condition::expression_timer:
            case sleep_condition::expression: {
                restore_state(state);
                jump(condition.expression);
                auto result
                    = *evaluate(*op_at(condition.expression).value(), handler);

                if(condition.condition == sleep_condition::expression_timer)
                {
                    condition.time -= delta;

                    if(condition.time <= milliseconds::zero())
                        result.set(true);
                }

                if(bypass_conditions)
                    result.set(true);

                if(result.to_bool())
                    state.status = script_status::running;

                return_();

                if(result.to_bool())
                    advance();

                stash_state(state);
                break;
            }
            case sleep_condition::undefined:
                break;
            default:
                Throw(script_error("invalid sleep condition"));
            }
        }
    }

    inline void execute_state(
        script_state_t& state, opcode_handlers const& handler)
    {
        if(state.is_inactive())
            return;

        restore_state(state);

        eval     run_state = eval::running;
        result_t result;
        while(run_state == eval::running)
        {
            while(!finished() && run_state == eval::running)
            {
                result    = evaluate(*current, handler);
                run_state = result.state;
                if(run_state == eval::running)
                    advance();
                if(is_value(result.result.ret_type))
                {
                    value_stack.push_back(result.result);
                    value_ptr.push_back(current_ip);
                }
            }

            if(run_state == eval::branching)
            {
                jump(result.next);
                run_state = eval::running;
            } else if(run_state == eval::sleeping)
            {
                /* TODO: Set up wait_condition */
                state.status    = script_status::sleeping;
                state.condition = result.condition;
                break;
            } else if(
                state.function->schedule == script_type_t::startup
                && current_ip == terminator)
            {
                state.status = script_status::finished;
                break;
            } else if(
                state.function->schedule == script_type_t::dormant
                && current_ip == terminator)
            {
                state.status = script_status::sleeping;
                current_ip   = state.script_start;
                update_opcode();
                break;
            } else if(
                state.function->schedule == script_type_t::continuous
                && current_ip == terminator)
            {
                current_ip = state.script_start;
                update_opcode();
                break;
            } else if(
                run_state == eval::running && current_ip == terminator
                && !link_register.empty())
            {
                return_();
                advance();
            } else
            {
                /* Reset bytecode pointer */
                current_ip = state.script_start;
                update_opcode();
            }
        }

        stash_state(state);
    }

    inline void execute_timestep(
        std::chrono::milliseconds delta, opcode_handlers const& handler)
    {
        for(auto& s : context.scripts)
            execute_state(s.second, handler);

        update_sleepers(delta, handler);
    }
}; // namespace hsc

template<typename Bytecode>
struct types
{
    using bytecode_ptr   = bytecode_pointer<Bytecode>;
    using layout_t       = opcode_layout<Bytecode>;
    using opcode_t       = Bytecode;
    using opcode_handler = typename bytecode_ptr::opcode_handler_t;
    using ptr_type       = typename bytecode_ptr::ptr_type;
    using result_t       = typename bytecode_ptr::result_t;

    static inline auto signature(layout_t const& opc)
    {
        return hsc::opcode_signature<Bytecode>(opc);
    }
};

} // namespace blam::hsc

namespace Coffee {
namespace Strings {

template<
    typename BC,
    typename std::enable_if<
        std::is_same<BC, blam::hsc::bc::v1>::value
        || std::is_same<BC, blam::hsc::bc::v2>::value>::type* = nullptr>
inline std::string to_string(BC opc)
{
    return blam::hsc::to_string(opc);
}

inline std::string to_string(blam::hsc::type_t type)
{
    return blam::hsc::to_string(type);
}

inline std::string to_string(blam::hsc::script_eval_result type)
{
    return blam::hsc::to_string(type);
}

inline std::string to_string(blam::hsc::expression_t exp)
{
    return blam::hsc::to_string(exp);
}

inline std::string to_string(blam::hsc::script_type_t script_type)
{
    return blam::hsc::to_string(script_type);
}

inline std::string to_string(blam::hsc::script_status stat)
{
    return blam::hsc::to_string(stat);
}

template<typename BC>
inline std::string to_string(blam::hsc::opcode_layout<BC> const& op)
{
    using namespace blam::hsc;
    std::string out = {};

    if(op.exp_type == expression_t::global_ref)
        out = "global@" + std::to_string(op.data_ptr);
    else
        switch(op.param_type)
        {
        case type_t::bool_:
            out = op.to_bool() ? "true" : "false";
            break;
        case type_t::real_:
            out = std::to_string(op.to_real());
            break;
        case type_t::short_:
            out = std::to_string(op.to_u16());
            break;
        case type_t::long_:
            out = std::to_string(op.to_u32());
            break;
        default:
            out = std::to_string(op.template get<i32>()) + "/"
                  + std::to_string(op.to_ptr());
            break;
        }

    return "[" + blam::hsc::to_string(op.ret_type) + ":" + out + "]";
}

} // namespace Strings
} // namespace Coffee
