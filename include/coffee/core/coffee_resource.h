#pragma once

#include "types/tdef/integertypes.h"
#include "types/tdef/stltypes.h"

namespace Coffee{
namespace CResources{

/*!
 * \brief Set the asset/resource prefix for file access.
 * Used mostly by the Url class when searching for assets.
 * \param prefix shall always end with '/', if you don't,
 *  santa will give you coal.
 */
extern void FileResourcePrefix(cstring prefix);

/*!
 * \brief Get the current file resource prefix
 * \return
 */
extern CString const& GetFileResourcePrefix();

}
}
