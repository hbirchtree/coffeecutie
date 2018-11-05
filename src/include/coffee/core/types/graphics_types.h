#pragma once

#include <peripherals/typing/vectors/camera.h>
#include <peripherals/typing/vectors/transform.h>

namespace Coffee {
namespace SceneGraph {

template<typename T>
using _cbasic_camera = typing::vectors::scene::camera<T>;

template<typename T>
using _cbasic_camera_ortho = typing::vectors::scene::camera_ortho<T>;

template<typename T>
using _cbasic_graphics_transform = typing::vectors::scene::transform<T>;

template<typename T>
using _cbasic_graphics_camera = _cbasic_camera<T>;

template<typename T>
using camera = _cbasic_camera<T>;

template<typename T>
using camera_ortho = _cbasic_camera_ortho<T>;


} // namespace SceneGraph

using Transform = SceneGraph::_cbasic_graphics_transform<scalar>;
using CGCamera = SceneGraph::camera<scalar>;

} // namespace Coffee
