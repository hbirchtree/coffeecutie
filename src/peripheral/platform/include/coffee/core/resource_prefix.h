#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace file {

/*!
 * \brief Set the asset/resource prefix for file access.
 * Used mostly by the Url class when searching for assets.
 * \param prefix shall always end with '/', if you don't,
 *  santa will give you coal.
 */
extern void ResourcePrefix(libc_types::cstring prefix);

/*!
 * \brief Get the current file resource prefix
 * \return
 */
extern std::optional<std::string> ResourcePrefix(bool fallback = false);

} // namespace file
} // namespace platform

namespace Coffee {
namespace file {
using namespace ::platform::file;
}
} // namespace Coffee
