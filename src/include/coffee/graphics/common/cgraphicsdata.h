#pragma once

#include <coffee/core/types/cmatrix_functions.h>
#include <coffee/core/types/graphics_types.h>

namespace Coffee {
/*!
 * \brief Graphics rendering data, mostly for creating matrices
 */
namespace SceneGraph {

using CGCamera = _cbasic_graphics_camera<scalar>;

using CTransform = _cbasic_graphics_transform<scalar>;

} // namespace SceneGraph

using namespace SceneGraph;
using Transform = CTransform;

} // namespace Coffee
