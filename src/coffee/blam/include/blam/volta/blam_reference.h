#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <type_traits>

#include "blam_atlas_type.h"
#include "../common/endian.h"
#include "../common/reference.h"
#include "blam_magic_data.h"
#include "blam_versions.h"

namespace blam {

using libc_types::u32;
using libc_types::u64;

static_assert(
    sizeof(reference<int, grbx_t>) == 12,
    "reference_t<..., grbx_t> needs to be 12 bytes");

static_assert(
    sizeof(reference<int, xbox_t>) == 8,
    "reference_t<..., xbox_t> needs to be 8 bytes");

static_assert(resolves_with<reference<int>, map_ptr>);
static_assert(
    !resolves_with<reference<int>, bsp_ptr>,
    "a block only resolves through the magic of its own heap");

template<typename T, typename V = grbx_t>
using bsp_reference = reference<T, V, atlas_type_t::map_file, heap_t::bsp>;

template<typename T, typename V = grbx_t>
using vertex_reference =
    reference<T, V, atlas_type_t::map_file, heap_t::vertex>;

} // namespace blam
