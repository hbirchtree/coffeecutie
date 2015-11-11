#ifndef CVERTEXARRAYOBJECT
#define CVERTEXARRAYOBJECT

#include "cbuffer.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief A wrapper for VAO objects
 */
struct CVertexArrayObject{
    CVertexArrayObject();

    CGhnd handle; /*!< VAO handle */
};

/*!
 * \brief Vertex format description, specifies data format in buffer
 */
struct CVertexFormat
{
    CVertexFormat();

    CGsize size; /*!< Amount of values of type*/
    CGszptr offset; /*!< Offset in vertex buffer, used for interleaved attribute*/
    CDataType type; /*!< Data type*/
    bool normalized; /*!< Whether it is normalized or not*/
};

/*!
 * \brief Vertex buffer binding, used to associate vertex attribute with buffer
 */
struct CVertexBufferBinding
{
    CVertexBufferBinding();

    CBuffer* buffer; /*!< Buffer bound to attribute*/
    CGszptr offset; /*!< Offset into stride*/
    CGsize stride; /*!< Total size of attributes*/
    CGidx divisor; /*!< Divisor for instancing*/
    CGidx binding; /*!< Binding index*/
};

/*!
 * \brief A vertex attribute that is used with VAO objects
 */
struct CVertexAttribute
{
    CVertexAttribute();

    CGidx attribIdx; /*!< Attribute index to shader*/
    CVertexFormat* fmt; /*!< Vertex format for attribute*/
    CVertexBufferBinding* bnd; /*!< Vertex buffer binding for attribute*/
};

/*!
 * \brief Allocate a vertex array object
 * \param vao
 */
extern void coffee_graphics_alloc(CVertexArrayObject* vao);
/*!
 * \brief Free a vertex array object
 * \param vao
 */
extern void coffee_graphics_free(CVertexArrayObject* vao);

/*!
 * \brief Bind and unbind vertex array object, convenience function
 * \param vao
 */
extern void coffee_graphics_activate(const CVertexArrayObject* vao);

/*!
 * \brief Bind vertex array object
 * \param vao
 */
extern void coffee_graphics_bind(const CVertexArrayObject* vao);
/*!
 * \brief Unbind vertex array object, takes arbitrary VAO object to select the overload
 */
extern void coffee_graphics_unbind(const CVertexArrayObject*);

/*!
 * \brief Set format for a VAO attribute. This is used initally to specify the vertex attribute
 * \param vao VAO to set vertex attribute to
 * \param attr Attribute to set
 * \param fmt Format to set
 */
extern void coffee_graphics_vao_attribute_format(
        CVertexArrayObject *vao, const CVertexAttribute& attr,
        const CVertexFormat &fmt);
/*!
 * \brief Set vertex buffer binding for a VAO attribute. This is done after defining the format of a vertex attribute
 * \param vao VAO to set vertex buffer to
 * \param attr Attribute to bind with
 * \param buf Buffer binding
 */
extern void coffee_graphics_vao_attribute_buffer(
        CVertexArrayObject* vao, const CVertexAttribute &attr,
        const CVertexBufferBinding& buf);
/*!
 * \brief Bind buffer for VAO. This is done before a VAO is rendered
 * \param vao VAO to bind with
 * \param buf Buffer binding to bind
 */
extern void coffee_graphics_vao_attribute_bind_buffer(
        const CVertexArrayObject* vao,
        const CVertexBufferBinding& buf);
/*!
 * \brief Bind a specified buffer to a binding point on a VAO, useful for swapping vertex buffers
 * \param vao
 * \param buf
 * \param buffer
 */
extern void coffee_graphics_vao_attribute_bind_buffer(
        const CVertexArrayObject *vao,
        const CVertexBufferBinding &buf,
        const CBuffer* buffer);

}
}

#endif // CVERTEXARRAYOBJECT

