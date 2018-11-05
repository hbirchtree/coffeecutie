#pragma once

#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/types/edef/colorenum.h>
#include <coffee/core/types/edef/dbgenum.h>
#include <coffee/core/types/edef/pixenum.h>
#include <peripherals/stl/types.h>

namespace Coffee {

/*!
 * \brief Used for minor messages and similar, implemented on window
 * system-layer
 */
struct DialogInterface
{
    struct WinHnd;
    struct MsgWinHnd;

    static WinHnd*    CreateDWindow(cstring title, WinHnd* parent = nullptr);
    static MsgWinHnd* CreateMessageDialog(
        cstring title, WinHnd* parent = nullptr);

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

    struct Title
    {
        FORCEDINLINE Title(
            CString   t,
            Color     c = Color::White,
            Alignment a = AlignUndefined,
            int       s = 12) :
            m_title(t),
            m_align(a), m_col(c), m_size(s)
        {
        }
        FORCEDINLINE Title(Title const& t) :
            m_title(t.m_title), m_align(t.m_align), m_col(t.m_col),
            m_size(t.m_size)
        {
        }

        const CString   m_title;
        const Alignment m_align;
        const Color     m_col;
        const int       m_size;
    };

    static SplashHandle* CreateSplash();

    static void ShowSplash(SplashHandle*);
    static void HideSplash(SplashHandle*);

    static void SetSize(SplashHandle*, CSize const&);

    static void SetTitle(SplashHandle*, Title const&);
    static void SetSubText(SplashHandle*, Title const&);

    static void SetProgress(SplashHandle*, scalar);
    static void Repaint(SplashHandle*);

    static bool SetBitmap(
        SplashHandle*, PixFmt, CSize const&, const byte_t*);

    static void DestroySplash(SplashHandle*);
};

} // namespace Coffee
