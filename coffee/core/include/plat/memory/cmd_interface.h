#ifndef COFFEE_CORE_PLAT_COMMAND_INTERFACE_H
#define COFFEE_CORE_PLAT_COMMAND_INTERFACE_H

#include "../environment/environment_details.h"
#include <coffee/core/CTypes>

#if defined(COFFEE_USE_TERMINAL_CTL)
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace Coffee{

inline C_FORCE_INLINE bool interactive_cmd()
{
    //TODO: Find way of detecting interactive session
    return false;
}

inline C_FORCE_INLINE cstring ReadString(cstring_w target, int32 size, FILE* strm)
{
    return CGets(target,size,strm);
}

inline C_FORCE_INLINE void ClearScreen()
{
#if defined(COFFEE_USE_TERMINAL_CTL)
    fprintf(stderr,"\e[1;1H\e[2J");
#endif
}

/*!
 * \brief Enable alternate buffer for xterm
 */
inline C_FORCE_INLINE void AltScreen()
{
#if defined(COFFEE_USE_TERMINAL_CTL)
    fprintf(stderr,"\033[?1049h\033[H");
    TermScreen::UsingAlternateBuffer = !TermScreen::UsingAlternateBuffer;
#endif
}

/*!
 * \brief Disable alternate buffer for xterm
 */
inline C_FORCE_INLINE void ResetScreen()
{
#if defined(COFFEE_USE_TERMINAL_CTL)
    if(!TermScreen::UsingAlternateBuffer)
        return;
    fprintf(stderr,"\033[?1049l");
    TermScreen::UsingAlternateBuffer = !TermScreen::UsingAlternateBuffer;
#endif
}

inline C_FORCE_INLINE CSize TerminalSize()
{
#if defined(COFFEE_USE_IOCTL_TERM_SIZE)
    struct winsize w;
    ioctl(STDOUT_FILENO,TIOCGWINSZ,&w);
    return CSize(w.ws_col,w.ws_row);
#else
    return CSize();
#endif
}

inline C_FORCE_INLINE void Exit(int code)
{
    cBasicPrint("Exiting with code: {0}",code);
    exit(code);
}

}

#endif
