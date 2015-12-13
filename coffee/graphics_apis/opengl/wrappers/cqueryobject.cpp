#include "cqueryobject.h"

#include "glfunctions.h"

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_alloc(size_t count, CQueryObject *obj, const CQueryType &type)
{
    CGuint *handles = new CGuint[count];
    glGenQueries(count,handles);
    for(size_t i=0;i<count;i++)
        obj[i].handle = handles[i];
    delete[] handles;
}

void coffee_graphics_free(size_t count, CQueryObject *obj)
{
    CGuint *handles = new CGuint[count];
    for(size_t i=0;i<count;i++)
    {
        handles[i] = obj[i].handle;
        obj[i].handle = 0;
    }
    glDeleteQueries(count,handles);
    delete[] handles;
}

void coffee_graphics_query_begin(CQueryObject &obj)
{
    glBeginQueryIndexed(gl_get(obj.type),obj.index,obj.handle);
}

void coffee_graphics_query_end(CQueryObject &obj)
{
    glEndQueryIndexed(gl_get(obj.type),obj.index);
}

void coffee_graphics_query_counter(CQueryObject &obj)
{
    glQueryCounter(obj.handle,gl_get(obj.type));
}

void coffee_graphics_query_get_value(const CQueryObject &obj, const CQueryResultType &type, CGuint64 *value)
{
    glGetQueryObjectui64v(obj.handle,gl_get(type),value);
}

}
}
