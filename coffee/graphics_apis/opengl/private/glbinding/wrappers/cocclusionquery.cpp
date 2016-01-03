#include "cocclusionquery.h"

#include "glfunctions.h"

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_occquery_conditional_begin(const CQueryObject &obj, bool wait)
{
    glBeginConditionalRender(
                obj.handle,
                (wait) ? GL_QUERY_BY_REGION_WAIT : GL_QUERY_BY_REGION_NO_WAIT);
}

void coffee_graphics_occquery_conditional_end(const CQueryObject &obj)
{
    glEndConditionalRender();
}

}
}
