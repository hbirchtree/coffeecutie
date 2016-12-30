#ifndef COFFEE_GRAPHICS_APIS_OPENGL_BUFFER_STORAGE_FALLBACK_H
#define COFFEE_GRAPHICS_APIS_OPENGL_BUFFER_STORAGE_FALLBACK_H

#include "cbuffer_types.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief Will map a buffer to a void* pointer which may be used with normal memory operations. May slow down GL performance, as the buffer may be located in system heap. This is a slower version for older GL versions.
 * \param buf The buffer to be mapped
 * \param mask Buffer data access mask, determines characteristics of mapping
 * \return A pointer to the buffer data
 */
extern void* coffee_graphics_buffer_map_safe(CBuffer& buf, CBufferAccess const& mask);
/*!
 * \brief The previously mapped pointer should no longer be used. This is a slower version for older GL versions.
 * \param buf Buffer to be unmapped
 * \return Whether or not the unmapping succeeded. Returns true if buffer was not mapped.
 */
extern bool coffee_graphics_buffer_unmap_safe(CBuffer& buf);

/*!
 * \brief Download part or all of a buffer into a local pointer. This is a slower version for older GL versions.
 * \param buf Buffer to download
 * \param offset Offset into buffer
 * \param size Size of region
 * \return A pointer to the downloaded buffer data
 */
extern void* coffee_graphics_buffer_download_buffer_safe(
        CBuffer& buf, CGszptr const& offset, CGsize const& size);

/*!
 * \brief Store mutable data in buffer. This is a slower version for older GL versions.
 * \param buf Buffer to store into
 * \param data Pointer to data
 * \param size Size of data
 * \param usage Storage flag, determines storage location
 */
extern void coffee_graphics_buffer_store_safe(
        CBuffer& buf, const void* data,
        CGsize const& size, CBufferUsage const& usage);
/*!
 * \brief Store mutable data in buffer. This is a slower version for older GL versions.
 * \param buf Buffer to store into
 * \param data Pointer to data
 * \param size Size of data
 * \param usage Storage mask
 */
extern void coffee_graphics_buffer_store_immutable_safe(
        CBuffer& buf, const void* data,
        CGsize const& size, CBufferStorage const& usage);
/*!
 * \brief This version uses a more widely available set of commands, but is generally slower. Binds and unbinds buffer in order to work.
 * \param buf Buffer to be stored into
 * \param data Pointer to data
 * \param offset Offset into GL buffer
 * \param size Size of data
 *
 */
extern void coffee_graphics_buffer_substore_safe(
        CBuffer& buf, const void* data,
        CGszptr const& offset, CGsize const& size);
/*!
 * \brief Copies buffer data, using buffer handles. This is a slower version for older GL versions.
 * \param buf1 Source buffer handle
 * \param buf2 Target buffer handle
 * \param offset1 Source offset
 * \param offset2 Target offset
 * \param size Size of data
 */
extern void coffee_graphics_buffer_copy_safe(
        CBuffer const& buf1, CBuffer& buf2,
        CGszptr const& offset1, CGszptr const& offset2,
        CGsize const& size);
/*!
 * \brief Invalidates contents of buffer, often used with mapping to allow faster discarding
 * \param buf Target buffer
 */
extern void coffee_graphics_buffer_invalidate_safe(CBuffer& buf);
}
}

#endif
