#ifndef CUNIFORMTYPES
#define CUNIFORMTYPES

#include <coffee/core/types/vector_types.h>
#include "copengl_types.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CUniform : _cbasic_graphics_resource_desc {};
/*!
 * \brief The uniform location for a fitting subroutine
 */
struct CUniformSubroutine : _cbasic_graphics_resource_desc {};
/*!
 * \brief The handle for a shader subroutine, usable with CUniformSubroutine
 */
struct CShaderSubroutine : _cbasic_graphics_resource_desc {};
/*!
 * \brief Represents a uniform block interface
 */
typedef _cbasic_graphics_resource_desc CUniformBlock;
/*!
 * \brief Represents a shader storage buffer interface
 */
typedef _cbasic_graphics_resource_desc CShaderStorageBuffer;

/*!
 * \brief A binding of a uniform
 */
typedef _cbasic_graphics_resource_binding<_cbasic_graphics_buffer_section> CUniformBlockBinding;
typedef _cbasic_graphics_resource_binding<_cbasic_graphics_buffer_section> CShaderStorageBufferBinding;

//Subroutines

extern void coffee_graphics_uniform_subroutine_get(
        const _cbasic_graphics_shader_program& shader,
        CUniformSubroutine &routine);

extern void coffee_graphics_subroutine_get(
        const _cbasic_graphics_shader_program& shader,
        CShaderSubroutine& routine);
/*!
 * \brief Applies a subroutine to the uniform routine value specified
 * \param shader
 * \param routine
 * \param routineUniform
 */
extern void coffee_graphics_uniform_subroutine_set(
        _cbasic_graphics_shader_program& shader,
        const CShaderSubroutine& routine,
        const CUniformSubroutine& routineUniform);

//Uniforms

/*!
 * \brief Get the index of a uniform value
 * \param shader
 * \param uniform
 */
extern void coffee_graphics_uniform_get(
        const _cbasic_graphics_shader_program& shader,
        CUniform& uniform);

//Uniform blocks

/*!
 * \brief Get the index of a uniform block
 * \param shader
 * \param uBlock
 */
extern void coffee_graphics_uniform_block_get(
        const _cbasic_graphics_shader_program& shader,
        CUniformBlock& uBlock);

/*!
 * \brief Set binding index for uniform block
 * \param prg Program to set index to
 * \param block Uniform block to set, contains binding index
 */
extern void coffee_graphics_uniform_block_set(
        _cbasic_graphics_shader_program& prg,
        const CUniformBlockBinding &block);

//Shader storage blocks

extern void coffee_graphics_shaderstoragebuffer_get(
        const _cbasic_graphics_shader_program& shader,
        CShaderStorageBuffer& sBuffer);

extern void coffee_graphics_shaderstoragebuffer_set(
        _cbasic_graphics_shader_program& prg,
        const CShaderStorageBufferBinding &block);

//Texture handles

extern void coffee_graphics_uniform_set_texhandle(
        const _cbasic_graphics_shader_program& prog,
        const CUniform& uniform,
        const CGuint64& value);

extern void coffee_graphics_uniform_set_texhandle_safe(
        const _cbasic_graphics_shader_program& prog,
        const CUniform &uniform,
        const CGuint& value);

}
}

#endif // CUNIFORMTYPES

