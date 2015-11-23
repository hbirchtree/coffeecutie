#include "crenderfence.h"

#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CGSync
{
    GLsync handle;
};

CGSync *coffee_graphics_fence_create()
{
    CGSync* snc = new CGSync;

    snc->handle = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,GL_NONE_BIT);

    return snc;
}

void coffee_graphics_free(CGSync* snc)
{
    delete snc;
}

bool coffee_graphics_fence_wait(CGSync* snc, uint64 timeout)
{
    GLenum status = glClientWaitSync(snc->handle,GL_SYNC_FLUSH_COMMANDS_BIT,timeout);
    return (status!=GL_WAIT_FAILED)&&(status==GL_CONDITION_SATISFIED);
}

}
}
