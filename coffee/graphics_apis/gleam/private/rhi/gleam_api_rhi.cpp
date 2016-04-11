#include <coffee/graphics_apis/gleam/rhi/gleam_api_rhi.h>

#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

void GLEAM_API::LoadAPI()
{
    instance_data = new GLEAM_Instance_Data;

    {
        const szptr num_pbos = 5;

        CGhnd* bufs = new CGhnd[num_pbos];
        CGL33::BufAlloc(num_pbos,bufs);

        instance_data->pboQueue.buffers.reserve(num_pbos);
        for(uint32 i=0;i<num_pbos;i++)
        {
            GLEAM_PboQueue::Pbo pbo;
            pbo.buf = bufs[i];
            instance_data->pboQueue.buffers.push_back(pbo);
        }
        delete[] bufs;
    }

    auto ver = CGL_Implementation::Debug::ContextVersion();
    const Display::CGLVersion ver33(3,3);
    const Display::CGLVersion ver43(4,3);
    const Display::CGLVersion ver45(4,5);

    if(ver==ver33)
        GL_CURR_API = GL_3_3;
    else if(ver==ver43)
        GL_CURR_API = GL_4_3;
    else if(ver==ver45)
        GL_CURR_API = GL_4_5;
}

void GLEAM_API::SetRasterizerState(const RasterizerState &rstate)
{

}

void GLEAM_API::SetTessellatorState(const TessellatorState& tstate)
{

}

void GLEAM_API::SetViewportState(const ViewportState& vstate)
{
}

void GLEAM_API::SetBlendState(const BlendState& bstate)
{

}

void GLEAM_API::SetDepthState(const DepthState& dstate)
{

}

void GLEAM_API::SetStencilState(const StencilState& sstate)
{

}

void GLEAM_API::SetPixelProcessState(const PixelProcessState& pstate)
{

}

void GLEAM_API::SetShaderUniformState(const GLEAM_ShaderUniformState &ustate)
{
}

}
}
}
