#pragma once

#include <peripherals/stl/types.h>
#include <regex>

namespace stl_types {
namespace regex {

using namespace ::stl_types;

struct regex_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int) const;
};

using regex_error_code =
    domain_error_code<std::regex_constants::error_type, regex_error_category>;

} // namespace regex
} // namespace stl_types
