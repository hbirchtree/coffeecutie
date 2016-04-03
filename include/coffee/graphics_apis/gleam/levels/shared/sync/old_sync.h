#pragma once

#include "../gl_shared_include.h"

#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace CGL{

template<typename CGsync,typename CGenum>
struct CGL_Old_Sync
{
    STATICINLINE CGsync FenceCreate(){return glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);}
    STATICINLINE void FenceServerAwait(CGsync s){glWaitSync((GLsync)s,0,0);}

    STATICINLINE bool FenceAwait(CGsync s,uint64 t,CGenum *r = nullptr)
    {
        CGenum status = glClientWaitSync((GLsync)s,0,t);
        if(r)
            *r = status;
        return status!=GL_TIMEOUT_EXPIRED;
    }

    STATICINLINE void FenceGetiv(CGsync s,CGenum p,int32* d){glGetSynciv((GLsync)s,p,1,nullptr,d);}
};

}
}
