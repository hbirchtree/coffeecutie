#pragma once

/* Build environment info */
#include <peripherals/identify/architecture.h>
#include <peripherals/identify/compiler.h>
#include <peripherals/identify/system.h>

/* Build configuration */
#include <peripherals/identify/quirks.h>

/* Compiler-specific attributes */
#include <peripherals/identify/compiler/function_export.h>
#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/identify/compiler/struct_packing.h>
#include <peripherals/identify/compiler/variable_attributes.h>

/* Debugger hooks */
#include <peripherals/identify/compiler/debug_break.h>

/* Wrappers around static_cast<> and etc. */
#include <peripherals/stl/type_safety.h>

/* Class shortcuts */
#include <peripherals/stl/constructors.h>
