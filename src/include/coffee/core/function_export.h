#pragma once

#include "plat/plat_compiler_identify.h"

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)

#if defined(SWIG)
#define COFFAPI extern
#elif !defined(COFFEE_NO_EXPORT)
#define COFFAPI extern "C" __attribute__((visibility("default")))
#define COFFAPI_PP extern __attribute__((visibility("default")))
#else
#define COFFAPI extern "C"
#define COFFAPI_PP extern
#endif

#elif defined(COFFEE_MSVCXX)

#if !defined(COFFEE_NO_EXPORT)
#define COFFAPI extern "C" __declspec(dllexport)
#else
#define COFFAPI extern "C"
#endif

#else

#define COFFAPI extern "C"

#endif

#ifndef COFFEE_APP_FUNC
/*!
  \brief If a function is marked with this, it is not
 available within shared libraries.
  */
#define COFFEE_APP_FUNC extern
/*!
  \brief If a class/struct is marked with this, it is not
 available within shared libraries.
  */
#define COFFEE_APP_CLASS
#endif
