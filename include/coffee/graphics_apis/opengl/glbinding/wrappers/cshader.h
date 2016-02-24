#ifndef COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
#define COFFEE_CGRAPHICSWRAPPERS_CSHADER_H

#include <coffee/core/types/composite_types.h>

#include "copengl_types.h"
#include "cuniformtypes.h"

#include <coffee/core/CFiles>

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
extern void coffee_graphics_bind(CPipeline& pl);
/*!
 * \brief Unbind a pipeline to clear state
 * \param pl Arbitrary pipeline
 */
extern void coffee_graphics_unbind(CPipeline&);

/*!
 * \brief Allocate shader pipeline
 * \param pl Pipeline to allocate
 */
extern void coffee_graphics_alloc(size_t count, CPipeline* pl);
/*!
 * \brief Allocate shader program
 * \param shd Program to allocate
 * \param separable Whether or not it should be separable
 */
extern void coffee_graphics_alloc(size_t count, CShaderProgram* shd, bool separable = true);

inline void coffee_graphics_alloc(CPipeline& pl)
{
    coffee_graphics_alloc(1,&pl);
}

inline void coffee_graphics_alloc(CShaderProgram& prg, bool separable = true)
{
    coffee_graphics_alloc(1,&prg,separable);
}

/*!
 * \brief Free pipeline resource
 * \param pl Pipeline to free
 */
extern void coffee_graphics_free(size_t count, CPipeline* pl);
/*!
 * \brief Free shader resource
 * \param shd Shader to free
 */
extern void coffee_graphics_free(size_t count, CShader* shd);
/*!
 * \brief Free program resource
 * \param prg Program to free
 */
extern void coffee_graphics_free(size_t count, CShaderProgram* prg);
/*!
 * \brief Free stage program resource
 * \param prg Stage program to free
 */
extern void coffee_graphics_free(size_t count, CShaderStageProgram* prg);

inline void coffee_graphics_free(CPipeline& pl)
{
    coffee_graphics_free(1,&pl);
}

inline void coffee_graphics_free(CShader& shd)
{
    coffee_graphics_free(1,&shd);
}

inline void coffee_graphics_free(CShaderProgram& prg)
{
    coffee_graphics_free(1,&prg);
}

inline void coffee_graphics_free(CShaderStageProgram& prg)
{
    coffee_graphics_free(1,&prg);
}

/*!
 * \brief Restore shader program from file, fully linked if succeeded
 * \param prg Program to load to
 * \param rsc Resource to load from
 * \return Whether or not it succeeded
 */
extern bool coffee_graphics_restore(
        CShaderProgram &prg, const CResources::CResource &rsc);
/*!
 * \brief Save shader program to file if GL supports it
 * \param prg Program to save from
 * \param rsc Resource to save to
 */
extern bool coffee_graphics_store(
        const CShaderProgram &prg, CResources::CResource &rsc);

extern bool coffee_graphics_shader_compile(
        CShader &prg, cstring rsc,
        const CProgramStage &type);

extern bool coffee_graphics_shader_compile(
        CShaderStageProgram &prg, cstring rsc,
        CProgramStage const& stage);

/*!
 * \brief Compile a shader from resource
 * \param prg Shader to compile
 * \param rsc Resource containing shader
 * \param stage Which stage you are compiling
 * \return
 */
extern bool coffee_graphics_shader_compile(
        CShader &prg, const CResources::CResource &rsc,
        const CProgramStage &stage);
/*!
 * \brief Compile a shader stage program from resource
 * \param prg Program being compiled
 * \param res Resource containing shader
 * \param stage Which stage it contains
 * \return
 */
extern bool coffee_graphics_shader_compile(
        CShaderStageProgram& prg, const CResources::CResource &res,
        CProgramStage const& stage);

/*!
 * \brief Link the shader program
 * \param prg Program to link
 */
extern void coffee_graphics_shader_link(CShaderProgram& prg);

/*!
 * \brief Attach shader stage to pipeline
 * \param pl Pipeline being attached to
 * \param stg Shader stage to attach
 * \param filter Mask for which stage to attach
 */
extern void coffee_graphics_shader_attach(
        CPipeline& pl, CShaderStageProgram& stg, CProgramStage const& filter);
/*!
 * \brief Attach shader program to pipeline
 * \param pl Pipeline being attached to
 * \param stg Shader program to attach
 * \param filter Mask for which stages to attach
 */
extern void coffee_graphics_shader_attach(
        CPipeline &pl, CShaderProgram &stg, const CProgramStage &filter);

extern void coffee_graphics_shader_attach(
        CShaderProgram& shd, CShader& stg);
/*!
 * \brief Detach a shader from a program, detaches all of them
 * \param shd Shader program being detached from
 * \param stg Shader to detach
 */
extern void coffee_graphics_shader_detach(
        CShaderProgram& shd, CShader& stg);

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
        coffee_graphics_free(vert);
        coffee_graphics_free(frag);
        coffee_graphics_free(*m_data);
        delete m_data;
    }

    void create(cstring vshader = nullptr, cstring fshader = nullptr, cstring gshader = nullptr, cstring tcshader = nullptr, cstring teshader = nullptr, cstring cshader = nullptr)
    {
        coffee_graphics_alloc(*m_data);

        if(vshader)
        {
            coffee_graphics_shader_compile(vert,vshader,CProgramStage::Vertex);
            coffee_graphics_shader_attach(data_ref(),vert,CProgramStage::Vertex);
        }
        if(fshader)
        {
            coffee_graphics_shader_compile(frag,fshader,CProgramStage::Fragment);
            coffee_graphics_shader_attach(data_ref(),frag,CProgramStage::Fragment);
        }
    }

    CShaderStageProgram vert;
    CShaderStageProgram frag;
};

} // namespace CGraphicsWrappers
} // namespace Coffee

#endif // COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
