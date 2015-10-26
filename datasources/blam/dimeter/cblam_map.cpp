#include "cblam_map.h"

namespace Coffee{
namespace CBlam{

cstring blam_file_header_full_mapname(const blam_file_header *map)
{
    for(int32 i=0;i<blam_num_map_names;i++)
    {
        if(!strcmp(map->name,blam_map_names[i].inname))
            return blam_map_names[i].outname;
    }
    return "";
}

blam_file_header *blam_file_header_get(void *baseptr, int32 expectedVersion)
{
    blam_file_header* fh = (blam_file_header*)baseptr;
    if(
            fh->version!=expectedVersion&&
            coffee_cmp_memarea((cstring)&fh->id,blam_header_head,4)&&
            coffee_cmp_memarea((cstring)&fh->footer,blam_header_foot,4))
        return nullptr;
    return fh;
}

void blam_tag_index_magic(blam_tag_index *tagindex, int32 tagIndexOffset)
{
    tagindex->index_magic -= tagIndexOffset+40;
}

const blam_tag_index *blam_tag_index_ptr(const blam_file_header *file)
{
    return (const blam_tag_index*)&((const byte*)file)[file->tagIndexOffset];
}

blam_tag_index blam_tag_index_get(const blam_file_header *file)
{
    const blam_tag_index* tgi = blam_tag_index_ptr(file);
    blam_tag_index dupe;
    memcpy(&dupe,tgi,sizeof(blam_tag_index));
    blam_tag_index_magic(&dupe,file->tagIndexOffset);
    return dupe;
}

const blam_index_item *blam_tag_meta_offset(const blam_file_header *file)
{
    const blam_tag_index* ptr = blam_tag_index_ptr(file);
    return (const blam_index_item*)&(ptr[1]);
}

const void *blam_magic_ptr(const void *base, int32 magic, int32 offset)
{
    return ((const byte*)base)+offset-magic;
}

cstring blam_index_item_get_string(const blam_index_item *idx, const blam_file_header *map, const blam_tag_index *tagindex)
{
    return ((const byte*)map)+idx->stringOffset-tagindex->index_magic;
}

}

bool coffee_cmp_memarea(cstring target, cstring cmp, szptr len)
{
    return memcmp(target,cmp,len)==0;
}

}
