#pragma once

#include <optional>
#include <string>

/* Text input through the browser's EditContext on the canvas, which carries
 * input method compositions that key events never see. Chromium only */
namespace sdl3::web_text_input {

struct event
{
    bool        composing{false}; /*!< else written */
    std::string text;
    int         cursor{-1}; /*!< in the composition, in code points */
};

/*! False where the browser has no EditContext */
bool start();
void stop();

/*! Where the text is, in canvas pixels, for the candidate list */
void area(int x, int y, int w, int h, int cursor);

std::optional<event> next();

} // namespace sdl3::web_text_input
