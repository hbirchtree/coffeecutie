#ifndef CBUFFER
#define CBUFFER

#include "coffee/core/coffee.h"
#include "coffee/core/base/cdebug.h"
#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CBuffer{
    CBuffer();

    GLuint handle; /*!< GL handle for buffer */
    GLsizeiptr size; /*!< Size of buffer */
    GLenum bufferType;  /*!< Buffer type */
    BufferStorageMask flags;  /*!< Access mask */
    BufferAccessMask mapflags; /*!< Mapping access mask */
    void* data; /*!< Data pointer, only valid if it is mapped */
};

struct CSubBuffer
{
    CSubBuffer();

    CBuffer* parent; /*!< Pointer to parent containing data */
    GLsizeiptr offset; /*!< Offset into parent */
    GLsizeiptr size; /*!< Size in parent */
    GLenum bufferType; /*!< Buffer type */
    GLuint index; /*!< Binding index */
};

/*!
 * \brief Allocation in this context is the creation of the object. It will not have defined content unless something is stored in it.
 * \param buf Buffer to be allocated
 */
extern void coffee_graphics_alloc(CBuffer* buf);
/*!
 * \brief Allocate multiple buffers at once, efficient for minimal GL calls
 * \param count
 * \param buf
 */
extern void coffee_graphics_alloc(int count, CBuffer *buf);
/*!
 * \brief Any mapped buffer, content or similar is no longer valid after this.
 * \param buf Buffer to be freed
 */
extern void coffee_graphics_free(CBuffer* buf);
/*!
 * \brief Free multiple buffers at once, efficient for minimal GL calls
 * \param count
 * \param buf
 */
extern void coffee_graphics_alloc(int count, GLenum type, CBuffer *buf);

/*!
 * \brief This operation is used before bindless operations, where a buffer has to have been bound at some point before being used. Binds and unbinds a buffer
 * \param buf The buffer to be "activated"
 */
extern void coffee_graphics_activate(CBuffer* buf);
/*!
 * \brief Bind a buffer to buffer's target
 * \param buf Buffer to be bound to the buffer structure's buffer target
 */
extern void coffee_graphics_bind(CBuffer* buf);
/*!
 * \brief Unbind buffer from buffer's target
 * \param buf Provides buffer target for unbinding
 */
extern void coffee_graphics_unbind(CBuffer* buf);

/*!
 * \brief Will map a buffer to a void* pointer which may be used with normal memory operations. May slow down GL performance, as the buffer may be located in system heap.
 * \param buf The buffer to be mapped
 * \param mask Buffer data access mask, determines characteristics of mapping
 * \return A pointer to the buffer data
 */
extern void* coffee_graphics_buffer_map(CBuffer* buf, BufferAccessMask mask);
/*!
 * \brief The previously mapped pointer should no longer be used.
 * \param buf Buffer to be unmapped
 * \return Whether or not the unmapping succeeded. Returns true if buffer was not mapped.
 */
extern bool coffee_graphics_buffer_unmap(CBuffer* buf);

/*!
 * \brief Will map a buffer to a void* pointer which may be used with normal memory operations. May slow down GL performance, as the buffer may be located in system heap. This is a slower version for older GL versions.
 * \param buf The buffer to be mapped
 * \param mask Buffer data access mask, determines characteristics of mapping
 * \return A pointer to the buffer data
 */
extern void* coffee_graphics_buffer_map_safe(CBuffer* buf, BufferAccessMask mask);
/*!
 * \brief The previously mapped pointer should no longer be used. This is a slower version for older GL versions.
 * \param buf Buffer to be unmapped
 * \return Whether or not the unmapping succeeded. Returns true if buffer was not mapped.
 */
extern bool coffee_graphics_buffer_unmap_safe(CBuffer* buf);

/*!
 * \brief Download part or all of a buffer into a local pointer.
 * \param buf Buffer to download
 * \param offset Offset into buffer
 * \param size Size of region
 * \return A pointer to the downloaded buffer data
 */
extern void *coffee_graphics_buffer_download_buffer(CBuffer* buf, GLsizeiptr offset, GLsizeiptr size);

/*!
 * \brief Download part or all of a buffer into a local pointer. This is a slower version for older GL versions.
 * \param buf Buffer to download
 * \param offset Offset into buffer
 * \param size Size of region
 * \return A pointer to the downloaded buffer data
 */
extern void* coffee_graphics_buffer_download_buffer_safe(
        CBuffer* buf, GLsizeiptr offset, GLsizeiptr size);

/*!
 * \brief Store mutable data in buffer
 * \param buf Buffer to store into
 * \param data Pointer to data
 * \param size Size of data
 * \param usage Storage flag, determines storage location
 */
extern void coffee_graphics_buffer_store(CBuffer* buf, const void* data,
        GLsizeiptr size, GLenum usage);
/*!
 * \brief Store mutable data in buffer. This is a slower version for older GL versions.
 * \param buf Buffer to store into
 * \param data Pointer to data
 * \param size Size of data
 * \param usage Storage flag, determines storage location
 */
extern void coffee_graphics_buffer_store_safe(
        CBuffer* buf, const void* data,
        GLsizeiptr size, GLenum usage);

/*!
 * \brief Store immutable data in buffer.
 * \param buf Buffer to store into
 * \param data Pointer to data
 * \param size Size of data
 * \param usage Storage mask
 */
extern void coffee_graphics_buffer_store_immutable(
        CBuffer* buf, const void* data,
        GLsizeiptr size, BufferStorageMask usage);
/*!
 * \brief Store mutable data in buffer. This is a slower version for older GL versions.
 * \param buf Buffer to store into
 * \param data Pointer to data
 * \param size Size of data
 * \param usage Storage mask
 */
extern void coffee_graphics_buffer_store_immutable_safe(
        CBuffer* buf, const void* data,
        GLsizeiptr size, BufferStorageMask usage);

/*!
 * \brief coffee_graphics_buffer_substore
 * \param buf Buffer to be stored into
 * \param data Pointer to data
 * \param offset Offset into GL buffer
 * \param size Size of data
 */
extern void coffee_graphics_buffer_substore(
        CBuffer* buf, const void* data,
        GLsizeiptr offset, GLsizeiptr size);
/*!
 * \brief This version uses a more widely available set of commands, but is generally slower. Binds and unbinds buffer in order to work.
 * \param buf Buffer to be stored into
 * \param data Pointer to data
 * \param offset Offset into GL buffer
 * \param size Size of data
 *
 */
extern void coffee_graphics_buffer_substore_safe(
        CBuffer* buf, const void* data,
        GLsizeiptr offset, GLsizeiptr size);

/*!
 * \brief Bind range of buffer to an indexed target
 * \param buf The buffer used as source
 * \param index Index of the target
 * \param bufferType Type of buffer to bind to
 * \param offset Offset into source buffer
 * \param size Size in source buffer
 */
extern void coffee_graphics_buffer_bind_range(
        CBuffer* buf,
        GLuint index, GLenum bufferType,
        GLsizeiptr offset, GLsizeiptr size);

/*!
 * \brief Copies buffer data, using buffer handles
 * \param buf1 Source buffer handle
 * \param buf2 Target buffer handle
 * \param offset1 Source offset
 * \param offset2 Target offset
 * \param size Size of data
 */
extern void coffee_graphics_buffer_copy(
        GLuint buf1, GLuint buf2,
        GLsizeiptr offset1, GLsizeiptr offset2,
        GLsizeiptr size);

/*!
 * \brief Copies buffer data, using buffer handles. This is a slower version for older GL versions.
 * \param buf1 Source buffer handle
 * \param buf2 Target buffer handle
 * \param offset1 Source offset
 * \param offset2 Target offset
 * \param size Size of data
 */
extern void coffee_graphics_buffer_copy_safe(
        GLuint buf1, GLuint buf2,
        GLsizeiptr offset1, GLsizeiptr offset2,
        GLsizeiptr size);

/*!
 * \brief Copies buffer data
 * \param buf1 Source buffer
 * \param buf2 Target buffer
 * \param offset1 Source offset
 * \param offset2 Target offset
 * \param size Size of data
 */
extern void coffee_graphics_buffer_copy(
        CBuffer* buf1, CBuffer* buf2,
        GLsizeiptr offset1, GLsizeiptr offset2,
        GLsizeiptr size);

/*!
 * \brief Copies buffer data. This is a slower version for older GL versions.
 * \param buf1 Source buffer
 * \param buf2 Target buffer
 * \param offset1 Source offset
 * \param offset2 Target offset
 * \param size Size of data
 */
extern void coffee_graphics_buffer_copy_safe(
        CBuffer* buf1, CBuffer* buf2,
        GLsizeiptr offset1, GLsizeiptr offset2,
        GLsizeiptr size);

/*!
 * \brief This operation will create a new buffer, copy the specified data and delete the old buffer.
 * \param buf Target buffer
 * \param oldOffset Offset for data in old buffer
 * \param oldSize Size of data in old buffer
 * \param targetOffset Offset for old data in new buffer (end of data should be within buffer)
 * \param newSize Size of the new buffer
 */
extern void coffee_graphics_buffer_resize(
        CBuffer* buf,
        GLsizeiptr oldOffset, GLsizeiptr oldSize,
        GLsizeiptr targetOffset, GLsizeiptr newSize);

/*!
 * \brief Invalidates contents of buffer, often used with mapping to allow faster discarding
 * \param buf Target buffer
 */
extern void coffee_graphics_buffer_invalidate(CBuffer* buf);

/*!
 * \brief Invalidates contents of buffer, often used with mapping to allow faster discarding
 * \param buf Target buffer
 */
extern void coffee_graphics_buffer_invalidate_safe(CBuffer* buf);

/*!
 * \brief Binds a range
 * \param buf Subbuffer target
 */
extern void coffee_graphics_buffer_sub_bind(CSubBuffer* buf);

/*!
 * \brief Unbinds a range
 * \param buf Subbuffer target
 */
extern void coffee_graphics_buffer_sub_unbind(CSubBuffer* buf);

/*!
 * \brief If parent is a mapped buffer, return an offset pointer
 * \param buf Subbuffer target
 * \return A pointer to this subbuffer's data
 */
extern void* coffee_graphics_buffer_sub_data(CSubBuffer* buf);


typedef void* (*SubBufferData)(CSubBuffer*);
typedef void  (*BufferCopy)(CBuffer*, CBuffer*,GLsizeiptr, GLsizeiptr,GLsizeiptr);
typedef void  (*BufferStore)(CBuffer*, const void*,GLsizeiptr, GLenum);
typedef void  (*BufferStoreImmutable)(CBuffer*, const void*,GLsizeiptr,BufferStorageMask);
typedef void  (*BufferSubStore)(CBuffer*, const void*,GLsizeiptr, GLsizeiptr);
typedef void* (*BufferMap)(CBuffer*,BufferAccessMask);
typedef bool  (*BufferUnmap)(CBuffer*);
typedef void* (*BufferDownload)(CBuffer*, GLsizeiptr,GLsizeiptr);
typedef void  (*BufferInvalidate)(CBuffer*);

//typedef std::function<void*(CSubBuffer*)> SubBufferData;
//typedef std::function<void(CBuffer*, CBuffer*,
//                           GLsizeiptr, GLsizeiptr,GLsizeiptr)> BufferCopy;
//typedef std::function<void(CBuffer*, const void*,
//                           GLsizeiptr, GLenum)> BufferStore;
//typedef std::function<void(CBuffer*, const void*,GLsizeiptr,
//                           BufferStorageMask)> BufferStoreImmutable;
//typedef std::function<void(CBuffer*, const void*,GLsizeiptr, GLsizeiptr)> BufferSubStore;
//typedef std::function<void*(CBuffer*,BufferAccessMask)> BufferMap;
//typedef std::function<void(CBuffer*)> BufferUnmap;
//typedef std::function<void(CBuffer*, GLsizeiptr,GLsizeiptr)> BufferDownload;
//typedef std::function<void(CBuffer*)> BufferInvalidate;

struct CBufferFunctionBinds
{
    BufferStore             store;/*!< Function for storing data */
    BufferCopy              copy;/*!< Function for copying data */
    BufferStoreImmutable    store_immutable;/*!< Function for storing immutable data */
    BufferSubStore          substore;/*!< Function for storing sections of data */
    BufferMap               map;/*!< Function for mapping buffer */
    BufferUnmap             unmap;/*!< Function for unmapping buffer */
    BufferDownload          download;/*!< Function for downloading data */
    BufferInvalidate        invalidate;/*!< Function for invalidating data */
    SubBufferData           subdata;/*!< Function for acquiring pointer to subdata */
};

}
}

#endif // CBUFFER

