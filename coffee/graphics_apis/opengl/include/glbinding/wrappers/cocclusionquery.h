#ifndef COFFEE_GRAPHICS_APIS_OPENGL_WRAPPERS_OCCLUSION_QUERY_H
#define COFFEE_GRAPHICS_APIS_OPENGL_WRAPPERS_OCCLUSION_QUERY_H

#include "cqueryobject.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief Start a conditional rendering based on a query object's amount of passed samples.
 * \param obj A query object which must be measuring the amount of samples passed. (CQueryType::AnySamplesPassed and CQueryType::SamplesPassed)
 * \param wait Whether or not it should wait for the samples to become available. If this fails, it will render.
 */
extern void coffee_graphics_occquery_conditional_begin(
        const CQueryObject& obj, bool wait = true);

extern void coffee_graphics_occquery_conditional_end(const CQueryObject& obj);

}
}

#endif
