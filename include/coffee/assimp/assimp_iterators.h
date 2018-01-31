#pragma once

#include "cassimpimporters.h"
#include "assimp_data.h"

#include <assimp/scene.h>
#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <assimp/color4.h>

#include <coffee/graphics/common/SMesh>
#include <coffee/interfaces/cgraphics_api.h>

namespace Coffee{
namespace ASSIMP{

struct MeshLoader
{
    using M = Mesh;

    using AttrType = M::AttributeTypes;

    enum Quirks
    {
        QuirkCompressElements = 0x1,
        /*!< When possible, use a smaller data type for element indices */
        QuirkCompressVertices = 0x2,
        /*!< When possible, use lower-precision format for storing vertex data */
        QuirkElementVertexOffset = 0x4,
        /*!< Add vertex offsets to elements */
    };

    struct Attr
    {
        Attr(AttrType t, u32 ch = 0):
            type(t),
            channel(ch)
        {
        }

        AttrType type;
        u32 channel;
    };

    struct DrawInfo
    {
        DrawInfo():
            element_type(TypeEnum::UInt),
            quirks(0)
        {
        }

        TypeEnum element_type;
        u32 quirks;
    };

    template<typename To, typename From = u32,
             typename std::enable_if<std::is_integral<To>::value
                                     >::type* = nullptr,
             typename std::enable_if<std::is_integral<From>::value
                                     >::type* = nullptr,
             typename std::enable_if<
                 std::is_signed<To>::value == std::is_signed<From>::value
                 >::type* = nullptr>
    /*!
     * \brief integer_downcast is aimed for the current scenario:
     *  - The integers are within a continuous range
     *  - The continuous range in From fits into the limits of the
     *           To datatype
     * This allows simple compression of element indices where possible.
     * \param target
     * \param src
     * \param size
     * \return
     */
    static szptr integer_downcast(c_ptr target, c_cptr src, szptr size)
    {
        if(!target || !src)
            return size / (sizeof(From) / sizeof(To));

        From const* srcT = C_RCAST<From const*>(src);
        To* trgT = C_RCAST<To*>(target);
        szptr icount = size / sizeof(From);

        for(szptr i=0; i<icount; i++)
        {
            From const& s = srcT[i];
            To& t = trgT[i];

            t = C_FCAST<To>(s);
        }

        return size / (sizeof(From) / sizeof(To));
    }

    template<typename T>
    static T integer_range_get_max(T const* src, szptr count)
    {
        T max = std::numeric_limits<T>::min();
        for(szptr i=0; i<count; i++)
            max = CMath::max(max, src[i]);

        return max;
    }

    template<typename To, typename From = scalar,
             typename std::enable_if<
                 std::is_signed<To>::value>::type* = nullptr,
             typename std::enable_if<
                 std::is_integral<To>::value>::type* = nullptr>
    /* For simple down-cast from floating-point to i16 and i8, scales  */
    static szptr integer_transform(c_ptr target, c_cptr src, szptr size)
    {
        if(!target || !src)
            return size / (sizeof(From) / sizeof(To));

        From const* srcT = C_RCAST<From const*>(src);
        To* trgT = C_RCAST<To*>(target);
        szptr icount = size / sizeof(From);

        auto constexpr to_min = std::numeric_limits<To>::min();
        auto constexpr to_max = std::numeric_limits<To>::max();

        scalar from_min = std::numeric_limits<scalar>::infinity();
        scalar from_max = -std::numeric_limits<scalar>::infinity();

        for(szptr i=0; i<icount; i++)
        {
            from_min = CMath::min(from_min, srcT[i]);
            from_max = CMath::max(from_max, srcT[i]);
        }

        for(szptr i=0; i<icount; i++)
        {
            From const& s = srcT[i];
            To& t = trgT[i];

            To res = (((s - from_min) *
                       (1.0 / from_max) * 2.0) - 1.0)
                    * to_max;

            t = res;
        }

        return size / (sizeof(From) / sizeof(To));
    }

    template<szptr Size, /* Size of structure that is copied */
             szptr Stride /* Size of entire structure */
             >
    /*!
     * \brief jumping_memcpy is made for the case where you want to copy a vec2 out of a vec3 structure. This is specifically used for Assimp's vec3 texture coordinates, because we mostly use vec2 for TCs.
     * \param target
     * \param source
     * \param size
     * \return
     */
    static szptr jumping_memcpy(c_ptr target, c_cptr source, szptr size)
    {
        static_assert(Stride > Size,
                      "Stride has to be larger than Size"
                      " for this to be effective");

        szptr ptr = 0;
        szptr tptr = 0;

        byte_t* raw_target = C_RCAST<byte_t*>(target);
        byte_t const* raw_sauce = C_RCAST<byte_t const*>(source);

        while(ptr + Stride < size)
        {
            MemCpy(&raw_target[ptr], &raw_sauce[ptr], Size);

            ptr += Stride;
            tptr += Size;
        }

        return (size / (Size - (Stride - Size))) * Size;
    }

    /*!
     * \brief Container for Bytes that will collect Bytes into another
     *  memory region, making it easier to do zero-copy transfer
     *  between ASSIMP and the GL.
     */
    struct ChainedBytes
    {
        static szptr default_transform(c_ptr dest, c_cptr src, szptr size)
        {
            if(!dest || !src)
                return size;
            MemCpy(dest, src, size);
            return size;
        }

        ChainedBytes():
            refs()
        {
        }

        using xf = szptr(*)(c_ptr target, c_cptr source, szptr size);

        Vector<Bytes> refs;
        /* transform works as such: when given a memory region source+size,
         *  the enclosed function will return the N bytes output by
         *  to the target.
         * If given a nullptr as target, the function will only
         *  return an estimate for the amount of memory needed.
         *  The source+size memory area contains floating-point values
         *  which may be converted to integer types. */
        Vector<xf> ref_transform;

        FORCEDINLINE void check_transforms()
        {
            /* If user hasn't supplied anything, fill it
             *  with default */
            if(ref_transform.size() == 0)
            {
                ref_transform.reserve(refs.size());
                for(szptr i=0;i<refs.size();i++)
                    ref_transform.push_back(default_transform);
            }
        }

        FORCEDINLINE szptr size()
        {
            szptr size = 0;

            check_transforms();

            szptr i = 0;
            for(auto const& data : refs)
            {
                size += ref_transform[i](
                        nullptr, nullptr, data.size);
                i ++;
            }

            return size;
        }

        FORCEDINLINE bool cpy(void* target, szptr size)
        {
            szptr ptr = 0;

            check_transforms();

            /* First, do a dry run to check that the
             * copy will succeed */
            szptr i = 0;
            for(auto const& data : refs)
            {
                szptr est = ref_transform[i](
                            nullptr, nullptr, data.size);
                if(ptr + est > size)
                    return false;
                ptr += est;
                i++;
            }

            ptr = 0;
            i = 0;

            byte_t* targetP = C_RCAST<byte_t*>(target);

            for(auto const& data : refs)
            {
                auto transform = ref_transform[i];
                ptr += transform(&targetP[ptr], data.data, data.size);
                i++;
            }

            return true;
        }
    };

    template<typename GFX>
    struct BufferDescription
    {
//        using GFX = RHI::NullAPI;

        typename GFX::D_CALL call;
        Vector<typename GFX::V_ATTR> attributes;
        Vector<typename GFX::D_DATA> draws;
        Vector<Mesh> meshdata;

        ChainedBytes vertexData;
        ChainedBytes elementData;
    };

    template<typename API>
    static
    bool ExtractDescriptors(
            ASSIMP::AssimpPtr& scene,
            Vector<Attr>const& attributes,
            DrawInfo const& draw,
            BufferDescription<API>& buffers
            )
    {
        i32 meshCount = 0;

        if(!ASSIMP::GetMeshCount(scene, meshCount))
            return false;

        /* Helper class for calculating attribute sizes */
        Vector<Mesh>& meshes = buffers.meshdata;
        meshes.resize(meshes.size() + C_FCAST<szptr>(meshCount));

        buffers.attributes.resize(
                    attributes.size());
        buffers.draws.resize(
                    C_FCAST<szptr>(meshCount));
        buffers.vertexData.refs.resize(
                    C_FCAST<szptr>(meshCount) * attributes.size()
                    );
        buffers.elementData.refs.resize(
                    C_FCAST<szptr>(meshCount));

        szptr mesh_buffer_size = 0,
                element_buffer_size = 0,
                vertex_size = 0;

        buffers.call.m_idxd = true;
        buffers.call.setPrim(Prim::Triangle);
        buffers.call.setCreat(PrimCre::Explicit);

        u32 max_element = 0;

        for(auto i : Range<i32>(meshCount))
        {
            auto& mesh = meshes[i];
            auto& di = buffers.draws[i];

            ASSIMP::GetMeshData(scene, i, mesh);

            di.m_eltype = TypeEnum::UInt;
            di.m_elems = mesh.attrSize(M::Indices) / sizeof(u32);
            di.m_eoff = element_buffer_size / sizeof(u32);
            di.m_verts = mesh.attrSize(M::Position) / sizeof(Vecf3);
            di.m_voff = vertex_size;

            if(draw.quirks & QuirkElementVertexOffset)
            {
                /* For OpenGL ES 2.0, there is no such thing as
                 *  a vertex offset. This simplifies that situation. */
                u32* elements = mesh.getAttributeData<u32>(M::Indices);
                auto indices = mesh.attrCount(M::Indices, sizeof(u32));
                for(szptr i=0; i<indices; i++)
                {
                    elements[i] += di.m_voff;
                    di.m_voff = 0;
                }
            }

            /* When compressing elements, we would like to avoid the
             *  case where each mesh has a different classification.
             *  Instead, the whole batch has a single classification. */
            if(draw.quirks & QuirkCompressElements)
                max_element = CMath::max(
                            integer_range_get_max(
                                mesh.getAttributeData<u32>(M::Indices),
                                mesh.attrCount(M::Indices, sizeof(u32))),
                            max_element);

            for(auto j : Range<i32>(attributes.size()))
            {
                /* Calculate attribute offsets */
                for(i32 k=j+1;k<attributes.size();k++)
                    buffers.attributes[k].m_boffset +=
                            mesh.attrSize(attributes[j].type,
                                          attributes[j].channel);

                /* Calculate total vertex buffer size for all attributes */
                mesh_buffer_size += mesh.attrSize(attributes[j].type,
                                                  attributes[j].channel);
            }

            vertex_size += mesh.attrCount(M::Position, sizeof(Vecf3));

            /* Calculate element buffer size */
            element_buffer_size += mesh.attrSize(M::Indices);

            /* Add element buffer to ChainedBytes structure */
            auto& ebuf = buffers.elementData.refs[i];
            ebuf.size = mesh.attrSize(M::Indices);
            ebuf.data = mesh.getAttributeData<byte_t>(M::Indices);
        }

        TypeEnum idx_type = TypeEnum::UInt;

        if(max_element <= std::numeric_limits<u8>::max())
            idx_type = TypeEnum::UByte;
        else if(max_element <= std::numeric_limits<u16>::max())
            idx_type = TypeEnum::UShort;

        if((draw.quirks & QuirkCompressElements)
                && idx_type != TypeEnum::UInt)
        {
            auto transform = ChainedBytes::default_transform;
            switch(idx_type)
            {
            case TypeEnum::UByte:
                transform = integer_downcast<u8>;
                break;
            case TypeEnum::UShort:
                transform = integer_downcast<u16>;
                break;
            default:
                break;
            }

            buffers.elementData.ref_transform.reserve(
                        buffers.elementData.refs.size());
            for(auto i : Range<>(buffers.elementData.refs.size()))
                buffers.elementData.ref_transform.push_back(transform);

            for(auto i : Range<>(meshCount))
            {
                buffers.draws[i].m_eltype = idx_type;
            }
        }

        i32 bufIdx = 0;
        buffers.vertexData.ref_transform.resize(
                    buffers.vertexData.size());
        for(auto const& attr : attributes)
            for(auto i :  Range<i32>(meshCount))
            {
                auto& buf = buffers.vertexData.refs[bufIdx];

                buf.size = meshes[i].attrSize(
                            attr.type, attr.channel);
                buf.data = meshes[i].getAttributeData<byte_t>(
                            attr.type, attr.channel);

                if(attr.type == AttrType::TexCoord)
                    buffers.vertexData.ref_transform[bufIdx] =
                            jumping_memcpy<sizeof(Vecf2), sizeof(Vecf3)>;
                else
                    buffers.vertexData.ref_transform[bufIdx] =
                            ChainedBytes::default_transform;

                bufIdx ++;
            }

        /* Initialize some vertex attribute information */
        for(auto i : Range<i32>(attributes.size()))
        {
            auto& vd = buffers.attributes[i];
            vd.m_idx = i; /* The user will have to define this or bind it */
            vd.m_bassoc = i;
            vd.m_type = TypeEnum::Scalar;
            switch(attributes[i].type)
            {
            case M::Color:
                vd.m_size = 4;
                break;
            case M::Position:
            case M::Normal:
            case M::Tangent:
            case M::Bitangent:
                vd.m_size = 3;
                break;
            case M::TexCoord:
                vd.m_size = 2;
                break;
            }
            vd.m_stride = vd.m_size * sizeof(scalar);
        }

        /* TODO: If user is requesting a different
         *  element type, perform on-the-fly conversion */

        /* TODO: See if we can use i16 or similar for attributes */

        return true;
    }
};

}
}
