#pragma once

#include <peripherals/typing/geometry/bounding_box.h>
#include <peripherals/typing/geometry/point.h>
#include <peripherals/typing/geometry/range.h>
#include <peripherals/typing/geometry/rect.h>
#include <peripherals/typing/geometry/size.h>

#include <peripherals/libc/types.h>

namespace Coffee {

using _cbasic_size = typing::geometry::base_size;

using _cbasic_point = typing::geometry::base_point;

template<typename T>
using _cbasic_point_2d = typing::geometry::point_2d<T>;

template<typename T>
using _cbasic_point_3d = typing::geometry::point_3d<T>;

template<typename T>
using _cbasic_size_2d = typing::geometry::size_2d<T>;

template<typename T>
using _cbasic_size_3d = typing::geometry::size_3d<T>;

template<typename T>
using _cbasic_range = typing::geometry::range<T>;

template<typename T>
using _cbasic_rect = typing::geometry::rect<T>;

template<typename T>
using _cbasic_boundingbox = typing::geometry::boundingbox<T>;


/*!
 * \brief Typical size, uses integer, should be used for window size
 */
using CSize = _cbasic_size_2d<libc_types::i32>;

using Size = CSize;

using CSize3 = _cbasic_size_3d<libc_types::i32>;

using Size3 = CSize3;
/*!
 * \brief Heavy-duty size structure
 */
using CSize64 = _cbasic_size_2d<libc_types::u64>;
/*!
 * \brief Size for inaccurate measurements
 */
using CSizeF = _cbasic_size_2d<libc_types::scalar>;
/*!
 * \brief Size of accurate measurements
 */
using CSizeD = _cbasic_size_2d<libc_types::bigscalar>;

/*!
 * \brief Used for window rectangles
 */
using CRect = _cbasic_rect<libc_types::i32>;

using CRect64 = _cbasic_rect<libc_types::i64>;
/*!
 * \brief Used for orthographic view rectangle
 */
using CRectF = _cbasic_rect<libc_types::scalar>;
/*!
 * \brief Used for high-accuracy orthographic view rectangle
 */
using CRectD = _cbasic_rect<libc_types::bigscalar>;

/*!
 * \brief 32-bit integer point
 */
using CPoint = _cbasic_point_2d<libc_types::i32>;

using CPoint3 = _cbasic_point_3d<libc_types::i32>;
/*!
 * \brief Single-precision floating-point point
 */
using CPointF = _cbasic_point_2d<libc_types::scalar>;
/*!
 * \brief Double-precision floating-point point
 */
using CPointD = _cbasic_point_2d<libc_types::bigscalar>;

/*!
 * \brief Standard clipping type
 */
using ZField = _cbasic_range<libc_types::scalar>;

using ZField64 = _cbasic_range<libc_types::bigscalar>;

using BoundBox = _cbasic_boundingbox<libc_types::scalar>;

using PtF = CPointF;
using PtD = CPointD;
using PtI = CPoint;

using Point  = CPoint;
using Point3 = CPoint3;

}
