#ifndef COFFEE_CORE_DIALOGS_FUNCTIONAL_H
#define COFFEE_CORE_DIALOGS_FUNCTIONAL_H

#include "../coffee_macros.h"
#include "../types/types.h"

namespace Coffee{

/*!
 * \brief Used for minor messages and similar, implemented on window system-layer
 */
struct DialogInterface
{
    struct WinHnd;
    struct MsgWinHnd;

    static WinHnd* CreateWindow(cstring title, WinHnd* parent = nullptr);
    static MsgWinHnd* CreateMessageDialog(cstring title, WinHnd* parent = nullptr);

    static void SetDialogSeverity(MsgWinHnd*, Severity);

    static void ShowDialog(WinHnd*);
    static void ShowDialog(MsgWinHnd*);

    static void HideDialog(WinHnd*);
    static void HideDialog(MsgWinHnd*);

    static void DestroyWindow(WinHnd*);
    static void DestroyMessageDialog(MsgWinHnd*);
};

struct SplashInterface
{
    struct SplashHandle;

    static SplashHandle* CreateSplash();

    static void ShowSplash(SplashHandle*);
    static void HideSplash(SplashHandle*);

    static void SetSize(SplashHandle*,CSize const&);

    static void SetTitle(SplashHandle*,CString);
    static void SetSubText(SplashHandle*,CString);

    static bool SetBitmap(SplashHandle*,PixelFormat,CSize const&,const byte_t*);

    static void DestroySplash(SplashHandle*);
};

}

#endif