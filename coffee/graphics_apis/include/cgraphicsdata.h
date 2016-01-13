#ifndef CGRAPHICSDATA_H
#define CGRAPHICSDATA_H

#include <coffee/core/CTypes>
#include <base/cmath.h>
#include <types/cmatrix_functions.h>

namespace Coffee{
namespace CGraphicsData{

typedef _cbasic_graphics_camera<scalar> CGCamera;

typedef _cbasic_graphics_transform<scalar> CTransform;

typedef _cbasic_rect<scalar> CViewport;
typedef _cbasic_rect<int32> CScissorViewport;
typedef _cbasic_zfield<bigscalar> CDepthViewport;

} //CGraphicsData
} //Coffee

#endif // CGRAPHICSDATA_H
