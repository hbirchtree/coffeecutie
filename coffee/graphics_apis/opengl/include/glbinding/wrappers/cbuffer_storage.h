#ifndef COFFEE_GRAPHICS_API_OPENGL_BUFFER_STORAGE_H
#define COFFEE_GRAPHICS_API_OPENGL_BUFFER_STORAGE_H

#include "cbuffer_storage_fallback.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief Will map a buffer to a void* pointer which may be used with normal memory operations. May slow down GL performance, as the buffer may be located in system heap.
 * \param buf The buffer to be mapped
 * \param mask Buffer data access mask, determines characteristics of mapping
 * \return A pointer to the buffer data
 */
extern void* coffee_graphics_buffer_map(
        CBuffer& buf, CBufferAccess const& mask);
/*!
 * \brief The previously mapped pointer should no longer be used.
 * \param buf Buffer to be unmapped
 * \return Whether or not the unmapping succeeded. Returns true if buffer was not mapped.
 */
extern bool coffee_graphics_buffer_unmap(
        CBuffer &buf);

/*!
 * \brief Download part or all of a buffer into a local pointer.
 * \param buf Buffer to download
 * \param offset Offset into buffer
 * \param size Size of region
 * \return A pointer to the downloaded buffer data
 */
extern void *coffee_graphics_buffer_download_buffer(
        CBuffer& buf, CGszptr const& offset, CGsize const& size);

/*!
 * \brief Store mutable data in buffer
 * \param buf Buffer to store into
 * \param data Pointer to data
 * \param size Size of data
 * \param usage Storage flag, determines storage location
 */
extern void coffee_graphics_buffer_store(
        CBuffer& buf, c_cptr data,
        CGsize const& size, CBufferUsage const& usage);
/*!
 * \brief Store immutable data in buffer.
 * \param buf Buffer to store into
 * \param data Pointer to data
 * \param size Size of data
 * \param usage Storage mask
 */
extern void coffee_graphics_buffer_store_immutable(
        CBuffer& buf, c_cptr data,
        CGsize const& size, CBufferStorage const& usage);
/*!
 * \brief coffee_graphics_buffer_substore
 * \param buf Buffer to be stored into
 * \param data Pointer to data
 * \param offset Offset into GL buffer
 * \param size Size of data
 */
extern void coffee_graphics_buffer_substore(
        CBuffer& buf, c_cptr data,
        CGszptr const& offset, CGsize const& size);

/*!
 * \brief Copies buffer data
 * \param buf1 Source buffer
 * \param buf2 Target buffer
 * \param offset1 Source offset
 * \param offset2 Target offset
 * \param size Size of data
 */
extern void coffee_graphics_buffer_copy(
        CBuffer const& buf1, CBuffer& buf2,
        CGszptr const& offset1, CGszptr const& offset2,
        CGsize const& size);

/*!
 * \brief Invalidates contents of buffer, often used with mapping to allow faster discarding
 * \param buf Target buffer
 */
extern void coffee_graphics_buffer_invalidate(CBuffer& buf);

/*!
 * \brief Bind range of buffer to an indexed target
 * \param buf The buffer used as source
 * \param index Index of the target
 * \param bufferType Type of buffer to bind to
 * \param offset Offset into source buffer
 * \param size Size in source buffer
 */
extern void coffee_graphics_buffer_bind_range(
        const _cbasic_graphics_resource_binding<_cbasic_graphics_buffer_section> &binding);

extern void coffee_graphics_buffer_bind_range(
        const CBuffer& buffer, const CBufferType& type,
        const CGszptr& offset, const CGsize& size, const CGuint &index);

}
}

#endif
