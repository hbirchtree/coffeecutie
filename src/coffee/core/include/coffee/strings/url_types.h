#pragma once

#include <url/url.h>

namespace Coffee {
namespace Strings {
extern std::string to_string(platform::url::Url const& url);
extern std::string to_string(platform::url::Path const& path);
} // namespace Strings
} // namespace Coffee
