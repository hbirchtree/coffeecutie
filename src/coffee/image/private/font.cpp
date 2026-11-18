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

struct typeface::backend
{
    virtual ~backend() = default;

    virtual text_metrics measure(std::u32string_view text, float pixels_per_em) const = 0;
    virtual bool rasterize(
        std::u32string_view text, float pixels_per_em, text_metrics const& at, coverage_target target) const = 0;
};

namespace {

constexpr char32_t replacement = U'�';

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

/* One line's characters: control characters draw nothing */
std::u32string decode(std::string_view utf8)
{
    std::u32string out;
    out.reserve(utf8.size());
    while(!utf8.empty())
        if(char32_t const point = next_code_point(utf8); point >= 0x20 && (point < 0x7F || point > 0x9F))
            out.push_back(point);
    return out;
}

/* Bitmap bounds around the pen's travel and the ink, both from the pen's
 * start on the baseline; ink above it is positive */
text_metrics frame(float advance, float ascent, float descent, float ink_left, float ink_right, float ink_top, float ink_bottom)
{
    text_metrics out{.advance = advance, .ascent = ascent, .descent = descent};
    out.origin_x = static_cast<int>(std::ceil(std::max(0.f, -ink_left)));
    out.baseline = static_cast<int>(std::ceil(std::max(ascent, ink_top)));
    out.width    = out.origin_x + static_cast<int>(std::ceil(std::max(advance, ink_right)));
    out.height   = out.baseline + static_cast<int>(std::ceil(std::max(descent, ink_bottom)));
    return out;
}

bool fits(text_metrics const& at, coverage_target const& target)
{
    if(at.empty())
        return true;
    auto const width  = static_cast<std::size_t>(at.width);
    auto const height = static_cast<std::size_t>(at.height);
    return target.stride >= width && target.pixels.size() >= target.stride * (height - 1) + width;
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

    text_metrics measure(std::u32string_view text, float pixels_per_em) const override
    {
        return lay_out(text, pixels_per_em, nullptr);
    }

    bool rasterize(
        std::u32string_view text, float pixels_per_em, text_metrics const& at, coverage_target target) const override
    {
        std::vector<placed> glyphs;
        lay_out(text, pixels_per_em, &glyphs);
        std::vector<unsigned char> scratch;
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
            int const top  = at.baseline + glyph.y0;
            for(int y = 0; y < height; y++)
                for(int x = 0; x < width; x++)
                {
                    int const tx = left + x, ty = top + y;
                    if(tx < 0 || ty < 0 || tx >= at.width || ty >= at.height)
                        continue;
                    auto& texel = target.pixels[ty * target.stride + tx];
                    texel = std::max(texel, static_cast<std::byte>(scratch[y * width + x]));
                }
        }
        return true;
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

    text_metrics lay_out(std::u32string_view text, float pixels_per_em, std::vector<placed>* out) const
    {
        auto const& primary = m_faces.front();
        float const scale   = stbtt_ScaleForMappingEmToPixels(&primary, pixels_per_em);
        int         ascent = 0, descent = 0, gap = 0;
        stbtt_GetFontVMetrics(&primary, &ascent, &descent, &gap);

        constexpr float none = std::numeric_limits<float>::infinity();
        float pen = 0.f, ink_left = none, ink_right = -none, ink_top = -none, ink_bottom = -none;
        std::pair<stbtt_fontinfo const*, int> previous{nullptr, 0};
        for(char32_t point : text)
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
            {
                ink_left   = std::min(ink_left, static_cast<float>(glyph.pen + glyph.x0));
                ink_right  = std::max(ink_right, static_cast<float>(glyph.pen + glyph.x1));
                ink_top    = std::max(ink_top, static_cast<float>(-glyph.y0));
                ink_bottom = std::max(ink_bottom, static_cast<float>(glyph.y1));
            }
            int advance = 0, bearing = 0;
            stbtt_GetGlyphHMetrics(face, index, &advance, &bearing);
            pen += advance * face_scale;
            if(out)
                out->push_back(glyph);
        }
        if(ink_left == none)
            ink_left = ink_right = ink_top = ink_bottom = 0.f;
        return frame(pen, ascent * scale, -descent * scale, ink_left, ink_right, ink_top, ink_bottom);
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

EM_JS(void, coffee_font_canvas_draw,
      (char const* family, int family_length, char const* text, int text_length, float pixels, int origin_x,
       int baseline, int width, int height, unsigned char* out, int stride), {
          if(!globalThis.coffeeFontCanvas)
              globalThis.coffeeFontCanvas = new OffscreenCanvas(1, 1).getContext('2d', {willReadFrequently : true});
          const context = globalThis.coffeeFontCanvas;
          /* Resizing clears the canvas and resets its state */
          context.canvas.width  = width;
          context.canvas.height = height;
          context.font          = pixels + 'px ' + UTF8ToString(Number(family), family_length);
          context.textBaseline  = 'alphabetic';
          context.fillStyle     = '#fff';
          context.fillText(UTF8ToString(Number(text), text_length), origin_x, baseline);
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
std::string encode(std::u32string_view text)
{
    std::string out;
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
    return out;
}

class canvas_backend final : public typeface::backend
{
  public:
    explicit canvas_backend(std::string families)
        : m_families(std::move(families))
    {
    }

    text_metrics measure(std::u32string_view text, float pixels_per_em) const override
    {
        if(text.empty())
            return {};
        std::string const utf8 = encode(text);
        float             m[7]{};
        coffee_font_canvas_measure(
            m_families.data(), static_cast<int>(m_families.size()), utf8.data(), static_cast<int>(utf8.size()),
            pixels_per_em, m);
        return frame(m[0], m[1], m[2], -m[3], m[4], m[5], m[6]);
    }

    bool rasterize(
        std::u32string_view text, float pixels_per_em, text_metrics const& at, coverage_target target) const override
    {
        std::string const utf8 = encode(text);
        coffee_font_canvas_draw(
            m_families.data(), static_cast<int>(m_families.size()), utf8.data(), static_cast<int>(utf8.size()),
            pixels_per_em, at.origin_x, at.baseline, at.width, at.height,
            reinterpret_cast<unsigned char*>(target.pixels.data()), static_cast<int>(target.stride));
        return true;
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

text_metrics typeface::measure(std::string_view utf8, float pixels_per_em) const
{
    return m_impl->measure(decode(utf8), pixels_per_em);
}

bool typeface::rasterize(std::string_view utf8, float pixels_per_em, coverage_target target) const
{
    std::u32string const text = decode(utf8);
    text_metrics const   at   = m_impl->measure(text, pixels_per_em);
    if(!fits(at, target) || target.stride > std::numeric_limits<int>::max())
        return false;
    if(at.empty())
        return true;
    clear(at, target);
    return m_impl->rasterize(text, pixels_per_em, at, target);
}

} // namespace image::font
