#ifndef CBUFFER
#define CBUFFER

#include "coffee/core/coffee.h"
#include "cbuffer_storage.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief Allocation in this context is the creation of the object. It will not have defined content unless something is stored in it.
 * \param buf Buffer to be allocated
 */
extern void coffee_graphics_alloc(
        size_t count, CBuffer* buf, const CBufferType &type = CBufferType::Array);
/*!
 * \brief Allocate multiple buffers at once, efficient for minimal GL calls
 * \param count
 * \param buf
 */
extern void coffee_graphics_free(size_t count, CBuffer *buf);

inline void coffee_graphics_alloc(CBuffer& buf)
{
    coffee_graphics_alloc(1,&buf);
}

inline void coffee_graphics_free(CBuffer& buf)
{
    coffee_graphics_free(1,&buf);
}

/*!
 * \brief This operation is used before bindless operations, where a buffer has to have been bound at some point before being used. Binds and unbinds a buffer
 * \param buf The buffer to be "activated"
 */
extern void coffee_graphics_activate(CBuffer& buf);
/*!
 * \brief Bind a buffer to buffer's target
 * \param buf Buffer to be bound to the buffer structure's buffer target
 */
extern void coffee_graphics_bind(CBuffer& buf);
/*!
 * \brief Unbind buffer from buffer's target
 * \param buf Provides buffer target for unbinding
 */
extern void coffee_graphics_unbind(CBuffer& buf);

/*!
 * \brief This operation will create a new buffer, copy the specified data and delete the old buffer.
 * \param buf Target buffer
 * \param oldOffset Offset for data in old buffer
 * \param oldSize Size of data in old buffer
 * \param targetOffset Offset for old data in new buffer (end of data should be within buffer)
 * \param newSize Size of the new buffer
 */
extern void coffee_graphics_buffer_resize(CBuffer &buf,
        const CGszptr &oldOffset, const CGsize &oldSize,
        const CGszptr &targetOffset, const CGsize &newSize);

/*!
 * \brief If parent is a mapped buffer, return an offset pointer
 * \param buf Subbuffer target
 * \return A pointer to this subbuffer's data
 */
inline void* coffee_graphics_buffer_sub_dataptr(CSubBuffer& buf)
{
    if(!buf.parent&&!buf.parent->data)
        return nullptr;
    byte_t* dPtr = (byte_t*)buf.parent->data;
    return &dPtr[buf.offset];
}

/*!
 * \brief A typical discarding transfer to a buffer.
 * \param buffer
 * \param offset
 * \param size
 * \param data
 */
extern void coffee_graphics_buffer_map_memcpy(
        CBuffer* buffer, szptr offset, szptr size, c_cptr data);

}
}

#endif // CBUFFER

