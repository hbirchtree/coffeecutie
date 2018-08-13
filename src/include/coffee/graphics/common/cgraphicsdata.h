#ifndef CGRAPHICSDATA_H
#define CGRAPHICSDATA_H

#include <coffee/core/types/graphics_types.h>
#include <coffee/core/types/cmatrix_functions.h>

namespace Coffee{
/*!
 * \brief Graphics rendering data, mostly for creating matrices
 */
namespace CGraphicsData{

typedef _cbasic_graphics_camera<scalar> CGCamera;

typedef _cbasic_graphics_transform<scalar> CTransform;

typedef _cbasic_rect<scalar> CViewport;
typedef _cbasic_rect<i32> CScissorViewport;
typedef _cbasic_range<bigscalar> CDepthViewport;

} //CGraphicsData

using namespace CGraphicsData;
using Transform = CTransform;

} //Coffee

#endif // CGRAPHICSDATA_H
