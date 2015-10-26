#include "cblam_map.h"

namespace Coffee{
namespace CBlam{
namespace Elegy{

cstring blam_file_header_full_mapname(const blam_file_header_h3 *map)
{
    for(int32 i=0;i<blam_num_map_names;i++)
    {
        if(!strcmp(map->name,blam_map_names[i].inname))
            return blam_map_names[i].outname;
    }
    return "";
}

}
}
}
