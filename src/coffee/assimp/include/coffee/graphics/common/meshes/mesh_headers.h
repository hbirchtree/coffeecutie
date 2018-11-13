#pragma once

#include <peripherals/libc/types.h>
#include <coffee/interfaces/cgraphics_api.h>

namespace Coffee{
namespace SMSH{

const constexpr cstring magic_word = "CSMSH";

struct vertex_blob_t
{
    using buffer_id = u16;

    static constexpr buffer_id invalid_id =
            std::numeric_limits<buffer_id>::max();

    szptr content_size;
    i64 content_offset;
    buffer_id id;

    uint16 pad_2;
    u32 pad_3;
};

struct attr_description_t
{
    RHI::GraphicsAPI::VertexAttribute attr;
    u32 id;
};

struct mesh_descriptor_t
{
    RHI::GraphicsAPI::DrawInstanceData data;
    RHI::GraphicsAPI::DrawCall call_info;
    u32 id;
    u32 num_attrs;
    u16 element_buffer;
    u16 pad_1;

    /* These are there in memory, but not in the struct.
     *  They are accessed by `&mesh_descriptor_t[1]` */
/*    u32 attr_ids[num_attrs]; */
/*    u16 attr_buffers[num_attrs]; */
};

PACKEDSTRUCT smsh_header_t
{
    char magic[8];
    char build_ver[32];
    u32 num_attr_desc;
    u32 num_mesh_descs;
    u32 num_groups;
};

//struct smsh_groups_t
//{
//    vertex_blob_t attr_desc[1];
//    vertex_blob_t mesh_descs[1];
//    vertex_blob_t groups[1];
//};

}
}
