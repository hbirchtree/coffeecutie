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

struct _cbasic_graphics_buffer_mappable
{
    void* data;
    BufferStorageMask flags;
    BufferAccessMask mapflags;
    GLsizeiptr size;
    GLuint handle;
    GLenum type;
};

struct _cbasic_graphics_buffer_section
{
    _cbasic_graphics_buffer_mappable* parent;
    GLsizeiptr offset;
    GLsizeiptr size;
    GLuint handle;
    GLenum type;
};

struct _cbasic_graphics_resource_desc
{
    _cbasic_graphics_resource_desc();

    cstring object_name;
    GLuint index;
};

struct _cbasic_graphics_buffer_resource_desc
{
    _cbasic_graphics_buffer_resource_desc();

    cstring object_name;
    _cbasic_graphics_buffer_section* buffer;
    GLuint index;
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
