#ifndef CBLAM_SCENARIO_LOADER
#define CBLAM_SCENARIO_LOADER

#include "cblam_scenario.h"

namespace Coffee{
namespace Blam{

/*!
 * \brief Acquire a map's scenario structure, containing information about objects and etc.
 * \param map Pointer to the map file header
 * \param tags Pointer to the tag index
 * \return A valid pointer to the map file's scenario. In Halo 1 this is the first tag of the tag index. This pointer should only invalid if the map as a whole is invalid.
 */
extern const blam_scenario *blam_scn_get(
        const file_header_t* map,
        const tag_index_t *tags);

}
}

#endif
