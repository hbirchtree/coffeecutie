#ifndef CBLAM_SCENARIO_LOADER
#define CBLAM_SCENARIO_LOADER

#include "cblam_scenario.h"

namespace Coffee{
namespace CBlam{

extern const blam_scenario *blam_scn_get(
        const blam_file_header* map,
        const blam_tag_index *tags);

}
}

#endif
