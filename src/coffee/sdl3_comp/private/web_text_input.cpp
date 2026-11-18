#include "web_text_input.h"

#include <emscripten.h>

/* The EditContext's text only ever holds the composition: whatever is
 * written is queued and cleared */
EM_JS(int, coffee_ime_start, (), {
    var canvas = Module['canvas'];
    if(!canvas || typeof EditContext === 'undefined')
        return 0;
    var ime = Module.coffeeIme;
    if(!ime)
    {
        var ec = new EditContext();
        ime = Module.coffeeIme = {ec: ec, queue: [], composing: false, shown: "", caret: null};
        ime.clear = function() {
            ec.updateText(0, ec.text.length, "");
            ec.updateSelection(0, 0);
        };
        ime.show = function(text, caret) {
            if(text !== ime.shown)
                ime.queue.push([1, text, caret]);
            ime.shown = text;
        };
        var points = function(at) { return Array.from(ec.text.substring(0, at)).length; };
        ec.addEventListener('textupdate', function(e) {
            if(ime.composing)
                return ime.show(ec.text, points(e.selectionStart));
            if(ec.text.length)
                ime.queue.push([0, ec.text, -1]);
            ime.clear();
        });
        ec.addEventListener('compositionstart', function() { ime.composing = true; });
        ec.addEventListener('compositionend', function() {
            ime.composing = false;
            ime.show("", -1);
            if(ec.text.length)
                ime.queue.push([0, ec.text, -1]);
            ime.clear();
        });
        /* SDL cancels keypress while writing, which also cancels the text
         * reaching the EditContext: keep it from SDL while ours is on */
        window.addEventListener('keypress', function(e) {
            var canvas = Module['canvas'];
            if(canvas && canvas.editContext === ec)
                e.stopImmediatePropagation();
        }, true);
        ec.addEventListener('characterboundsupdate', function(e) {
            if(!ime.caret)
                return;
            var rects = [];
            for(var i = e.rangeStart; i < e.rangeEnd; i++)
                rects.push(ime.caret);
            ec.updateCharacterBounds(e.rangeStart, rects);
        });
    }
    if(!canvas.hasAttribute('tabindex'))
    {
        canvas.tabIndex = -1;
        ime.ownTabIndex = true;
    }
    canvas.editContext = ime.ec;
    canvas.focus();
    return 1;
});

EM_JS(void, coffee_ime_stop, (), {
    var canvas = Module['canvas'];
    var ime = Module.coffeeIme;
    if(!ime)
        return;
    if(canvas && canvas.editContext === ime.ec)
        canvas.editContext = null;
    if(canvas && ime.ownTabIndex)
        canvas.removeAttribute('tabindex');
    ime.ownTabIndex = false;
    ime.composing = false;
    ime.shown = "";
    ime.queue.length = 0;
    ime.clear();
});

EM_JS(void, coffee_ime_area, (int x, int y, int w, int h, int cursor), {
    var canvas = Module['canvas'];
    var ime = Module.coffeeIme;
    if(!ime || !canvas || !canvas.width || !canvas.height)
        return;
    var r = canvas.getBoundingClientRect();
    var sx = r.width / canvas.width;
    var sy = r.height / canvas.height;
    ime.ec.updateControlBounds(new DOMRect(r.left + x * sx, r.top + y * sy, w * sx, h * sy));
    ime.caret = new DOMRect(r.left + (x + cursor) * sx, r.top + y * sy, 0, h * sy);
    ime.ec.updateSelectionBounds(ime.caret);
});

EM_JS(int, coffee_ime_peek, (), {
    var ime = Module.coffeeIme;
    if(!ime || !ime.queue.length)
        return -1;
    return lengthBytesUTF8(ime.queue[0][1]);
});

EM_JS(void, coffee_ime_take, (char* text, int size, int* kind, int* cursor), {
    var e = Module.coffeeIme.queue.shift();
    stringToUTF8(e[1], Number(text), size);
    HEAP32[Number(kind) / 4] = e[0];
    HEAP32[Number(cursor) / 4] = e[2];
});

EM_JS_DEPS(coffee_ime, "$stringToUTF8,$lengthBytesUTF8");

namespace sdl3::web_text_input {

bool start()
{
    return coffee_ime_start() != 0;
}

void stop()
{
    coffee_ime_stop();
}

void area(int x, int y, int w, int h, int cursor)
{
    coffee_ime_area(x, y, w, h, cursor);
}

std::optional<event> next()
{
    int const size = coffee_ime_peek();
    if(size < 0)
        return std::nullopt;
    std::string text(static_cast<std::size_t>(size) + 1, '\0');
    int         kind = 0, cursor = -1;
    coffee_ime_take(text.data(), size + 1, &kind, &cursor);
    text.resize(static_cast<std::size_t>(size));
    return event{kind == 1, std::move(text), cursor};
}

} // namespace sdl3::web_text_input
