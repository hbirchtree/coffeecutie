/*
 * Bionic pre-API-21 symbols that libc++ needs and KitKat does not export.
 *
 * This is deliberately tiny. Most of the pre-21 gap is already covered:
 *
 *   - <android/legacy_stdlib_inlines.h> and friends, which the NDK sysroot
 *     pulls in automatically below API 21, define strtof, strtold, strtod_l
 *     and strtof_l as static inlines.
 *   - libandroid_support.a supplies strtold_l, strtoll_l, strtoull_l and the
 *     wide-character conversion functions.
 *   - kitkat_compat.h declares the *_l ctype/wctype/string/time functions
 *     that bionic hides behind "#if __ANDROID_API__ >= 21".
 *   - __cxa_thread_atexit_impl (API 23) is referenced weakly by libc++abi,
 *     which falls back to its own thread-local destructor list when it
 *     resolves to null. Nothing to do.
 *
 * What is left is one strong reference that would otherwise fail to link.
 * Compiled with -fvisibility=hidden so it never interposes on the real bionic
 * implementation if the resulting .so runs on a newer platform.
 */

/*
 * API 21. libc++abi's __abort_message calls this to attach the failure text to
 * the tombstone, then separately routes the same text to logcat via syslog(),
 * which KitKat does have. There is no pre-21 equivalent for the tombstone
 * half, so this is a genuine no-op rather than an emulation: the message is
 * not lost, it just does not appear in the crash dump header.
 */
void android_set_abort_message(const char* __msg) { (void)__msg; }
