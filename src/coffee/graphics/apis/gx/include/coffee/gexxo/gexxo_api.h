#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/typing/geometry/size.h>

/* Low-level GameCube video/GX bring-up shared by the application bootstrap
 * (coffee.cpp) and the comp_app windowing bridge (cog_comp). The newer RHI
 * surface lives in <coffee/graphics/apis/CGexxo>; these free functions remain
 * the minimal video contract those consumers depend on. */

namespace gexxo {

/* Bring up VI: allocate framebuffers, configure the preferred video mode and
 * initialize the console. Safe to call multiple times (no-op if already up). */
extern void initialize();

/* Initialize the GX FIFO + pipeline (viewport, disp-copy, pixel format).
 * Calls initialize() first if needed. */
extern void gxInitialize();

/* Block forever waiting for vsync (used as a halt on fatal bootstrap paths). */
extern void infiniteLoop();

/* Copy EFB -> XFB (applying the copy-clear color) and flip. */
extern void swapBuffers();

/* Current external framebuffer dimensions. */
extern typing::geometry::size_2d<libc_types::i32> framebufferSize();

} // namespace gexxo
