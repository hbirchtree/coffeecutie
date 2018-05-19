#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_XF2
{
    /* XFB */
    STATICINLINE void XFBind(CGhnd h){glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,h);}

    STATICINLINE void XFPause(){glPauseTransformFeedback();}
    STATICINLINE void XFResume(){glResumeTransformFeedback();}
};
}
}
