#include <glw/base.h>

#include <glw/enums/ErrorCode.h>
#define MAGIC_ENUM_RANGE_MIN 0x0
#define MAGIC_ENUM_RANGE_MAX 0x599
#include <peripherals/stl/magic_enum.hpp>

namespace gl::detail {

void error_check(std::string_view cmd_name)
{
    using gl::group::error_code;
    if constexpr(compile_info::debug_mode && !compile_info::platform::is_emscripten)
    {
        if(auto error = static_cast<error_code>(glGetError());
           error != error_code::no_error)
        {
            std::string out = "Error executing ";
            out.insert(out.end(), cmd_name.begin(), cmd_name.end());
            out.append(": ");
#if !defined(COFFEE_EMSCRIPTEN)
            auto error_name = magic_enum::enum_name(error);
#else
            std::string_view error_name = {};
#endif
            if(!error_name.empty())
                out.insert(out.end(), error_name.begin(), error_name.end());
            else
                out.append(stl_types::str::print::pointerify(
                    static_cast<libc_types::u32>(error)));
            if constexpr(compile_info::platform::is_emscripten)
                return;
            Throw(undefined_behavior(out));
        }
    }
}

} // namespace gl::detail
