#ifndef COPENGL_TYPES_H
#define COPENGL_TYPES_H

#include "coffee/core/coffee_basetypes.h"
#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct _cbasic_graphics_resource
{
    GLuint handle;
};

struct _cbasic_graphics_shader_program
{
    _cbasic_graphics_shader_program();

    GLuint handle;
    union{
        UseProgramStageMask stages;
        UseProgramStageMask stage;
    };
};

struct _cbasic_graphics_buffer_desc : _cbasic_graphics_resource
{
    GLsizeiptr size;
    GLenum type;
};

struct _cbasic_graphics_buffer_mappable : _cbasic_graphics_buffer_desc
{
    void* data;
    BufferStorageMask flags;
    BufferAccessMask mapflags;
};

struct _cbasic_graphics_buffer_section : _cbasic_graphics_buffer_desc
{
    _cbasic_graphics_buffer_mappable* parent;
    GLsizeiptr offset;
};

struct _cbasic_graphics_resource_desc
{
    _cbasic_graphics_resource_desc();

    cstring object_name;
    GLuint index;
};

struct _cbasic_graphics_buffer_resource_desc : _cbasic_graphics_resource_desc
{
    _cbasic_graphics_buffer_resource_desc();

    _cbasic_graphics_buffer_section* buffer;
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
    GLuint binding;
};

typedef _cbasic_graphics_resource_binding<_cbasic_graphics_buffer_resource_desc>
    _cbasic_graphics_resource_binding_generic;

}
}

#endif
