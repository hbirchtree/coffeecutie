/*
 * Bionic pre-API-21 locale compatibility shim.
 *
 * Upstream libc++ (since the locale backend rework) routes Android through
 * __locale_dir/support/linux.h, which assumes the glibc-style *_l locale
 * functions exist. Bionic only grew those in API 21, and hides them behind
 * "#if __ANDROID_API__ >= 21" in <ctype.h>/<wctype.h>/<string.h>/<time.h>,
 * so at API 19 they are not merely unavailable, they are undeclared.
 *
 * Bionic has exactly one locale ("C", plus a UTF-8 flavour of it), and its
 * own API 21+ implementations of these functions ignore the locale_t argument
 * entirely. Forwarding to the unsuffixed function is therefore not an
 * approximation, it is what the platform does.
 *
 * Force-included (-include) when compiling C++ against this toolchain, both
 * while building libc++ and when using it. Functions that bionic *declares*
 * but does not ship at this API level live in kitkat_compat.c instead, since
 * a static inline definition would clash with the existing declaration.
 */

#ifndef KITKAT_COMPAT_H
#define KITKAT_COMPAT_H

#include <android/api-level.h>

#if defined(__ANDROID__) && __ANDROID_API__ < 21

#include <ctype.h>
#include <locale.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _KK_UNUSED_LOC(l) ((void)(l))

/* <ctype.h> */
static inline int isdigit_l(int __c, locale_t __l) { _KK_UNUSED_LOC(__l); return isdigit(__c); }
static inline int isxdigit_l(int __c, locale_t __l) { _KK_UNUSED_LOC(__l); return isxdigit(__c); }
static inline int toupper_l(int __c, locale_t __l) { _KK_UNUSED_LOC(__l); return toupper(__c); }
static inline int tolower_l(int __c, locale_t __l) { _KK_UNUSED_LOC(__l); return tolower(__c); }

/* <string.h> */
static inline int strcoll_l(const char* __a, const char* __b, locale_t __l) {
    _KK_UNUSED_LOC(__l);
    return strcoll(__a, __b);
}
static inline size_t strxfrm_l(char* __dst, const char* __src, size_t __n, locale_t __l) {
    _KK_UNUSED_LOC(__l);
    return strxfrm(__dst, __src, __n);
}

/* <time.h> */
static inline size_t strftime_l(char* __s, size_t __n, const char* __fmt, const struct tm* __tm, locale_t __l) {
    _KK_UNUSED_LOC(__l);
    return strftime(__s, __n, __fmt, __tm);
}

/* <wchar.h> */
static inline int wcscoll_l(const wchar_t* __a, const wchar_t* __b, locale_t __l) {
    _KK_UNUSED_LOC(__l);
    return wcscoll(__a, __b);
}
static inline size_t wcsxfrm_l(wchar_t* __dst, const wchar_t* __src, size_t __n, locale_t __l) {
    _KK_UNUSED_LOC(__l);
    return wcsxfrm(__dst, __src, __n);
}

/* <wctype.h>. iswblank itself is also API 21, so inline the C-locale rule. */
static inline int iswblank_l(wint_t __c, locale_t __l) {
    _KK_UNUSED_LOC(__l);
    return __c == L' ' || __c == L'\t';
}
static inline int iswctype_l(wint_t __c, wctype_t __t, locale_t __l) { _KK_UNUSED_LOC(__l); return iswctype(__c, __t); }
static inline int iswspace_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswspace(__c); }
static inline int iswprint_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswprint(__c); }
static inline int iswcntrl_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswcntrl(__c); }
static inline int iswupper_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswupper(__c); }
static inline int iswlower_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswlower(__c); }
static inline int iswalpha_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswalpha(__c); }
static inline int iswdigit_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswdigit(__c); }
static inline int iswpunct_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswpunct(__c); }
static inline int iswxdigit_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswxdigit(__c); }
static inline int iswalnum_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswalnum(__c); }
static inline int iswgraph_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return iswgraph(__c); }
static inline wint_t towupper_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return towupper(__c); }
static inline wint_t towlower_l(wint_t __c, locale_t __l) { _KK_UNUSED_LOC(__l); return towlower(__c); }
static inline wctype_t wctype_l(const char* __name, locale_t __l) { _KK_UNUSED_LOC(__l); return wctype(__name); }

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __ANDROID__ && __ANDROID_API__ < 21 */
#endif /* KITKAT_COMPAT_H */
