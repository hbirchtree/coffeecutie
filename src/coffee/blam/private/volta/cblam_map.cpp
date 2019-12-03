#include <coffee/blam/volta/cblam_map.h>
#include <coffee/core/types/chunk.h>

namespace blam {
namespace detail {

c_cptr magic_ptr(c_cptr base, i32 magic, i32 offset)
{
    const byte_t* ptr = (C_FCAST<const byte_t*>(base)) + offset - magic;
    return (ptr < base) ? nullptr : ptr;
}

} // namespace detail

using semantic::Bytes;
using namespace semantic::chunk_ops;

cstring file_header_full_mapname(const file_header_t* map)
{
    for(auto name : blam::map_names)
    {
        if(libc::str::cmp(C_OCAST<const char*>(map->name), name.first))
            return name.second;
    }
    return map->name;
}

file_header_t const* file_header_get(c_cptr baseptr, version_t expectedVersion)
{
    file_header_t const* fh = C_CAST<const file_header_t*>(baseptr);
    if(fh->version != expectedVersion &&
       MemCmp(Bytes::Create(fh->id), Bytes::From(header_head, 4)) &&
       MemCmp(Bytes::Create(fh->footer), Bytes::From(header_foot, 4)))
        return nullptr;
    return fh;
}

void tag_index_magic(tag_index_t* tagindex, i32 tagIndexOffset)
{
    tagindex->index_magic -= tagIndexOffset + 40;
}

const tag_index_t* tag_index_ptr(const file_header_t* file)
{
    return C_FCAST<const tag_index_t*>(
        &(C_FCAST<const byte_t*>(file))[file->tagIndexOffset]);
}

tag_index_t tag_index_get(const file_header_t* file)
{
    const tag_index_t* tgi = tag_index_ptr(file);
    tag_index_t        dupe;
    MemCpy(Bytes::Create(*tgi), Bytes::Create(dupe));
    //    MemCpy(&dupe,tgi,sizeof(tag_index_t));
    tag_index_magic(&dupe, file->tagIndexOffset);
    return dupe;
}

const index_item_t* tag_index_get_items(const file_header_t* file)
{
    const tag_index_t* ptr = tag_index_ptr(file);
    return C_FCAST<const index_item_t*>(&(ptr[1]));
}

cstring index_item_get_string(
    const index_item_t*  idx,
    const file_header_t* map,
    const tag_index_t*   tagindex)
{
    return magic_ptr<cstring>(
        map, tagindex->index_magic, C_CAST<i32>(idx->string_offset));
}

const index_item_t* tag_index_get_item(
    const file_header_t* file, const tag_index_t* tags, i32 tag_id)
{
    return &tag_index_get_items(file)[tag_id - tags->baseTag];
}

cstring tagref_get_name(
    const tagref_t* tag, const file_header_t* file, const tag_index_t* tags)
{
    return magic_ptr<cstring>(file, tags->index_magic, tag->string_offset);
}

} // namespace blam
