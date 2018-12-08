#pragma once

#include <url/url.h>

namespace Coffee {
namespace Strings {
extern stl_types::CString to_string(platform::url::Url const& url);
extern stl_types::CString to_string(platform::url::Path const& path);
} // namespace Strings
} // namespace Coffee
