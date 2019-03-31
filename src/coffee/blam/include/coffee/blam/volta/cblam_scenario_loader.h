#pragma once

#include "cblam_scenario.h"

namespace Coffee {
namespace Blam {

class tag_index_view;

/*!
 * \brief Acquire a map's scenario structure, containing information about
 * objects and etc. \param tags Wrapped container for tag_index_t \return A
 * valid pointer to the map file's scenario. In Halo 1 this is the first tag of
 * the tag index. This pointer should only invalid if the map as a whole is
 * invalid.
 */
extern const scenario* scn_get(tag_index_view& tags);

} // namespace Blam
} // namespace Coffee
