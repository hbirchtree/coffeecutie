#pragma once

#include <peripherals/semantic/chunk.h>
#include <peripherals/semantic/chunk_ops.h>
#include <peripherals/semantic/serial_array.h>

namespace Coffee {

using namespace semantic::chunk_ops;

using semantic::Bytes;
using semantic::BytesConst;
using semantic::SerialArray;
using semantic::Span;
using semantic::mem_chunk;

template<typename T>
using _cbasic_data_chunk = semantic::mem_chunk<T>;

} // namespace Coffee
