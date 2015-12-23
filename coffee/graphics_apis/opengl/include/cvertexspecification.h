#ifndef COFFEE_GRAPHICS_OPENGL_VERTEX_SPECIFICATION_H
#define COFFEE_GRAPHICS_OPENGL_VERTEX_SPECIFICATION_H

#include "wrappers/cvertexarrayobject.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CVertexSpecification
{
    std::vector<CVertexFormat*> formats;
    std::vector<CVertexAttribute> attributes;
};

struct CVertexDataSet
{
    std::vector<CVertexBufferBinding*> bindings;
};

class CVertexDescription
{
public:
    CVertexDescription()
    {
    }
    ~CVertexDescription()
    {
        for(CVertexBufferBinding* bnd : m_dataset.bindings)
            delete bnd;
        for(CVertexFormat* fmt : m_spec.formats)
            delete fmt;
    }

    CVertexFormat* getFormat(CDataType type, CGszptr offset, CGsize size)
    {
        for(CVertexFormat *fmt : m_spec.formats)
            if(size==fmt->size && type==fmt->type && offset==fmt->offset)
            {
                return fmt;
            }

        CVertexFormat *fmt = new CVertexFormat;
        fmt->size = size;
        fmt->type = type;
        fmt->offset = offset;
        m_spec.formats.push_back(fmt);

        return fmt;
    }

    template<typename T, size_t Size,CDataType MType>
    void addAttribute(int32 attrIdx, CVectors::_cbasic_tvector<T,Size> const* = nullptr)
    {
        CVertexAttribute attr = {};
        attr.attribIdx = attrIdx;
        attr.fmt = getFormat(MType,0,Size);

        CVertexBufferBinding *bnd = new CVertexBufferBinding;
        bnd->stride = sizeof(CVectors::_cbasic_tvector<T,Size>);

        m_dataset.bindings.push_back(bnd);
        attr.bnd = bnd;

        m_spec.attributes.push_back(attr);
    }

    template<typename T, size_t Size>
    void addAttributeMatrix(int32 attribIdxBase, CVectors::_cbasic_tmatrix<T,Size> const*)
    {
        CVertexFormat* fmt = getFormat(CDataType::Scalar,0,Size);
        for(size_t i=0;i<Size;i++)
        {
            CVertexAttribute attr = {};
            attr.fmt = fmt;
            attr.attribIdx = attribIdxBase+i;

            CVertexBufferBinding *bnd = new CVertexBufferBinding;
            bnd->offset = sizeof(typename CVectors::_cbasic_tmatrix<T,Size>::col_type)*i;
            bnd->stride = sizeof(CVectors::_cbasic_tmatrix<T,Size>);
            m_dataset.bindings.push_back(bnd);
            attr.bnd = bnd;

            m_spec.attributes.push_back(attr);
        }
    }

    CVertexBufferBinding* getBinding(uint32 attribIdx)
    {
        for(CVertexAttribute &attr : m_spec.attributes)
            if(attr.attribIdx == attribIdx)
                return attr.bnd;
        return nullptr;
    }

    void applyAttributes(CVertexArrayObject& vao)
    {
        for(CVertexAttribute const& attr : m_spec.attributes)
        {
            coffee_graphics_vao_attribute_format(vao,attr,*attr.fmt);
            coffee_graphics_vao_attribute_buffer(vao,attr,*attr.bnd);
        }
    }

    void bindAttributes(CVertexArrayObject& vao)
    {
        for(CVertexAttribute const& attr : m_spec.attributes)
            coffee_graphics_vao_attribute_bind_buffer(vao,*attr.bnd);
    }

private:
    struct cdata_idx
    {
        size_t fmt;
        size_t bnd;
    };

    CVertexDataSet m_dataset;
    CVertexSpecification m_spec;
    std::vector<cdata_idx> idx;
};

}
}

#endif
