#include "cblam_map.h"

namespace Coffee{
namespace CBlam{

cstring blam_file_header_full_mapname(
        const blam_file_header *map)
{
    for(int32 i=0;i<blam_num_map_names;i++)
    {
        if(c_strcmp(map->name,blam_map_names[i].key))
            return blam_map_names[i].value;
    }
    return "";
}

blam_file_header *blam_file_header_get(
        void *baseptr, blam_version expectedVersion)
{
    blam_file_header* fh = (blam_file_header*)baseptr;
    if(
            fh->version!=expectedVersion&&
            c_memcmp(&fh->id,blam_header_head,4)&&
            c_memcmp(&fh->footer,blam_header_foot,4))
        return nullptr;
    return fh;
}

void blam_tag_index_magic(
        blam_tag_index *tagindex,
        int32 tagIndexOffset)
{
    tagindex->index_magic -= tagIndexOffset+40;
}

const blam_tag_index *blam_tag_index_ptr(
        const blam_file_header *file)
{
    return (const blam_tag_index*)&((const byte*)file)[file->tagIndexOffset];
}

blam_tag_index blam_tag_index_get(
        const blam_file_header *file)
{
    const blam_tag_index* tgi = blam_tag_index_ptr(file);
    blam_tag_index dupe;
    c_memcpy(&dupe,tgi,sizeof(blam_tag_index));
    blam_tag_index_magic(&dupe,file->tagIndexOffset);
    return dupe;
}

const blam_index_item *blam_tag_index_get_items(
        const blam_file_header *file)
{
    const blam_tag_index* ptr = blam_tag_index_ptr(file);
    return (const blam_index_item*)&(ptr[1]);
}

const void *blam_mptr(const void *base, int32 magic, int32 offset)
{
    const byte* ptr = ((const byte*)base)+offset-magic;
    return (ptr < base) ? nullptr : ptr;
}

cstring blam_index_item_get_string(
        const blam_index_item *idx,
        const blam_file_header *map,
        const blam_tag_index *tagindex)
{
    return (const byte*)blam_mptr(map,tagindex->index_magic,idx->string_offset);
}

const blam_index_item *blam_tag_index_get_item(
        const blam_file_header *file,
        const blam_tag_index *tags,
        int32 tag_id)
{
    return &blam_tag_index_get_items(file)[tag_id-tags->baseTag];
}

cstring blam_tagref_get_name(const blam_tagref *tag, const blam_file_header *file, const blam_tag_index *tags)
{
    return (cstring)blam_mptr(file,tags->index_magic,tag->string_offset);
}

}

}
