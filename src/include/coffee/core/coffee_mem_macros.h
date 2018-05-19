#pragma once

#include "plat/plat_compiler_identify.h"
#include "plat/plat_quirks_toggling.h"

#define C_DEFAULT_CONSTRUCTOR(ctype) ctype() = default
#define C_DELETE_COPY_CONSTRUCTOR(ctype) ctype(ctype const&) = delete
#define C_MOVE_CONSTRUCTOR(ctype) ctype(ctype&& x) = default; ctype& operator=(ctype&& a) = default

/* Wrappers around static_cast, dynamic_cast, reinterpret_cast to make casting simpler */
#include "type_safety.h"

/* Library export */
#include "function_export.h"

/* Packing compiler flags */
#include "struct_packing.h"

/* Inlining compiler flags */
#include "function_inlining.h"

/* Marking functions/variables */
#include "variable_attributes.h"
