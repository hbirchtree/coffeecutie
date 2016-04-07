#pragma once

#include <coffee/core/CDisplay>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/types/tdef/integertypes.h>
#include "csdl2_pubtypes.h"

#include <coffee/core/base/renderer/baseapplication.h>

namespace Coffee{
namespace Display{

class SDL2ContextUser :
        public BaseApplication
{
public:
    using Context = CSDL2Types::CSDL2Context;

    Context* getSDL2Context()
    {
        return m_ctxt;
    }
    Context const* getSDL2Context() const
    {
        return m_ctxt;
    }

protected:
    SDL2ContextUser()
    {
    }
    SDL2ContextUser(Context* context):
        m_ctxt(context)
    {
    }
    SDL2ContextUser(SDL2ContextUser* parent):
        m_ctxt(parent->m_ctxt)
    {
    }

    void setSDL2Context(Context* context)
    {
        m_ctxt = context;
    }

    const CDProperties &initialProperties()
    {
        return m_properties;
    }

    uint32 m_window_flags;
    CDProperties m_properties;
    CString m_contextString;

private:
    Context* m_ctxt;
};

}
}