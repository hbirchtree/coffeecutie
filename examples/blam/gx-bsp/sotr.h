#pragma once

/* shader_transparent_generic (sotr) on the TEV. sotr is the Xbox's register
 * combiners: per stage and channel, A*B and C*D of mapped inputs, written as
 * they are, summed or muxed. A TEV stage reads one texture and one constant
 * and has no input mappings, so a combiner stage becomes one to four TEV
 * stages, with temporaries in the TEV registers the shader doesn't use. */

#include <coffee/graphics/apis/gexxo/rhi_resources.h>

#include <ogc/gx.h>

#include <algorithm>
#include <array>
#include <optional>
#include <vector>

namespace gx_sotr {

using libc_types::f32;
using libc_types::i8;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;
using typing::vector_types::Vecf4;
using combiner_t = gexxo::program_t::stage_t::combiner_t;
using stage_t    = gexxo::program_t::stage_t;

/* One channel of a combiner stage, numbers as the tag has them */
struct half_t
{
    std::array<u16, 4> input{}, mapping{};
    u16                ab{0}, cd{0}, sum{0}; // destinations
    u16                output_mapping{0};
    bool               mux{false};
};

struct stage_desc_t
{
    half_t color, alpha;
};

/* A constant the compiled program reads: stage `stage`'s constant 0 or 1 */
struct konst_recipe_t
{
    u32 stage{0};
    u32 which{0};
};

struct program_t
{
    std::vector<stage_t>        stages;
    std::vector<konst_recipe_t> konst; // GX_KCOLOR0..3
    bool                        ok{false};
    char const*                 failure{nullptr};
};

/* The tag's register-combiner registers */
namespace input {
enum : u16
{
    zero,
    one,
    half,
    minus_one,
    minus_half,
    map0,
    vertex0   = map0 + 4,
    scratch0  = vertex0 + 2,
    constant0 = scratch0 + 2,
    /* The same again, alpha (colour channel) or blue (alpha channel) */
    other = constant0 + 2,
};
} // namespace input

namespace output {
enum : u16
{
    discard,
    scratch0,
    scratch1,
    vertex0,
    vertex1,
    map0,
};
} // namespace output

namespace mapping {
enum : u16
{
    clamp,
    invert,        // 1 - x
    expand,        // 2x - 1
    invert_expand, // 1 - 2x
    minus_half,    // x - 1/2
    half_minus,    // 1/2 - x
    passthrough,   // x
    negate,        // -x
};
} // namespace mapping

namespace detail {

/* A TEV operand: selector plus what the stage must provide for it */
struct operand_t
{
    u8                sel{0};
    i8                tex{-1};
    std::optional<u8> konst{};          // KCSEL / KASEL
    bool              swap_blue{false}; // alpha reads the texture's blue
};

/* One TEV stage's work in one channel */
struct step_t
{
    combiner_t        comb;
    i8                tex{-1};
    std::optional<u8> konst{};
    bool              swap_blue{false};
};

struct channel_t
{
    bool alpha{false};

    u8 zero() const
    {
        return alpha ? GX_CA_ZERO : GX_CC_ZERO;
    }
};

class compiler_t
{
  public:
    explicit compiler_t(std::vector<stage_desc_t> const& stages)
        : m_stages(stages)
    {
    }

    program_t run()
    {
        program_t out;
        uses_scratch1();
        for(u32 i = 0; i < m_stages.size(); i++)
        {
            m_stage = i;
            std::vector<step_t> color, alpha;
            if(!half(m_stages[i].color, {false}, color) ||
               !half(m_stages[i].alpha, {true}, alpha))
            {
                out.failure = m_failure;
                return out;
            }
            pair(color, alpha, out.stages);
        }
        // The fragment is scratch 0
        if(out.stages.empty() ||
           out.stages.back().color_combiner.out != GX_TEVPREV ||
           out.stages.back().alpha_combiner.out != GX_TEVPREV)
            out.stages.push_back(
                tev_stage(idle({false}), idle({true}), -1, {}, {}, false));
        if(out.stages.size() > 16 || m_konst.size() > 4)
        {
            out.failure = "too many stages";
            return out;
        }
        for(u32 i = 0; i < out.stages.size(); i++)
            out.stages[i].stage =
                static_cast<stage_t::id_t>(stage_t::stage0 + i);
        out.konst = m_konst;
        out.ok    = true;
        return out;
    }

  private:
    /* Scratch 1 lives in REG0; where a channel of it isn't used, that half
     * of REG0 is a temporary too */
    void uses_scratch1()
    {
        auto scan = [this](half_t const& h, bool alpha) {
            for(u16 in : h.input)
            {
                bool const other = in >= input::other;
                u16 const base = other ? in - (input::other - input::map0) : in;
                if(base != input::scratch0 + 1)
                    continue;
                // Colour: other = alpha. Alpha: other = blue
                bool const reads_alpha = alpha != other;
                (reads_alpha ? m_scratch1_alpha : m_scratch1_rgb) = true;
            }
            if(h.ab == output::scratch1 || h.cd == output::scratch1 ||
               h.sum == output::scratch1)
                (alpha ? m_scratch1_alpha : m_scratch1_rgb) = true;
        };
        for(auto const& s : m_stages)
        {
            scan(s.color, false);
            scan(s.alpha, true);
        }
    }

    u8 temp(u32 n)
    {
        static constexpr u8 regs[3] = {GX_TEVREG2, GX_TEVREG1, GX_TEVREG0};
        bool const scratch1 = m_alpha ? m_scratch1_alpha : m_scratch1_rgb;
        if(n < (scratch1 ? 2u : 3u))
            return regs[n];
        m_failure = "out of temporaries";
        return 0xFF;
    }

    static u8 reg_sel(u8 reg, channel_t ch, bool alpha_of = false)
    {
        if(ch.alpha)
            return GX_CA_APREV + reg;
        return (alpha_of ? GX_CC_APREV : GX_CC_CPREV) + reg * 2;
    }

    std::optional<u8> konst_slot(u32 which)
    {
        konst_recipe_t const r{m_stage, which};
        for(u32 i = 0; i < m_konst.size(); i++)
            if(m_konst[i].stage == r.stage && m_konst[i].which == r.which)
                return static_cast<u8>(i);
        if(m_konst.size() == 4)
        {
            m_failure = "out of constants";
            return std::nullopt;
        }
        m_konst.push_back(r);
        return static_cast<u8>(m_konst.size() - 1);
    }

    /* A tag input as a TEV operand; nullopt when it can't be one */
    std::optional<operand_t> operand(u16 in, channel_t ch)
    {
        bool const other = in >= input::other;
        u16 const  base  = other ? in - (input::other - input::map0) : in;
        // Colour channel: other = alpha. Alpha channel: other = blue
        bool const want_alpha = ch.alpha || other;
        switch(base)
        {
        case input::zero:
        case input::minus_one: // negatives clamp to zero on the TEV
        case input::minus_half:
            return operand_t{ch.zero()};
        case input::one:
            return ch.alpha ? operand_t{GX_CA_KONST, -1, GX_TEV_KASEL_1}
                            : operand_t{GX_CC_ONE};
        case input::half:
            // The raster alpha is 1/2, see material_color()
            return ch.alpha ? operand_t{GX_CA_RASA} : operand_t{GX_CC_HALF};
        }
        if(base >= input::map0 && base < input::vertex0)
        {
            i8 const tex = static_cast<i8>(base - input::map0);
            if(ch.alpha)
                return operand_t{GX_CA_TEXA, tex, {}, other};
            return operand_t{
                static_cast<u8>(want_alpha ? GX_CC_TEXA : GX_CC_TEXC), tex};
        }
        // Vertex colours: lighting and fade, neither of which we do
        if(base >= input::vertex0 && base < input::scratch0)
            return operand(input::one, ch);
        if(base >= input::scratch0 && base < input::constant0)
        {
            u8 const reg = base == input::scratch0 ? GX_TEVPREV : GX_TEVREG0;
            // Blue of a register isn't readable in the alpha channel
            return operand_t{reg_sel(reg, ch, want_alpha)};
        }
        if(base >= input::constant0 && base < input::other)
        {
            auto slot = konst_slot(base - input::constant0);
            if(!slot)
                return std::nullopt;
            if(ch.alpha)
                return operand_t{
                    GX_CA_KONST,
                    -1,
                    static_cast<u8>(
                        (other ? GX_TEV_KASEL_K0_B : GX_TEV_KASEL_K0_A) +
                        *slot)}; // KASEL_Kn_* step by one per slot
            return operand_t{
                GX_CC_KONST,
                -1,
                static_cast<u8>(
                    (want_alpha ? GX_TEV_KCSEL_K0_A : GX_TEV_KCSEL_K0) +
                    *slot)};
        }
        return std::nullopt;
    }

    static bool compatible(operand_t const& a, operand_t const& b)
    {
        bool const tex = a.tex < 0 || b.tex < 0 ||
                         (a.tex == b.tex && a.swap_blue == b.swap_blue);
        bool const konst = !a.konst || !b.konst || *a.konst == *b.konst;
        return tex && konst;
    }

    static void merge(step_t& s, operand_t const& o)
    {
        if(o.tex >= 0)
        {
            s.tex       = o.tex;
            s.swap_blue = o.swap_blue;
        }
        if(o.konst)
            s.konst = o.konst;
    }

    static step_t step_of(
        u8 a, u8 b, u8 c, u8 d, std::initializer_list<operand_t const*> uses)
    {
        step_t s{.comb = {a, b, c, d}};
        for(auto const* u : uses)
            merge(s, *u);
        return s;
    }

    /* Into a temporary, mapped */
    std::optional<operand_t> load(
        operand_t const& op, u16 map, channel_t ch, std::vector<step_t>& steps)
    {
        u8 const t = temp(m_temps++);
        if(t == 0xFF)
            return std::nullopt;
        u8 const z     = ch.zero();
        auto     s     = step_of(z, z, z, op.sel, {&op});
        auto     place = [&](operand_t const& value) -> bool {
            // d op a: a free konst slot for the 1 or 1/2 it's taken from
            if(!compatible(value, op))
                return false;
            s         = step_of(op.sel, z, z, value.sel, {&op, &value});
            s.comb.op = GX_TEV_SUB;
            return true;
        };
        switch(map)
        {
        case mapping::invert:
            if(!place(*operand(input::one, ch)))
                return load_twice(op, map, ch, steps);
            break;
        case mapping::half_minus:
            if(!place(*operand(input::half, ch)))
                return load_twice(op, map, ch, steps);
            break;
        case mapping::invert_expand:
            if(!place(*operand(input::half, ch)))
                return load_twice(op, map, ch, steps);
            s.comb.scale = GX_CS_SCALE_2;
            break;
        case mapping::expand:
            s.comb.bias  = GX_TB_SUBHALF;
            s.comb.scale = GX_CS_SCALE_2;
            break;
        case mapping::minus_half:
            s.comb.bias = GX_TB_SUBHALF;
            break;
        case mapping::negate:
            s.comb.d = z;
            break;
        }
        s.comb.out = t;
        steps.push_back(s);
        return operand_t{reg_sel(t, ch)};
    }

    /* The input's konst is in the way of the mapping's: raw first */
    std::optional<operand_t> load_twice(
        operand_t const& op, u16 map, channel_t ch, std::vector<step_t>& steps)
    {
        m_temps--;
        auto raw = load(op, mapping::clamp, ch, steps);
        if(!raw)
            return std::nullopt;
        return load(*raw, map, ch, steps);
    }

    /* A mapped input that a product can take directly, or in (1 - c) */
    struct factor_t
    {
        operand_t op;
        bool      inverted{false};
    };

    std::optional<factor_t> factor(
        u16 in, u16 map, channel_t ch, std::vector<step_t>& steps)
    {
        auto op = operand(in, ch);
        if(!op)
            return std::nullopt;
        if(map == mapping::clamp || map == mapping::passthrough)
            return factor_t{*op};
        if(map == mapping::invert)
            return factor_t{*op, true};
        auto loaded = load(*op, map, ch, steps);
        if(!loaded)
            return std::nullopt;
        return factor_t{*loaded};
    }

    bool is(operand_t const& op, u16 in, channel_t ch)
    {
        auto o = operand(in, ch);
        return o && o->sel == op.sel && o->konst == op.konst;
    }

    /* x * y as a, b, c, with d left free; zero products are nullopt */
    struct term_t
    {
        step_t step;
        bool   zero{false};
        u32    temps_from{0}; // temporaries its inputs took, free after it
        /* x * 1: just x, which fits another stage's d */
        std::optional<operand_t> single{};
    };

    std::optional<term_t> product(
        half_t const& h, u32 first, channel_t ch, std::vector<step_t>& steps)
    {
        u32 const  from = m_temps;
        auto       x    = factor(h.input[first], h.mapping[first], ch, steps);
        bool const same = h.input[first] == h.input[first + 1] &&
                          h.mapping[first] == h.mapping[first + 1];
        auto y =
            same ? x
                 : factor(h.input[first + 1], h.mapping[first + 1], ch, steps);
        if(!x || !y)
            return std::nullopt;
        auto term = [from](step_t const& s) {
            return term_t{.step = s, .temps_from = from};
        };
        u8 const z    = ch.zero();
        auto     zero = [&](factor_t const& f) {
            return !f.inverted && f.op.sel == z;
        };
        if(zero(*x) || zero(*y))
            return term_t{.zero = true, .temps_from = from};
        // Only one texture and one constant per stage
        if(!compatible(x->op, y->op))
        {
            auto loaded = load(
                y->op,
                y->inverted ? mapping::invert : mapping::clamp,
                ch,
                steps);
            if(!loaded)
                return std::nullopt;
            y = factor_t{*loaded};
        }
        if(x->inverted && y->inverted)
        {
            auto loaded = load(x->op, mapping::invert, ch, steps);
            if(!loaded)
                return std::nullopt;
            x = factor_t{*loaded};
        }
        if(x->inverted)
            std::swap(x, y);
        if(!y->inverted && is(y->op, input::one, ch))
            std::swap(x, y);
        if(!x->inverted && !y->inverted && is(x->op, input::one, ch))
        {
            auto t =
                term(step_of(z, y->op.sel, x->op.sel, z, {&x->op, &y->op}));
            t.single = y->op;
            return t;
        }
        // (1 - c) a + c b
        if(y->inverted)
            return term(step_of(x->op.sel, z, y->op.sel, z, {&x->op, &y->op}));
        return term(step_of(z, x->op.sel, y->op.sel, z, {&x->op, &y->op}));
    }

    static bool same(term_t const& a, term_t const& b)
    {
        auto const& x = a.step;
        auto const& y = b.step;
        return a.zero == b.zero && x.comb.a == y.comb.a &&
               x.comb.b == y.comb.b && x.comb.c == y.comb.c &&
               x.comb.d == y.comb.d && x.comb.op == y.comb.op &&
               x.tex == y.tex && x.konst == y.konst &&
               x.swap_blue == y.swap_blue;
    }

    static void output_mapping(combiner_t& c, u16 map)
    {
        switch(map)
        {
        case 1:
            c.scale = GX_CS_DIVIDE_2;
            break;
        case 2:
            c.scale = GX_CS_SCALE_2;
            break;
        case 3:
            c.scale = GX_CS_SCALE_4;
            break;
        case 4:
            c.bias = GX_TB_SUBHALF;
            break;
        case 5:
            c.bias  = GX_TB_SUBHALF;
            c.scale = GX_CS_SCALE_2;
            break;
        }
    }

    static std::optional<u8> destination(u16 out)
    {
        switch(out)
        {
        case output::scratch0:
            return GX_TEVPREV;
        case output::scratch1:
            return GX_TEVREG0;
        }
        return std::nullopt; // discarded, or a register we don't keep
    }

    /* `term` plus `d`, mapped, into `out` */
    static step_t finish(
        term_t const& term, u8 d, u16 map, u8 out, channel_t ch)
    {
        step_t s = term.zero
                       ? step_t{.comb = {ch.zero(), ch.zero(), ch.zero(), d}}
                       : term.step;
        if(!term.zero)
            s.comb.d = d;
        output_mapping(s.comb, map);
        s.comb.out = out;
        return s;
    }

    bool half(half_t const& h, channel_t ch, std::vector<step_t>& steps)
    {
        m_temps        = 0;
        m_alpha        = ch.alpha;
        auto const ab  = destination(h.ab);
        auto const cd  = destination(h.cd);
        auto const sum = destination(h.sum);
        if(!ab && !cd && !sum)
            return true;
        u8 const z = ch.zero();

        // A product into a register of its own, over its spent temporaries
        auto keep = [&](term_t const& p) -> std::optional<u8> {
            if(p.zero)
                return z;
            m_temps    = p.temps_from;
            u8 const t = temp(m_temps++);
            if(t == 0xFF)
                return std::nullopt;
            steps.push_back(finish(p, z, 0, t, ch));
            return reg_sel(t, ch);
        };

        if(sum && !ab && !cd && !h.mux)
        {
            auto p_ab = product(h, 0, ch, steps);
            if(!p_ab)
                return false;
            if(p_ab->zero)
            {
                auto p_cd = product(h, 2, ch, steps);
                if(!p_cd)
                    return false;
                steps.push_back(finish(*p_cd, z, h.output_mapping, *sum, ch));
                return true;
            }
            auto p_cd = product(h, 2, ch, steps);
            if(!p_cd)
                return false;
            // x + C*D in one stage when x needs no multiply
            for(auto [lone, other] :
                {std::pair{&*p_ab, &*p_cd}, std::pair{&*p_cd, &*p_ab}})
                if(lone->single && !other->zero)
                {
                    operand_t const& x = *lone->single;
                    step_t           s =
                        finish(*other, x.sel, h.output_mapping, *sum, ch);
                    operand_t const have{0, s.tex, s.konst, s.swap_blue};
                    if(!compatible(x, have))
                        continue;
                    merge(s, x);
                    steps.push_back(s);
                    return true;
                }
            auto const a = keep(*p_ab);
            if(!a)
                return false;
            steps.push_back(finish(*p_cd, *a, h.output_mapping, *sum, ch));
            return true;
        }

        // One product, written as it is
        if(!sum && !h.mux && (!ab || !cd))
        {
            auto p = product(h, ab ? 0 : 2, ch, steps);
            if(!p)
                return false;
            steps.push_back(
                finish(*p, z, h.output_mapping, ab ? *ab : *cd, ch));
            return true;
        }

        // Both products kept apart; zero ones need no register
        auto p_ab = product(h, 0, ch, steps);
        if(!p_ab)
            return false;
        if(sum && h.mux && !ab && !cd)
        {
            // Muxing a product with itself is that product
            std::vector<step_t> probe;
            u32 const           temps = m_temps;
            auto                p_cd  = product(h, 2, ch, probe);
            if(p_cd && probe.empty() && same(*p_ab, *p_cd))
            {
                steps.push_back(finish(*p_ab, z, h.output_mapping, *sum, ch));
                return true;
            }
            m_temps = temps;
        }
        auto const a = keep(*p_ab);
        if(!a)
            return false;
        auto p_cd = product(h, 2, ch, steps);
        if(!p_cd)
            return false;
        auto const c = keep(*p_cd);
        if(!c)
            return false;
        if(sum && h.mux && ch.alpha && p_ab->zero && h.output_mapping == 0)
        {
            // CD where scratch 0's alpha is over 1/2, else 0: a compare
            step_t s{.comb = {GX_CA_APREV, GX_CA_RASA, *c, z}};
            s.comb.op  = GX_TEV_COMP_A8_GT;
            s.comb.out = *sum;
            steps.push_back(s);
        } else if(sum && h.mux)
        {
            // CD where scratch 0's alpha is at least 1/2: a steep ramp
            u8 const t_m = temp(m_temps++);
            if(t_m == 0xFF)
                return false;
            step_t m{.comb = {reg_sel(GX_TEVPREV, ch, true), z, z, z}};
            m.comb.bias  = GX_TB_SUBHALF;
            m.comb.scale = GX_CS_SCALE_4;
            m.comb.out   = t_m;
            steps.push_back(m);
            step_t s{.comb = {*a, *c, reg_sel(t_m, ch), z}};
            output_mapping(s.comb, h.output_mapping);
            s.comb.out = *sum;
            steps.push_back(s);
        } else if(sum)
        {
            step_t s{.comb = {*a, z, z, *c}};
            output_mapping(s.comb, h.output_mapping);
            s.comb.out = *sum;
            steps.push_back(s);
        }
        for(auto [dest, src] : {std::pair{ab, *a}, std::pair{cd, *c}})
            if(dest)
            {
                step_t s{.comb = {src, z, z, z}};
                output_mapping(s.comb, h.output_mapping);
                s.comb.out = *dest;
                steps.push_back(s);
            }
        return true;
    }

    static combiner_t idle(channel_t ch)
    {
        u8 const z = ch.zero();
        return {z, z, z, static_cast<u8>(ch.alpha ? GX_CA_APREV : GX_CC_CPREV)};
    }

    stage_t tev_stage(
        combiner_t const& color,
        combiner_t const& alpha,
        i8                tex,
        std::optional<u8> kc,
        std::optional<u8> ka,
        bool              swap_blue)
    {
        stage_t s{
            .op             = stage_t::custom,
            .color          = gexxo::program_t::channel_t::color0a0,
            .color_combiner = color,
            .alpha_combiner = alpha};
        if(tex >= 0)
        {
            s.texcoord =
                static_cast<stage_t::texcoord_t>(stage_t::texcoord0 + tex);
            s.texmap = static_cast<stage_t::texmap_t>(stage_t::texmap0 + tex);
        }
        if(kc)
            s.konst_color = *kc;
        if(ka)
            s.konst_alpha = *ka;
        if(swap_blue)
            s.tex_swap = GX_TEV_SWAP2;
        return s;
    }

    /* Side by side, ending together so both channels write their results in
     * the stage's last TEV stage, after everything has read them */
    void pair(
        std::vector<step_t> const& color,
        std::vector<step_t> const& alpha,
        std::vector<stage_t>&      out)
    {
        std::vector<stage_t> rev;
        i32                  ci = static_cast<i32>(color.size()) - 1;
        i32                  ai = static_cast<i32>(alpha.size()) - 1;
        while(ci >= 0 || ai >= 0)
        {
            step_t const* c = ci >= 0 ? &color[ci] : nullptr;
            step_t const* a = ai >= 0 ? &alpha[ai] : nullptr;
            if(c && a &&
               !(c->tex < 0 || a->tex < 0 ||
                 (c->tex == a->tex && !c->swap_blue)))
            {
                // A texture each: the longer side goes first
                if(ci >= ai)
                    a = nullptr;
                else
                    c = nullptr;
            }
            i8 const tex = c && c->tex >= 0 ? c->tex : a ? a->tex : -1;
            rev.push_back(tev_stage(
                c ? c->comb : idle({false}),
                a ? a->comb : idle({true}),
                tex,
                c ? c->konst : std::nullopt,
                a ? a->konst : std::nullopt,
                a && a->swap_blue));
            if(c)
                ci--;
            if(a)
                ai--;
        }
        out.insert(out.end(), rev.rbegin(), rev.rend());
    }

    std::vector<stage_desc_t> const& m_stages;
    std::vector<konst_recipe_t>      m_konst;
    u32                              m_stage{0};
    u32                              m_temps{0};
    bool                             m_scratch1_rgb{false};
    bool                             m_scratch1_alpha{false};
    bool                             m_alpha{false}; // the channel at work
    char const*                      m_failure{"unsupported input"};
};

} // namespace detail

/* GX_TEV_SWAP2 must read blue into alpha; see load_swap_table() */
inline program_t compile(std::vector<stage_desc_t> const& stages)
{
    return detail::compiler_t(stages).run();
}

/* The draw's material colour: the raster alpha is the 1/2 alpha inputs read */
inline Vecf4 material_color()
{
    return {1.f, 1.f, 1.f, 0.5f};
}

inline void load_swap_table()
{
    GX_SetTevSwapModeTable(
        GX_TEV_SWAP2, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_BLUE);
}

} // namespace gx_sotr
