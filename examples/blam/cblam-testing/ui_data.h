#pragma once

#include <blam/volta/blam_ui.h>
#include <coffee/components/types.h>

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

/* What a provider's function did with a handler's run_function */
enum class ui_result_t
{
    ok,
    failed,  /*!< takes the tag's try_to_branch_on_failure path */
    pending, /*!< finish later with UIFunctionDone{token} on the UIEventBus */
};

struct UIFunctionCall
{
    using event_t = blam::ui_element::event_handler_t::type_t;

    libc_types::u16         function{};
    event_t                 event{};
    libc_types::u32         seat{};
    blam::ui_element const* widget{nullptr}; /*!< whose handler ran */
    /* Focused item of the nearest list, e.g. the selected map or difficulty */
    std::optional<libc_types::u16> selected;
    /* Keyboard input, for the name-change functions */
    std::u16string_view text;
    libc_types::u64     token{};
};

/* Where the UI gets game data from. Providers (profiles, settings, maps)
 * register against the hooks the tags already have: run_function ids, and
 * widget tag names (the path's last part) for spinners and engine text. */
struct UIDataSource : compo::SubsystemBase
{
    using type = UIDataSource;

    using function_t = std::function<ui_result_t(UIFunctionCall const&)>;
    using getter_t   = std::function<libc_types::u16()>;
    using setter_t   = std::function<void(libc_types::u16)>;
    /* Gets the tag's text, returns what to show */
    using text_t = std::function<std::u16string(std::u16string_view)>;

    struct value_t
    {
        getter_t get;
        setter_t set;
    };

    void on_function(libc_types::u16 function, function_t&& handler)
    {
        m_functions[function] = std::move(handler);
    }

    void bind_value(std::string_view widget, getter_t&& get, setter_t&& set)
    {
        m_values[std::string(widget)] = {std::move(get), std::move(set)};
    }

    void bind_text(std::string_view widget, text_t&& text)
    {
        m_texts[std::string(widget)] = std::move(text);
    }

    /* nullopt when no provider handles the function */
    std::optional<ui_result_t> call(UIFunctionCall const& call) const
    {
        auto it = m_functions.find(call.function);
        if(it == m_functions.end())
            return std::nullopt;
        return it->second(call);
    }

    value_t const* value(std::string_view widget) const
    {
        auto it = m_values.find(std::string(widget));
        return it != m_values.end() ? &it->second : nullptr;
    }

    text_t const* text(std::string_view widget) const
    {
        auto it = m_texts.find(std::string(widget));
        return it != m_texts.end() ? &it->second : nullptr;
    }

    libc_types::u64 next_token()
    {
        return ++m_token;
    }

  private:
    std::unordered_map<libc_types::u16, function_t> m_functions;
    std::unordered_map<std::string, value_t>        m_values;
    std::unordered_map<std::string, text_t>         m_texts;
    libc_types::u64                                 m_token{0};
};
