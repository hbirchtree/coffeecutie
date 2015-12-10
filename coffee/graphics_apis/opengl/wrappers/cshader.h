#ifndef COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
#define COFFEE_CGRAPHICSWRAPPERS_CSHADER_H

#include "coffee/core/types/composite_types.h"

#include "copengl_types.h"
#include "cuniformtypes.h"

#include "coffee/core/base/cfiles.h"

namespace Coffee {
namespace CGraphicsWrappers {

struct CPipeline : public _cbasic_graphics_shader_program{};
struct CShaderProgram : public _cbasic_graphics_shader_program{};
struct CShaderStageProgram : public _cbasic_graphics_shader_program{};
struct CShader : public _cbasic_graphics_shader_program{};

/*!
 * \brief Bind pipeline for rendering
 * \param pl Pipeline to bind
 */
extern void coffee_graphics_bind(CPipeline* pl);
/*!
 * \brief Unbind a pipeline to clear state
 * \param pl Arbitrary pipeline
 */
extern void coffee_graphics_unbind(CPipeline* pl);

/*!
 * \brief Allocate shader pipeline
 * \param pl Pipeline to allocate
 */
extern void coffee_graphics_alloc(CPipeline* pl);
/*!
 * \brief Allocate shader program
 * \param shd Program to allocate
 * \param separable Whether or not it should be separable
 */
extern void coffee_graphics_alloc(CShaderProgram* shd, bool separable);

/*!
 * \brief Restore shader program from file, fully linked if succeeded
 * \param prg Program to load to
 * \param rsc Resource to load from
 * \return Whether or not it succeeded
 */
extern bool coffee_graphics_restore(
        CShaderProgram* prg,CResources::CResource* rsc);
/*!
 * \brief Save shader program to file if GL supports it
 * \param prg Program to save from
 * \param rsc Resource to save to
 */
extern void coffee_graphics_store(
        CShaderProgram* prg,CResources::CResource* rsc);

/*!
 * \brief Free pipeline resource
 * \param pl Pipeline to free
 */
extern void coffee_graphics_free(CPipeline* pl);
/*!
 * \brief Free shader resource
 * \param shd Shader to free
 */
extern void coffee_graphics_free(CShader* shd);
/*!
 * \brief Free program resource
 * \param prg Program to free
 */
extern void coffee_graphics_free(CShaderProgram* prg);
/*!
 * \brief Free stage program resource
 * \param prg Stage program to free
 */
extern void coffee_graphics_free(CShaderStageProgram* prg);

extern bool coffee_graphics_shader_compile(
        CShader *prg, cstring rsc,
        CProgramStage type);

extern bool coffee_graphics_shader_compile(
        CShaderStageProgram *prg, cstring rsc,
        CProgramStage stage);

/*!
 * \brief Compile a shader from resource
 * \param prg Shader to compile
 * \param rsc Resource containing shader
 * \param stage Which stage you are compiling
 * \return
 */
extern bool coffee_graphics_shader_compile(
        CShader *prg, CResources::CResource *rsc,
        CProgramStage stage);
/*!
 * \brief Compile a shader stage program from resource
 * \param prg Program being compiled
 * \param res Resource containing shader
 * \param stage Which stage it contains
 * \return
 */
extern bool coffee_graphics_shader_compile(
        CShaderStageProgram* prg, CResources::CResource *res,
        CProgramStage stage);

/*!
 * \brief Link the shader program
 * \param prg Program to link
 */
extern void coffee_graphics_shader_link(CShaderProgram* prg);

/*!
 * \brief Attach shader stage to pipeline
 * \param pl Pipeline being attached to
 * \param stg Shader stage to attach
 * \param filter Mask for which stage to attach
 */
extern void coffee_graphics_shader_attach(
        CPipeline* pl, CShaderStageProgram* stg, CProgramStage filter);
/*!
 * \brief Attach shader program to pipeline
 * \param pl Pipeline being attached to
 * \param stg Shader program to attach
 * \param filter Mask for which stages to attach
 */
extern void coffee_graphics_shader_attach(
        CPipeline* pl, CShaderProgram* stg, CProgramStage filter);

extern void coffee_graphics_shader_attach(CShaderProgram* shd, CShader* stg);
/*!
 * \brief Detach a shader from a program, detaches all of them
 * \param shd Shader program being detached from
 * \param stg Shader to detach
 */
extern void coffee_graphics_shader_detach(
        CShaderProgram* shd, CShader* stg);

/*!
 * \brief Get shader index of uniform block
 * \param prg Program to acquire index from
 * \param name Name of the block
 * \param loc Pointer to which the location will be stored
 */
extern void coffee_graphics_shader_uniform_block_get(
        CShaderProgram* prg, cstring name, CGhnd* loc);
/*!
 * \brief Set binding index for uniform block
 * \param prg Program to set index to
 * \param block Uniform block to set, contains binding index
 */
extern void coffee_graphics_shader_uniform_block_set(CShaderProgram* prg, const CUniformBlockBinding &block);
/*!
 * \brief Get uniform value location in shader
 * \param prg Program to acquire location from
 * \param name Name of uniform value
 * \return Uniform value location
 */
extern CGhnd coffee_graphics_shader_uniform_value_get(CShaderProgram* prg, cstring name);

/*!
 * \brief For quick and simple shader setup
 */
class CSimplePipeline : public _cbasic_raii_container<CPipeline>
{
public:
    CSimplePipeline():
        _cbasic_raii_container(new CPipeline)
    {
    }
    virtual ~CSimplePipeline()
    {
        coffee_graphics_free(&vert);
        coffee_graphics_free(&frag);
        coffee_graphics_free(m_data);
        delete m_data;
    }

    void create(cstring vshader, cstring fshader)
    {
        coffee_graphics_alloc(m_data);

        coffee_graphics_shader_compile(&vert,vshader,CProgramStage::Vertex);
        coffee_graphics_shader_compile(&frag,fshader,CProgramStage::Fragment);

        coffee_graphics_shader_attach(m_data,&vert,CProgramStage::Vertex);
        coffee_graphics_shader_attach(m_data,&frag,CProgramStage::Fragment);
    }

    CShaderStageProgram vert;
    CShaderStageProgram frag;
};

} // namespace CGraphicsWrappers
} // namespace Coffee

#endif // COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
