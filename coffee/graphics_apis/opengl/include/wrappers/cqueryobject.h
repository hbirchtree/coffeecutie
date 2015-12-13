#ifndef COFFEE_GRAPHICS_APIS_OPENGL_WRAPPERS_QUERY_H
#define COFFEE_GRAPHICS_APIS_OPENGL_WRAPPERS_QUERY_H

#include "copengl_types.h"

/*!
 * All query functionality is written against OpenGL 3.3
 *
 */

namespace Coffee{
namespace CGraphicsWrappers{

struct CQueryObject : _cbasic_graphics_resource
{
    CGuint index;
    CQueryType type;
};

extern void coffee_graphics_alloc(size_t count, CQueryObject* obj, const CQueryType& type);
extern void coffee_graphics_free(size_t count, CQueryObject* obj);

inline void coffee_graphics_alloc(CQueryObject& obj, const CQueryType& type)
{
    coffee_graphics_alloc(1,&obj,type);
}
inline void coffee_graphics_free(CQueryObject& obj)
{
    coffee_graphics_free(1,&obj);
}

extern void coffee_graphics_query_begin(CQueryObject& obj);
extern void coffee_graphics_query_end(CQueryObject& obj);

extern void coffee_graphics_query_counter(CQueryObject& obj);

extern void coffee_graphics_query_get_value(const CQueryObject& obj,
                                            const CQueryResultType& type,
                                            CGuint64* value);

}
}

#endif
