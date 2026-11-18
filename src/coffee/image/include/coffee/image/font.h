#pragma once

#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <span>
#include <string_view>

namespace image::font {

/*! Where text may go. Lines wrap at spaces (and between CJK characters) to
 *  fit `max_width`; a word wider than that breaks between characters; '\n'
 *  starts a new line. Past `max_lines` the rest is dropped and the last
 *  line kept ends in "..." (U+2026). Nothing is drawn wider than
 *  `max_width` */
struct text_bounds
{
    float max_width{std::numeric_limits<float>::infinity()};
    int   max_lines{0}; /*!< 0 for no limit */
};

/*! Text set at one size, in pixels. The bitmap holds all of its ink: the
 *  pen starts `origin_x` in from its left edge, the first line's baseline
 *  `baseline` down from its top, each next line `line_advance` lower */
struct text_metrics
{
    float advance{0.f}; /*!< the widest line's pen travel */
    float ascent{0.f};  /*!< the font's, above a baseline */
    float descent{0.f}; /*!< the font's, below it */
    int   width{0};
    int   height{0};
    int   origin_x{0};
    int   baseline{0};
    int   line_advance{0};
    int   lines{0};
    bool  truncated{false}; /*!< lines were dropped to keep to max_lines */

    bool empty() const
    {
        return width <= 0 || height <= 0;
    }
};

/*! Where coverage is written: one byte per pixel, 0-255, rows `stride`
 *  bytes apart */
struct coverage_target
{
    std::span<std::byte> pixels;
    std::size_t          stride{0};
};

/*! A face of a font file or collection */
struct font_file
{
    std::span<std::byte const> data;
    unsigned                   index{0};
};

/*! Text in a typeface, taken whole as UTF-8 so nothing splits a character.
 *  Sizes are pixels per em, as a "16 px" font size means elsewhere */
class typeface
{
  public:
    /*! Font files, each character drawn from the first that has it. Read in
     *  place: the data must outlive the typeface. The parser trusts the
     *  files, so only open fonts that ship with the game */
    static std::optional<typeface> open(std::span<font_file const> files);

    /*! The platform's own text rendering, by a CSS font-family list such as
     *  "sans-serif": its fonts, fallback and shaping. Font files given are
     *  copied in and tried after those families, once they have loaded (a
     *  moment later). Only where the platform draws text (the browser);
     *  none elsewhere */
    static std::optional<typeface> platform(std::string_view families, std::span<font_file const> fallbacks = {});

    typeface(typeface&&) noexcept;
    typeface& operator=(typeface&&) noexcept;
    ~typeface();

    /*! Invalid UTF-8 is set as U+FFFD; control characters other than '\n'
     *  are left out */
    text_metrics measure(std::string_view utf8, float pixels_per_em, text_bounds bounds = {}) const;

    /*! The text's coverage, as measure() lays it out, into the target's
     *  top-left width x height (cleared first); false if it does not fit */
    bool rasterize(std::string_view utf8, float pixels_per_em, coverage_target target, text_bounds bounds = {}) const;

    /*! The rasterizer behind it: stb_truetype, or the browser's canvas */
    struct backend;

  private:
    explicit typeface(std::unique_ptr<backend> impl);

    std::unique_ptr<backend> m_impl;
};

} // namespace image::font
