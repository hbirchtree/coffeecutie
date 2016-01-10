#include <volta/cblam_map.h>

namespace Coffee{
namespace CBlam{

cstring blam_file_header_full_mapname(
        const file_header_t *map)
{
    for(int32 i=0;i<blam_num_map_names;i++)
    {
        if(CStrCmp(map->name,blam_map_names[i].key))
            return blam_map_names[i].value;
    }
    return "";
}

file_header_t *blam_file_header_get(
        void *baseptr, version_t expectedVersion)
{
    file_header_t* fh = (file_header_t*)baseptr;
    if(
            fh->version!=expectedVersion&&
            CMemCmp(&fh->id,blam_header_head,4)&&
            CMemCmp(&fh->footer,blam_header_foot,4))
        return nullptr;
    return fh;
}

void blam_tag_index_magic(
        tag_index_t *tagindex,
        int32 tagIndexOffset)
{
    tagindex->index_magic -= tagIndexOffset+40;
}

const tag_index_t *blam_tag_index_ptr(
        const file_header_t *file)
{
    return (const tag_index_t*)&((const byte_t*)file)[file->tagIndexOffset];
}

tag_index_t blam_tag_index_get(
        const file_header_t *file)
{
    const tag_index_t* tgi = blam_tag_index_ptr(file);
    tag_index_t dupe;
    CMemCpy(&dupe,tgi,sizeof(tag_index_t));
    blam_tag_index_magic(&dupe,file->tagIndexOffset);
    return dupe;
}

const index_item_t *blam_tag_index_get_items(
        const file_header_t *file)
{
    const tag_index_t* ptr = blam_tag_index_ptr(file);
    return (const index_item_t*)&(ptr[1]);
}

const void *blam_mptr(const void *base, int32 magic, int32 offset)
{
    const byte_t* ptr = ((const byte_t*)base)+offset-magic;
    return (ptr < base) ? nullptr : ptr;
}

cstring blam_index_item_get_string(
        const index_item_t *idx,
        const file_header_t *map,
        const tag_index_t *tagindex)
{
    return (const byte_t*)blam_mptr(map,tagindex->index_magic,idx->string_offset);
}

const index_item_t *blam_tag_index_get_item(
        const file_header_t *file,
        const tag_index_t *tags,
        int32 tag_id)
{
    return &blam_tag_index_get_items(file)[tag_id-tags->baseTag];
}

cstring blam_tagref_get_name(const tagref_t *tag, const file_header_t *file, const tag_index_t *tags)
{
    return (cstring)blam_mptr(file,tags->index_magic,tag->string_offset);
}

}

}
