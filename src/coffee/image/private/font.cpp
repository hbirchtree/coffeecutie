#include <coffee/image/font.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

/* Private copy: Dear ImGui links its own stb_truetype */
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

namespace image::font {

namespace {

constexpr float none = std::numeric_limits<float>::infinity();

/* One line as set: the font's vertical metrics, how far the pen travels,
 * and its ink from the pen's start on the baseline (above is positive);
 * the ink is +-inf where nothing is drawn */
struct line_ink
{
    float advance{0.f};
    float ascent{0.f};
    float descent{0.f};
    float gap{0.f};
    float left{none}, right{-none}, top{-none}, bottom{-none};

    void add(float l, float r, float t, float b)
    {
        left   = std::min(left, l);
        right  = std::max(right, r);
        top    = std::max(top, t);
        bottom = std::max(bottom, b);
    }
};

} // namespace

struct typeface::backend
{
    virtual ~backend() = default;

    virtual line_ink measure(std::u32string_view line, float pixels_per_em) const = 0;

    /* The lines, each `at.line_advance` under the last, the pen at
     * `at.origin_x`; into a target already cleared to at's size */
    virtual void rasterize(
        std::span<std::u32string const> lines, float pixels_per_em, text_metrics const& at,
        coverage_target target) const = 0;
};

namespace {

constexpr char32_t replacement = U'�';
constexpr char32_t ellipsis    = U'…';

/* The next code point of `text`, consumed; a malformed sequence gives one
 * U+FFFD for its valid start, as browsers decode it */
char32_t next_code_point(std::string_view& text)
{
    auto const byte = [&text](std::size_t i) { return static_cast<unsigned char>(text[i]); };
    unsigned char const lead = byte(0);
    std::size_t         length;
    char32_t            point, least;
    if(lead < 0x80)
        length = 1, point = lead, least = 0;
    else if((lead & 0xE0) == 0xC0)
        length = 2, point = lead & 0x1F, least = 0x80;
    else if((lead & 0xF0) == 0xE0)
        length = 3, point = lead & 0x0F, least = 0x800;
    else if((lead & 0xF8) == 0xF0)
        length = 4, point = lead & 0x07, least = 0x10000;
    else
        length = 0, point = 0, least = 0;
    auto const malformed = [&text](std::size_t consumed) {
        text.remove_prefix(std::max<std::size_t>(consumed, 1));
        return replacement;
    };
    if(length == 0)
        return malformed(1);
    for(std::size_t i = 1; i < length; i++)
    {
        if(i >= text.size() || (byte(i) & 0xC0) != 0x80)
            return malformed(i);
        point = (point << 6) | (byte(i) & 0x3F);
    }
    if(point < least || point > 0x10FFFF || (point >= 0xD800 && point <= 0xDFFF))
        return malformed(1);
    text.remove_prefix(length);
    return point;
}

/* The characters: "\r\n" and '\r' become '\n', tabs spaces, other control
 * characters draw nothing */
std::u32string decode(std::string_view utf8)
{
    std::u32string out;
    out.reserve(utf8.size());
    char32_t previous = 0;
    while(!utf8.empty())
    {
        char32_t point = next_code_point(utf8);
        if(point == U'\n' && previous == U'\r')
        {
            previous = point;
            continue;
        }
        previous = point;
        if(point == U'\r')
            point = U'\n';
        else if(point == U'\t')
            point = U' ';
        if(point == U'\n' || (point >= 0x20 && (point < 0x7F || point > 0x9F)))
            out.push_back(point);
    }
    return out;
}

/* Belongs to the character before it: combining marks, variation
 * selectors, joiners and what they join, skin tones, Hangul jamo vowels
 * and finals */
bool attached(char32_t point, char32_t before)
{
    return (point >= 0x0300 && point <= 0x036F) || (point >= 0x1AB0 && point <= 0x1AFF) ||
           (point >= 0x1DC0 && point <= 0x1DFF) || (point >= 0x20D0 && point <= 0x20FF) ||
           (point >= 0xFE20 && point <= 0xFE2F) || (point >= 0xFE00 && point <= 0xFE0F) ||
           (point >= 0xE0100 && point <= 0xE01EF) || (point >= 0x1F3FB && point <= 0x1F3FF) ||
           (point >= 0x1160 && point <= 0x11FF) || (point >= 0xD7B0 && point <= 0xD7FF) || point == 0x200D ||
           before == 0x200D;
}

/* Written without spaces between words: ideographs and kana. Korean has
 * spaces, so Hangul is not */
bool cjk(char32_t point)
{
    return (point >= 0x2E80 && point <= 0x9FFF && !(point >= 0x3130 && point <= 0x318F)) ||
           (point >= 0xF900 && point <= 0xFAFF) || (point >= 0xFF00 && point <= 0xFF60) ||
           (point >= 0x20000 && point <= 0x3FFFF);
}

/* A line may end before `text[i]` */
bool breakable(std::u32string_view text, std::size_t i)
{
    if(i == 0 || i >= text.size())
        return true;
    if(attached(text[i], text[i - 1]))
        return false;
    return text[i - 1] == U' ' || cjk(text[i - 1]) || cjk(text[i]);
}

/* Between characters, for a word too wide to keep whole */
bool boundary(std::u32string_view text, std::size_t i)
{
    return i == 0 || i >= text.size() || !attached(text[i], text[i - 1]);
}

std::u32string_view trimmed(std::u32string_view text)
{
    while(!text.empty() && text.back() == U' ')
        text.remove_suffix(1);
    return text;
}

struct layout
{
    std::vector<std::u32string> lines;
    text_metrics                metrics;
};

/* Lines to fit the bounds, greedily: as many whole words as fit, else as
 * many characters */
layout lay_out(typeface::backend const& face, std::u32string_view text, float pixels_per_em, text_bounds bounds)
{
    layout out;
    auto const fits = [&](std::u32string_view line) {
        return !std::isfinite(bounds.max_width) || face.measure(trimmed(line), pixels_per_em).advance <= bounds.max_width;
    };
    auto const full = [&] { return bounds.max_lines > 0 && out.lines.size() >= static_cast<std::size_t>(bounds.max_lines); };

    std::size_t start = 0;
    while(start <= text.size() && !text.empty())
    {
        std::size_t const      newline   = text.find(U'\n', start);
        std::u32string_view    paragraph = text.substr(start, newline == std::u32string_view::npos ? text.npos : newline - start);
        do
        {
            if(full())
            {
                out.metrics.truncated = true;
                break;
            }
            std::size_t end = 0;
            for(std::size_t i = 1; i <= paragraph.size(); i++)
                if(breakable(paragraph, i))
                {
                    if(!fits(paragraph.substr(0, i)))
                        break;
                    end = i;
                }
            if(end == 0)
            {
                end = paragraph.empty() ? 0 : 1;
                while(end < paragraph.size() && !boundary(paragraph, end))
                    end++;
                for(std::size_t i = end + 1; i <= paragraph.size(); i++)
                    if(boundary(paragraph, i))
                    {
                        if(!fits(paragraph.substr(0, i)))
                            break;
                        end = i;
                    }
            }
            out.lines.emplace_back(trimmed(paragraph.substr(0, end)));
            paragraph.remove_prefix(end);
            while(!paragraph.empty() && paragraph.front() == U' ')
                paragraph.remove_prefix(1);
        } while(!paragraph.empty());
        if(out.metrics.truncated || newline == std::u32string_view::npos)
            break;
        start = newline + 1;
    }
    if(out.metrics.truncated && !out.lines.empty())
    {
        auto& last = out.lines.back();
        while(!last.empty() && !fits(last + ellipsis))
        {
            last.pop_back();
            while(!last.empty() && !boundary(last, last.size()))
                last.pop_back();
            while(!last.empty() && last.back() == U' ')
                last.pop_back();
        }
        last += ellipsis;
    }
    if(out.lines.empty())
        return out;

    /* Each line's ink together, the first's top and the last's bottom
     * hanging out of the line spacing */
    std::vector<line_ink> inks;
    for(auto const& line : out.lines)
        inks.push_back(face.measure(line, pixels_per_em));
    auto const& first = inks.front();
    auto&       m     = out.metrics;
    m.ascent          = first.ascent;
    m.descent         = first.descent;
    m.lines           = static_cast<int>(out.lines.size());
    m.line_advance    = static_cast<int>(std::ceil(first.ascent + first.descent + first.gap));
    float left = 0.f, right = 0.f;
    for(auto const& ink : inks)
    {
        m.advance = std::max(m.advance, ink.advance);
        right     = std::max({right, ink.advance, std::isfinite(ink.right) ? ink.right : 0.f});
        if(std::isfinite(ink.left))
            left = std::min(left, ink.left);
    }
    float const top    = std::isfinite(first.top) ? first.top : 0.f;
    float const bottom = std::isfinite(inks.back().bottom) ? inks.back().bottom : 0.f;
    m.origin_x         = static_cast<int>(std::ceil(-left));
    m.baseline         = static_cast<int>(std::ceil(std::max(m.ascent, top)));
    m.height = m.baseline + (m.lines - 1) * m.line_advance + static_cast<int>(std::ceil(std::max(m.descent, bottom)));
    m.width  = m.origin_x + static_cast<int>(std::ceil(right));
    if(std::isfinite(bounds.max_width))
        m.width = std::min(m.width, m.origin_x + static_cast<int>(std::ceil(std::max(bounds.max_width, 0.f))));
    return out;
}

bool fits(text_metrics const& at, coverage_target const& target)
{
    if(at.empty())
        return true;
    auto const width  = static_cast<std::size_t>(at.width);
    auto const height = static_cast<std::size_t>(at.height);
    return target.stride >= width && target.stride <= std::numeric_limits<int>::max() &&
           target.pixels.size() >= target.stride * (height - 1) + width;
}

void clear(text_metrics const& at, coverage_target const& target)
{
    for(int row = 0; row < at.height; row++)
        std::fill_n(target.pixels.begin() + row * target.stride, at.width, std::byte{0});
}

class stb_backend final : public typeface::backend
{
  public:
    explicit stb_backend(std::vector<stbtt_fontinfo> faces)
        : m_faces(std::move(faces))
    {
    }

    line_ink measure(std::u32string_view line, float pixels_per_em) const override
    {
        return place(line, pixels_per_em, nullptr);
    }

    void rasterize(
        std::span<std::u32string const> lines, float pixels_per_em, text_metrics const& at,
        coverage_target target) const override
    {
        std::vector<placed>        glyphs;
        std::vector<unsigned char> scratch;
        for(std::size_t row = 0; row < lines.size(); row++)
        {
            glyphs.clear();
            place(lines[row], pixels_per_em, &glyphs);
            int const baseline = at.baseline + static_cast<int>(row) * at.line_advance;
            for(auto const& glyph : glyphs)
            {
                int const width  = glyph.x1 - glyph.x0;
                int const height = glyph.y1 - glyph.y0;
                if(width <= 0 || height <= 0)
                    continue;
                scratch.assign(static_cast<std::size_t>(width) * height, 0);
                stbtt_MakeGlyphBitmapSubpixel(
                    glyph.face, scratch.data(), width, height, width, glyph.scale, glyph.scale, glyph.shift, 0.f,
                    glyph.index);
                /* Neighbours can overlap: keep the stronger coverage */
                int const left = at.origin_x + glyph.pen + glyph.x0;
                int const top  = baseline + glyph.y0;
                for(int y = 0; y < height; y++)
                    for(int x = 0; x < width; x++)
                    {
                        int const tx = left + x, ty = top + y;
                        if(tx < 0 || ty < 0 || tx >= at.width || ty >= at.height)
                            continue;
                        auto& texel = target.pixels[ty * target.stride + tx];
                        texel       = std::max(texel, static_cast<std::byte>(scratch[y * width + x]));
                    }
            }
        }
    }

  private:
    struct placed
    {
        stbtt_fontinfo const* face{nullptr};
        int                   index{0};
        float                 scale{0.f};
        float                 shift{0.f}; /*!< sub-pixel part of the pen */
        int                   pen{0};     /*!< whole pixels of the pen */
        int                   x0{0}, y0{0}, x1{0}, y1{0};
    };

    /* The first face that has it, else the first face's replacement
     * character or its missing-glyph box */
    std::pair<stbtt_fontinfo const*, int> find(char32_t point) const
    {
        for(auto const& face : m_faces)
            if(int const index = stbtt_FindGlyphIndex(&face, static_cast<int>(point)))
                return {&face, index};
        return {&m_faces.front(), stbtt_FindGlyphIndex(&m_faces.front(), static_cast<int>(replacement))};
    }

    line_ink place(std::u32string_view line, float pixels_per_em, std::vector<placed>* out) const
    {
        auto const& primary = m_faces.front();
        float const scale   = stbtt_ScaleForMappingEmToPixels(&primary, pixels_per_em);
        int         ascent = 0, descent = 0, gap = 0;
        stbtt_GetFontVMetrics(&primary, &ascent, &descent, &gap);
        line_ink ink{.ascent = ascent * scale, .descent = -descent * scale, .gap = gap * scale};

        float                                 pen = 0.f;
        std::pair<stbtt_fontinfo const*, int> previous{nullptr, 0};
        for(char32_t point : line)
        {
            auto const [face, index] = find(point);
            float const face_scale   = stbtt_ScaleForMappingEmToPixels(face, pixels_per_em);
            if(previous.first == face)
                pen += stbtt_GetGlyphKernAdvance(face, previous.second, index) * face_scale;
            previous = {face, index};

            placed glyph{.face = face, .index = index, .scale = face_scale};
            glyph.pen   = static_cast<int>(std::floor(pen));
            glyph.shift = pen - glyph.pen;
            stbtt_GetGlyphBitmapBoxSubpixel(
                face, index, face_scale, face_scale, glyph.shift, 0.f, &glyph.x0, &glyph.y0, &glyph.x1, &glyph.y1);
            if(glyph.x1 > glyph.x0 && glyph.y1 > glyph.y0)
                ink.add(glyph.pen + glyph.x0, glyph.pen + glyph.x1, -glyph.y0, glyph.y1);
            int advance = 0, bearing = 0;
            stbtt_GetGlyphHMetrics(face, index, &advance, &bearing);
            pen += advance * face_scale;
            if(out)
                out->push_back(glyph);
        }
        ink.advance = pen;
        return ink;
    }

    std::vector<stbtt_fontinfo> m_faces;
};

#if defined(__EMSCRIPTEN__)

/* One OffscreenCanvas per thread; no DOM */
EM_JS(int, coffee_font_canvas_available, (), { return typeof OffscreenCanvas != 'undefined' ? 1 : 0; });

/* advance, font ascent and descent, ink left (of the pen), right, above and below */
EM_JS(void, coffee_font_canvas_measure,
      (char const* family, int family_length, char const* text, int text_length, float pixels, float* out), {
          if(!globalThis.coffeeFontCanvas)
              globalThis.coffeeFontCanvas = new OffscreenCanvas(1, 1).getContext('2d', {willReadFrequently : true});
          const context = globalThis.coffeeFontCanvas;
          context.font  = pixels + 'px ' + UTF8ToString(Number(family), family_length);
          const m       = context.measureText(UTF8ToString(Number(text), text_length));
          const font    = m.fontBoundingBoxAscent !== undefined;
          HEAPF32.set(
              [
                  m.width,
                  font ? m.fontBoundingBoxAscent : m.actualBoundingBoxAscent,
                  font ? m.fontBoundingBoxDescent : m.actualBoundingBoxDescent,
                  m.actualBoundingBoxLeft,
                  m.actualBoundingBoxRight,
                  m.actualBoundingBoxAscent,
                  m.actualBoundingBoxDescent,
              ],
              Number(out) / 4);
      });

/* Lines split on '\n', one under the other */
EM_JS(void, coffee_font_canvas_draw,
      (char const* family, int family_length, char const* text, int text_length, float pixels, int origin_x,
       int baseline, int line_advance, int width, int height, unsigned char* out, int stride), {
          if(!globalThis.coffeeFontCanvas)
              globalThis.coffeeFontCanvas = new OffscreenCanvas(1, 1).getContext('2d', {willReadFrequently : true});
          const context = globalThis.coffeeFontCanvas;
          /* Resizing clears the canvas and resets its state */
          context.canvas.width  = width;
          context.canvas.height = height;
          context.font          = pixels + 'px ' + UTF8ToString(Number(family), family_length);
          context.textBaseline  = 'alphabetic';
          context.fillStyle     = '#fff';
          const lines           = UTF8ToString(Number(text), text_length).split('\n');
          for(let i = 0; i < lines.length; i++)
              context.fillText(lines[i], origin_x, baseline + i * line_advance);
          const rgba = context.getImageData(0, 0, width, height).data;
          const base = Number(out);
          for(let y = 0; y < height; y++)
              for(let x = 0; x < width; x++)
                  HEAPU8[base + y * stride + x] = rgba[(y * width + x) * 4 + 3];
      });

/* A font file as a FontFace of that name, copied out of the heap; it is
 * used once it has loaded */
EM_JS(int, coffee_font_canvas_add, (unsigned char const* data, int length, char const* name, int name_length), {
    const fonts = globalThis.fonts || (globalThis.document && globalThis.document.fonts);
    if(!fonts || typeof FontFace == 'undefined')
        return 0;
    const start = Number(data);
    const face  = new FontFace(UTF8ToString(Number(name), name_length), HEAPU8.slice(start, start + length));
    fonts.add(face);
    face.load().catch(function() {});
    return 1;
});

EM_JS_DEPS(coffee_font_js_deps, "$UTF8ToString");

/* The cleaned characters as UTF-8 again, for the browser */
void encode(std::u32string_view text, std::string& out)
{
    for(char32_t point : text)
        if(point < 0x80)
            out += static_cast<char>(point);
        else if(point < 0x800)
            out += {static_cast<char>(0xC0 | point >> 6), static_cast<char>(0x80 | (point & 0x3F))};
        else if(point < 0x10000)
            out += {static_cast<char>(0xE0 | point >> 12), static_cast<char>(0x80 | (point >> 6 & 0x3F)),
                    static_cast<char>(0x80 | (point & 0x3F))};
        else
            out += {static_cast<char>(0xF0 | point >> 18), static_cast<char>(0x80 | (point >> 12 & 0x3F)),
                    static_cast<char>(0x80 | (point >> 6 & 0x3F)), static_cast<char>(0x80 | (point & 0x3F))};
}

class canvas_backend final : public typeface::backend
{
  public:
    explicit canvas_backend(std::string families)
        : m_families(std::move(families))
    {
    }

    line_ink measure(std::u32string_view line, float pixels_per_em) const override
    {
        std::string utf8;
        encode(line, utf8);
        float m[7]{};
        coffee_font_canvas_measure(
            m_families.data(), static_cast<int>(m_families.size()), utf8.data(), static_cast<int>(utf8.size()),
            pixels_per_em, m);
        line_ink ink{.advance = m[0], .ascent = m[1], .descent = m[2]};
        if(!line.empty())
            ink.add(-m[3], m[4], m[5], m[6]);
        return ink;
    }

    void rasterize(
        std::span<std::u32string const> lines, float pixels_per_em, text_metrics const& at,
        coverage_target target) const override
    {
        std::string utf8;
        for(auto const& line : lines)
        {
            if(&line != &lines.front())
                utf8 += '\n';
            encode(line, utf8);
        }
        coffee_font_canvas_draw(
            m_families.data(), static_cast<int>(m_families.size()), utf8.data(), static_cast<int>(utf8.size()),
            pixels_per_em, at.origin_x, at.baseline, at.line_advance, at.width, at.height,
            reinterpret_cast<unsigned char*>(target.pixels.data()), static_cast<int>(target.stride));
    }

  private:
    std::string m_families;
};

#endif

} // namespace

std::optional<typeface> typeface::open(std::span<font_file const> files)
{
    std::vector<stbtt_fontinfo> faces;
    for(auto const& file : files)
    {
        auto const* bytes = reinterpret_cast<unsigned char const*>(file.data.data());
        /* stb checks a file's tag, not its length */
        if(file.data.size() < 12 || file.data.size() > std::numeric_limits<int>::max())
            return std::nullopt;
        int const count = stbtt_GetNumberOfFonts(bytes);
        if(count <= 0 || file.index >= static_cast<unsigned>(count))
            return std::nullopt;
        int const offset = stbtt_GetFontOffsetForIndex(bytes, static_cast<int>(file.index));
        if(offset < 0 || !stbtt_InitFont(&faces.emplace_back(), bytes, offset))
            return std::nullopt;
    }
    if(faces.empty())
        return std::nullopt;
    return typeface(std::make_unique<stb_backend>(std::move(faces)));
}

std::optional<typeface> typeface::platform(
    [[maybe_unused]] std::string_view families, [[maybe_unused]] std::span<font_file const> fallbacks)
{
#if defined(__EMSCRIPTEN__)
    if(families.empty() || !coffee_font_canvas_available())
        return std::nullopt;
    static int  added = 0;
    std::string list(families);
    for(auto const& file : fallbacks)
    {
        std::string const name = "coffee-font-" + std::to_string(added++);
        if(file.index == 0 && file.data.size() <= std::numeric_limits<int>::max() &&
           coffee_font_canvas_add(
               reinterpret_cast<unsigned char const*>(file.data.data()), static_cast<int>(file.data.size()),
               name.data(), static_cast<int>(name.size())))
            list += ", '" + name + "'";
    }
    return typeface(std::make_unique<canvas_backend>(std::move(list)));
#else
    return std::nullopt;
#endif
}

typeface::typeface(std::unique_ptr<backend> impl)
    : m_impl(std::move(impl))
{
}

typeface::typeface(typeface&&) noexcept            = default;
typeface& typeface::operator=(typeface&&) noexcept = default;
typeface::~typeface()                              = default;

text_metrics typeface::measure(std::string_view utf8, float pixels_per_em, text_bounds bounds) const
{
    return lay_out(*m_impl, decode(utf8), pixels_per_em, bounds).metrics;
}

bool typeface::rasterize(std::string_view utf8, float pixels_per_em, coverage_target target, text_bounds bounds) const
{
    auto const set = lay_out(*m_impl, decode(utf8), pixels_per_em, bounds);
    if(!fits(set.metrics, target))
        return false;
    if(set.metrics.empty())
        return true;
    clear(set.metrics, target);
    m_impl->rasterize(set.lines, pixels_per_em, set.metrics, target);
    return true;
}

} // namespace image::font
