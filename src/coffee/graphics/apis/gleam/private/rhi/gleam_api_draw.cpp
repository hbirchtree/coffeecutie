#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>

#include "gleam_internal_types.h"
#include <algorithm>
#include <coffee/core/CProfiling>
#include <coffee/graphics/apis/gleam/rhi/gleam_data.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_profile_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_vertex_rhi.h>
#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace RHI {
namespace GLEAM {

using draw_hash = Tup<GLEAM_API::PSTATE*, GLEAM_API::V_DESC*, TypeEnum>;

using instance_hash = Tup<u32, u32, u32, u32>;

static draw_hash hash_draw(GLEAM_API::RenderPass::DrawCall const& drawCall)
{
    auto const& draw = drawCall.d_data;

    /* TODO: Use a proper, lightweight hash */

    return std::make_tuple(
        drawCall.state, drawCall.vertices.lock().get(), draw.m_eltype);
}

static instance_hash instance_hash_draw(
    GLEAM_API::RenderPass::DrawCall const& drawCall)
{
    return std::make_tuple(
        drawCall.d_data.m_elems,
        drawCall.d_data.m_eoff,
        drawCall.d_data.m_verts,
        drawCall.d_data.m_voff);
}

void GLEAM_API::OptimizeRenderPass(
    GLEAM_API::RenderPass& rpass, GLEAM_API::OPT_DRAW& buffer, u32 baseinstance)
{
    DProfContext _(GLM_API "Optimizing RenderPass");

    Map<draw_hash, Vector<RenderPass::DrawCall*>> draws;

    for(auto& draw : rpass.draws)
        draws[hash_draw(draw)].push_back(&draw);

    u32                          globalInstanceOffset = baseinstance;
    Vector<RenderPass::DrawCall> collectedDraws;
    collectedDraws.reserve(draws.size());

    for(auto& bucket : draws)
    {
        /* Sort by same mesh for instancing */
        Map<instance_hash, Vector<RenderPass::DrawCall*>> instance_draws;
        for(auto draw : bucket.second)
            instance_draws[instance_hash_draw(*draw)].push_back(draw);

        for(auto const& instance : instance_draws)
        {
            D_DATA collected  = instance.second.front()->d_data;
            collected.m_insts = 0;
            collected.m_ioff  = globalInstanceOffset;
            for(auto& draw : instance.second)
            {
                draw->d_data.m_ioff = globalInstanceOffset + collected.m_insts;
                collected.m_insts += draw->d_data.m_insts;
            }

            globalInstanceOffset += collected.m_insts;

            collectedDraws.push_back(*instance.second.front());
            collectedDraws.back().d_data = collected;
        }
        bucket.second.clear();
    }

    Map<ShPtr<V_DESC>, Vector<RenderPass::DrawCall*>> vert_sort;
    for(auto& call : collectedDraws)
    {
        vert_sort[call.vertices.lock()].push_back(&call);
    }

    auto& cmdBufs = buffer.cmdBufs;
    for(auto& group : vert_sort)
    {
        Map<PSTATE*, Vector<RenderPass::DrawCall*>> ustate_sort;

        for(auto& call : group.second)
        {
            ustate_sort[call->state].push_back(call);
        }

        cmdBufs.reserve(cmdBufs.size() + ustate_sort.size());

        for(auto& ustate_group : ustate_sort)
        {
            cmdBufs.push_back(
                CommandBuffer{group.first, ustate_group.first, {}});
            auto& cmd_buf = cmdBufs.back();

            cmd_buf.commands.reserve(ustate_group.second.size());

            for(auto& call : ustate_group.second)
                cmd_buf.commands.push_back({call->d_call, call->d_data});
        }
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    if(GLEAM_FEATURES.gles20)
        return;

    auto& mdData = buffer.multiDrawData;

    for(auto& buffer : cmdBufs)
    {
        auto& data = mdData[&buffer];

        /* TODO: Add functionality to determine whether we are
         *  using Indirect or not */

        data.counts.reserve(buffer.commands.size());
        data.offsets.reserve(buffer.commands.size());
        data.baseVertex.reserve(buffer.commands.size());
        data.indirectCalls.reserve(buffer.commands.size());

        for(auto& cmd : buffer.commands)
        {
            if(cmd.call.indexed())
                data.counts.push_back(cmd.data.elements());
            else
                data.counts.push_back(cmd.data.vertices());
            data.offsets.push_back(0);
            data.baseVertex.push_back(cmd.data.vertexOffset());

            data.indirectCalls.push_back({});
            auto& r = data.indirectCalls.back();
            if(cmd.call.indexed())
            {
                r.i.count         = cmd.data.elements();
                r.i.instanceCount = cmd.data.instances();
                r.i.firstIndex    = cmd.data.indexOffset();
                r.i.baseVertex    = C_FCAST<u32>(cmd.data.vertexOffset());
                r.i.baseInstance  = cmd.data.instanceOffset();
            } else
            {
                r.a.count         = cmd.data.vertices();
                r.a.instanceCount = cmd.data.instances();
                r.a.first         = C_FCAST<u32>(cmd.data.vertexOffset());
                r.a.baseInstance  = cmd.data.instanceOffset();
            }

            data.etype = cmd.data.elementType();
            data.dc    = cmd.call;
        }
    }
#endif
}

/*!
 * \brief The internal drawing function, making the assumption that
 *  all state is ready.
 * \param mode The primitive drawing mode
 * \param d Drawcall information
 * \param i Drawcall data, offsets and etc.
 * \return true if drawcall was possible on the current API
 */
static bool InternalDraw(
    glhnd const&                       pipelineHandle,
    DrwMd const&                       mode,
    GLEAM_API::DrawCall const&         d,
    GLEAM_API::DrawInstanceData const& i,
    gleam_error&                       ec)
{
    DPROF_CONTEXT_FUNC(GLM_API);
    // TODO: Use glGetVertexAttribPointer for vertex offsets

    if(d.indexed())
    {
        if(GL_DEBUG_MODE)
        {
            i32 elementHnd = 0;
            gl::vlow::IntegerGetv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementHnd);

            if(elementHnd == 0)
                ec = APIError::DrawNoIndexBuffer;
        }

        szptr elsize;

        if(i.elementType() == TypeEnum::UByte)
            elsize = 1;
        else if(i.elementType() == TypeEnum::UShort)
            elsize = 2;
        else if(i.elementType() == TypeEnum::UInt)
            elsize = 4;
        else
            ec = APIError::InvalidElementType;

        if(ec)
            return false;

        if(d.instanced())
        {
            /* TODO: Implement the disabled drawcalls using other means */
#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
            if(GLEAM_FEATURES.draw_base_instance && i.instanceOffset() > 0 &&
               i.vertexOffset() != 0)

                gl::v42::DrawElementsInstancedBaseVertexBaseInstance(
                    mode,
                    i.elements(),
                    i.elementType(),
                    i.indexOffset() * elsize,
                    i.instances(),
                    i.vertexOffset(),
                    i.instanceOffset());
#endif
#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
            else if(GLEAM_FEATURES.draw_base_instance && i.instanceOffset() > 0)

                gl::v42::DrawElementsInstancedBaseInstance(
                    mode,
                    i.elements(),
                    i.elementType(),
                    i.indexOffset() * elsize,
                    i.instanceOffset(),
                    i.instances());
            else
#endif
            {
#if GL_VERSION_VERIFY(0x320, 0x320)
                if(!GLEAM_FEATURES.gles20 && i.vertexOffset() > 0)
                    gl::v33::DrawElementsInstancedBaseVertex(
                        mode,
                        i.elements(),
                        i.elementType(),
                        i.indexOffset() * elsize,
                        i.instances(),
                        i.vertexOffset());
                else
#endif
#if GL_VERSION_VERIFY(0x310, 0x300)
                    if(!GLEAM_FEATURES.gles20)
                    gl::v33::DrawElementsInstanced(
                        mode,
                        i.elements(),
                        i.elementType(),
                        i.indexOffset() * elsize,
                        i.instances());
                else
#endif
                    gl::v33::DrawElements(
                        mode,
                        i.elements(),
                        i.elementType(),
                        i.indexOffset() * elsize);
            }
        } else
        {
#if GL_VERSION_VERIFY(0x320, 0x320)
            if(i.vertexOffset() > 0)
                gl::v33::DrawElementsBaseVertex(
                    mode,
                    i.elements(),
                    i.elementType(),
                    i.indexOffset() * elsize,
                    i.vertexOffset());
            else
#endif
                gl::v33::DrawElements(
                    mode,
                    i.elements(),
                    i.elementType(),
                    i.indexOffset() * elsize);
        }

    } else
    {
#if GL_VERSION_VERIFY(0x310, 0x300)
        if(d.instanced())
            gl::v33::DrawArraysInstanced(
                mode, i.vertexOffset(), i.vertices(), i.instances());
        else
#endif
            gl::v33::DrawArrays(mode, i.vertexOffset(), i.vertices());
    }

    return true;
}

#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
bool InternalMultiDraw(
    GLEAM_API::OptimizedDraw::MultiDrawData const& data, gleam_error& ec)
{
    DPROF_CONTEXT_FUNC(GLM_API);

#if MODE_DEBUG
    platform::profiling::GpuProfilerContext<GLEAM_TimeQuery> __(
        __FUNCTION__, GLEAM_API_INSTANCE_DATA->queries.at(0));
#endif

    if(GL_DEBUG_MODE)
    {
        if(data.dc.indexed())
        {
            i32 elementHnd = 0;
            gl::vlow::IntegerGetv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementHnd);

            if(elementHnd == 0)
                ec = APIError::DrawNoIndexBuffer;
        }

        if(data.counts.size() == 0)
            ec = APIError::DrawNoMeshes;

        szptr valid = 0;
        for(auto c : data.counts)
            if(c > 0)
                valid++;
        if(valid == 0)
            ec = APIError::DrawNullsizedMeshes;

        for(auto const& call : data.indirectCalls)
            if(data.dc.indexed())
            {
                if(call.i.instanceCount == 0)
                    ec = APIError::DrawNoInstances;
                if(call.i.count == 0)
                    ec = APIError::DrawNoVertices;
            } else
            {
                if(call.a.instanceCount == 0)
                    ec = APIError::DrawNoInstances;
                if(call.a.count == 0)
                    ec = APIError::DrawNoVertices;
            }
    }

    using IndirectCall = GLEAM_API::OptimizedDraw::IndirectCall;

    /* TODO: Add more MultiDraw* support */

    /* TODO: Implement fallbacks using the pseudo-implementations */

    /* TODO: Add fallbacks on Draw*Indirect */

    DrwMd mode = {data.dc.primitive(), data.dc.primitiveMode()};

    if(ec)
        return false;

    const bool indexed = data.dc.indexed();
    const bool multi_indirect =
        data.dc.instanced() && GLEAM_FEATURES.draw_multi_indirect;

    if(multi_indirect)
    {
        auto& indirectCalls =
            *C_CCAST<Vector<GLEAM_API::OptimizedDraw::IndirectCall>*>(
                &data.indirectCalls);

        GLEAM_API_INSTANCE_DATA->indirectBuf->commit(
            BytesConst::CreateFrom(indirectCalls));

        GLEAM_API_INSTANCE_DATA->indirectBuf->bind();
    }

    if(indexed && multi_indirect)
        gl::v43::MultiDrawElementsIndirect(
            mode,
            data.etype,
            0,
            data.indirectCalls.size(),
            sizeof(data.indirectCalls[0]));
    else if(indexed && !data.dc.instanced())
        gl::v33::MultiDrawElementsBaseVertex(
            mode,
            data.counts.data(),
            data.etype,
            C_RCAST<uintptr>(data.offsets.data()),
            data.counts.size(),
            data.baseVertex.data());
    else if(!data.dc.indexed() && multi_indirect)
        gl::v43::MultiDrawArraysIndirect(
            mode, 0, data.indirectCalls.size(), sizeof(IndirectCall));
    else if(!data.dc.indexed() && !data.dc.instanced())
        gl::v33::MultiDrawArrays(
            mode,
            C_RCAST<const i32*>(data.offsets.data()),
            data.counts.data(),
            data.offsets.size());
    else
        ec = APIE::DrawNotCompatible;

    GLEAM_API_INSTANCE_DATA->indirectBuf->unbind();

    return true;
}
#endif

/*!
 * \brief Add a substitute for the gl_BaseInstanceARB variable on
 *  GLES and GL 3.3
 */
static void GetInstanceUniform(
    GLEAM_API::PIP const& pipeline,
    cstring               unifName,
    i32&                  uloc,
    glhnd&                handle,
    gleam_error&          error_code)
{
    if(GLEAM_FEATURES.base_instance)
        return;

    /* TODO: Cache the uniform location */

    if(!GLEAM_FEATURES.separable_programs)
    {
        uloc = gl::v33::UnifGetLocation(pipeline.pipelineHandle(), unifName);
    }
#if GL_VERSION_VERIFY(0x430, 0x310)
    else
    {
        auto& hnd = handle;

        for(auto const& cnt : pipeline.internalHandles())
            if(cnt.stages == ShaderStage::Vertex)
                hnd = cnt.shader->internalHandle();

        auto raw_uloc =
            gl::v43::ProgramGetResourceLocation(hnd, GL_UNIFORM, unifName);

        uloc = C_FCAST<i32>(raw_uloc);
    }
#else
    else
        Throw(implementation_error("bad render path"));
#endif
}

static void SetInstanceUniform(
    glhnd const& hnd, i32 uloc, u32 baseInstance, gleam_error& ec)
{
    i32 baseInstance_i = C_FCAST<i32>(baseInstance);

    if(!GLEAM_FEATURES.separable_programs)
    {
        gl::v33::Unifiv(uloc, baseInstance_i);
    }
#if GL_VERSION_VERIFY(0x410, 0x310)
    else
    {
        gl::v41::Unifiv(hnd, C_FCAST<i32>(uloc), baseInstance_i);
    }
#else
    else
        Throw(implementation_error("bad render path"));
#endif
}

void GLEAM_API::MultiDraw(
    const GLEAM_API::PIP&      pipeline,
    const GLEAM_API::OPT_DRAW& draws,
    gleam_error&               ec)
{
    DPROF_CONTEXT_FUNC(GLM_API);
    GLEAM_API::DBG::SCOPE a(GLM_API "MultiDraw");

    /* In debug mode, display the entire draw call.
     *  This is the true verbose mode. */
#ifndef COFFEE_LOWFAT
    if(GL_DEBUG_MODE && PrintingVerbosityLevel() >= 12)
    {
        cVerbose(12, GLM_API "- Pipeline:{0}", pipeline.m_handle.hnd);
        for(auto& cmd : draws.cmdBufs)
        {
            cVerbose(
                12,
                GLM_API "-- Vertices:{0} + UState:{1}",
                &cmd.vertices,
                &cmd.state);
            for(auto& call : cmd.commands)
            {
                cVerbose(12, GLM_API "--- Draw call:{0}", &call);
            }
        }
    }
#endif

    {
        DProfContext _(GLM_API "Binding pipeline");
        /* The important limiting factor is the speed of
         *  binding new shader pipelines. */
        pipeline.bind();
    }

    /* TODO: Add fallback support for gl_DrawID */

    /* We will use these to allow use of existing state.
     * We will treat program uniform state as atomic,
     *  because it can be a lot to compare */
    ShPtr<V_DESC> p_vertices;
    PSTATE*       p_state      = nullptr;
    i32           vertexOffset = 0;

    glhnd vertexHandle(0);
    i32   baseInstanceLoc = -1;
    i32   instanceID_loc  = -1;
    u32   instanceID_val  = 0;

    {
        DProfContext _(GLM_API "Querying shader uniforms");
        /* For multiple instances, BaseInstance helps a lot */
        if(!GLEAM_FEATURES.base_instance)
            GetInstanceUniform(
                pipeline, "BaseInstance", baseInstanceLoc, vertexHandle, ec);

        /* If using a platform without instancing, cheat by using a
         *  uniform in its place. Preprocessor macros will handle the rest. */
        if(GLEAM_FEATURES.gles20)
            GetInstanceUniform(
                pipeline, "InstanceID", instanceID_loc, vertexHandle, ec);
    }

#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)

    /* For pure GL 3.3 platforms, we must assert that
     *  instancing is not necessary */
    bool call_instanced;

    {
        DProfContext _(GLM_API "Querying drawcalls");

        auto it = std::find_if(
            draws.multiDrawData.begin(),
            draws.multiDrawData.end(),
            [](OptMap::value_type const& e) {
                return e.second.dc.instanced();
            });

        call_instanced = it != draws.multiDrawData.end();
    }

    /* We assume that, if no multiDrawData is available,
     *  we can't use it */
    if(draws.multiDrawData.size() &&
       (GLEAM_FEATURES.draw_multi_indirect || !call_instanced))
    {
        if(GL_DEBUG_MODE && draws.multiDrawData.size() == 0)
            ec = APIE::DrawNoMeshes;

        for(auto& mdData : draws.multiDrawData)
        {
            auto& buffer = *mdData.first;

            auto tmp_vert = unwrap_ptr(buffer.vertices);

            if(tmp_vert != p_vertices)
            {
                GLEAM_API::DBG::SCOPE b(GLM_API "VBuffer::bind");
                p_vertices = tmp_vert;
                p_vertices->bind();
            }

            if(buffer.state != p_state)
            {
                GLEAM_API::DBG::SCOPE b(GLM_API "SetShaderUniformState");
                p_state = buffer.state;
                for(auto const& s : *p_state)
                    SetShaderUniformState(pipeline, s.first, *s.second, ec);
            }

            InternalMultiDraw(mdData.second, ec);
        }
    } else
#endif
        /* This method minimizes the amount of state changes only,
         *  draw calls are still a couple, but it's faster than
         *  multiple calls to Draw() */
        for(auto& buffer : draws.cmdBufs)
        {
            auto tmp_vert = unwrap_ptr(buffer.vertices);

            if(tmp_vert != p_vertices)
            {
                GLEAM_API::DBG::SCOPE b(GLM_API "VBuffer::bind");
                p_vertices = tmp_vert;
                p_vertices->bind();
            }

            if(buffer.state != p_state)
            {
                GLEAM_API::DBG::SCOPE b(GLM_API "SetShaderUniformState");
                p_state = buffer.state;
                for(auto const& s : *p_state)
                    SetShaderUniformState(pipeline, s.first, *s.second, ec);
            }

            for(auto& cmd : buffer.commands)
            {
                DrwMd mode = {cmd.call.primitive(), cmd.call.primitiveMode()};

                if(!GLEAM_FEATURES.base_instance)
                    SetInstanceUniform(
                        vertexHandle,
                        baseInstanceLoc,
                        cmd.data.instanceOffset(),
                        ec);
                if(GLEAM_FEATURES.gles20)
                {
                    SetInstanceUniform(
                        vertexHandle, instanceID_loc, instanceID_val, ec);
                    /* For the vertex offset, we bind the attributes
                     *  again with a new pointer */
                    if(cmd.data.vertexOffset() != vertexOffset)
                    {
                        vertexOffset = cmd.data.vertexOffset();
                        p_vertices->bind(vertexOffset);
                    }
                }
                if(GLEAM_FEATURES.gles20 && cmd.data.instances() > 1)
                {
                    for(u32 i = 0; i < cmd.data.instances(); i++)
                    {
                        if(i != 0)
                            SetInstanceUniform(
                                vertexHandle, instanceID_loc, i, ec);
                        InternalDraw(
                            pipeline.m_handle, mode, cmd.call, cmd.data, ec);
                    }
                } else
                    InternalDraw(
                        pipeline.m_handle, mode, cmd.call, cmd.data, ec);
            }
        }

    if(GL_DEBUG_MODE && ec)
    {
        gl::common::DebugMessageInsert(
            Severity::Critical,
            debug::Type::Other,
            ec.category().message(ec.value()).c_str());
    }

    vertexHandle.release();
    pipeline.unbind();
}

void GLEAM_API::Draw(
    const GLEAM_Pipeline&   pipeline,
    PipelineState const&    ustate,
    V_DESC&                 vertices,
    const DrawCall&         d,
    const DrawInstanceData& i,
    gleam_error&            ec,
    OccludeQuery*           query)
{
    DPROF_CONTEXT_FUNC(GLM_API);

    DrwMd mode = {d.primitive(), d.primitiveMode()};

    if(query)
        query->begin();

    pipeline.bind(ec);

    for(auto const& s : ustate)
    {
        SetShaderUniformState(pipeline, s.first, *s.second, ec);
    }

    vertices.bind();

#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x330)
    if(m_store->features.qcom_tiling)
    {
        auto size = GLEAM_API::DefaultFramebuffer()->size();
        glStartTilingQCOM(
            0,
            0,
            size.w,
            size.h,
            GL_COLOR_BUFFER_BIT0_QCOM | GL_DEPTH_BUFFER_BIT0_QCOM);
    }
#endif

    InternalDraw(pipeline.m_handle, mode, d, i, ec);

    if(GL_DEBUG_MODE && ec)
    {
        gl::common::DebugMessageInsert(
            Severity::Critical,
            debug::Type::Other,
            "GLEAM context error: " + ec.message());
    }

#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x330)
    if(m_store->features.qcom_tiling)
        glEndTilingQCOM(GL_COLOR_BUFFER_BIT0_QCOM);
#endif

    if(query)
        query->end();

    vertices.unbind();
    pipeline.unbind(ec);
}

void GLEAM_API::DrawConditional(
    C_UNUSED(const GLEAM_Pipeline& pipeline),
    C_UNUSED(const PipelineState& ustate),
    C_UNUSED(V_DESC& vertices),
    C_UNUSED(const DrawCall& d),
    C_UNUSED(const DrawInstanceData& i),
    C_UNUSED(OccludeQuery& c),
    C_UNUSED(gleam_error& ec))
{
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    /*TODO: Implement use of GL_QUERY_RESULT_AVAILABLE for GLES path */
    gl::v33::ConditionalRenderBegin(c.m_handle, Delay::Wait);
    Draw(pipeline, ustate, vertices, d, i, ec);
    gl::v33::ConditionalRenderEnd();
#endif
}

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
