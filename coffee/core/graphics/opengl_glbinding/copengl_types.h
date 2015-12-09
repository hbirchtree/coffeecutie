#ifndef COPENGL_TYPES_H
#define COPENGL_TYPES_H

#include <bitset>

#include "coffee/core/types/basetypes.h"
#include "copengl_enums.h"

namespace Coffee{
namespace CGraphicsWrappers{

//Opaque types
struct CGLReport;

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

    cstring object_name;
    CGhnd index;
};

struct _cbasic_graphics_buffer_resource_desc
{
    _cbasic_graphics_buffer_resource_desc();

    cstring object_name;
    _cbasic_graphics_buffer_section* buffer;
    CGhnd index;
};

template<typename T>
struct _cbasic_graphics_resource_binding
{
    _cbasic_graphics_resource_binding():
        object(nullptr),
        binding(0)
    {
    }

    T* object;
    CGhnd binding;
};

typedef _cbasic_graphics_resource_binding<_cbasic_graphics_buffer_resource_desc>
    _cbasic_graphics_resource_binding_generic;

}
}

#endif
