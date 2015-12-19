#ifndef COPENGL_TYPES_H
#define COPENGL_TYPES_H

#include <bitset>

#include "coffee/core/types/types.h"
#include "copengl_enums.h"

namespace Coffee{
namespace CGraphicsWrappers{

//Debug stuff
struct CGLDebugMessage
{
    CDebugSeverity severity;
    CDebugSource source;
    CDebugType type;
    uint32 id;
    CString message;
};

typedef _cbasic_version<int32> CGLVersion;

typedef void(*CDebugCallback)(CGLDebugMessage const&,void*);

//Type definitions
typedef int32 CGint;
typedef uint32 CGuint;

typedef int64 CGint64;
typedef uint64 CGuint64;

typedef int32 CGsize;
typedef int64 CGszptr;

/*!
 * \brief A handle for a GL resource
 */
typedef uint32 CGhnd;
typedef uint64 CGhnd64;
/*!
 * \brief An index retrieved from GL or to be used in GL
 */
typedef uint32 CGidx;


/*!
 * \brief GL multidraw indirect call used to put triangles on the screen
 */
struct CGLDrawCall
{
    CGLDrawCall();

    CGuint count; /*!< Number of indices*/
    CGuint instanceCount; /*!< Number of instances*/
    CGuint firstIndex; /*!< First index in buffer*/
    CGuint baseVertex; /*!< First vertex in buffer*/
    CGuint baseInstance; /*!< First instance to render*/
};


//Misc. data structures

struct _cbasic_graphics_resource
{
    CGhnd handle;
};

struct _cbasic_graphics_shader_program
{
    _cbasic_graphics_shader_program();

    CGhnd handle;
    union{
        CProgramStage stages;
        CProgramStage stage;
    };
};

struct _cbasic_graphics_buffer_mappable
{
    void* data;
    CBufferStorage flags;
    CBufferAccess mapflags;
    CGsize size;
    CGhnd handle;
    CBufferType type;
};

struct _cbasic_graphics_buffer_section
{
    _cbasic_graphics_buffer_mappable* parent;
    CGszptr offset;
    CGsize size;
    CGhnd handle;
    CBufferType type;
};

struct _cbasic_graphics_resource_desc
{
    _cbasic_graphics_resource_desc();

    /*!
     * \brief Object name or identifier
     */
    cstring object_name;
    /*!
     * \brief Object index
     */
    CGhnd index;
};

template<typename T>
struct _cbasic_graphics_resource_binding
{
    _cbasic_graphics_resource_binding():
        desc(nullptr),
        resource(nullptr),
        index(0)
    {
    }

    /*!
     * \brief Descriptor of the resource or its destination
     */
    _cbasic_graphics_resource_desc* desc;
    /*!
     * \brief Resource to be bound
     */
    T* resource;
    /*!
     * \brief Binding index for the resource
     */
    CGidx index;
};

}
}

#endif
