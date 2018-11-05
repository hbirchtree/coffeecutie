#pragma once

#include <peripherals/semantic/chunk.h>
#include <peripherals/semantic/chunk_ops.h>

namespace Coffee {

using namespace semantic::chunk_ops;

using semantic::mem_chunk;

using Bytes      = semantic::mem_chunk<byte_t>;
using BytesConst = semantic::mem_chunk<const byte_t>;

template<typename T>
using _cbasic_data_chunk = semantic::mem_chunk<T>;

template<typename T>
using Span = semantic::mem_chunk<T>;

} // namespace Coffee
