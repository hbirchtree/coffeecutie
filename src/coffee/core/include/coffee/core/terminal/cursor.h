#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/type_safety.h>
#include <peripherals/stl/types.h>

#include <coffee/core/CDebug>

#if defined(COFFEE_LINUX)
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace Coffee {

/*!
 * \brief Terminal cursor for doing fancy progress bars and stuff
 */
struct TerminalCursor
{
    const Array<cstring, 4> progress_bar = {{"◷", "◶", "◵", "◴"}};

    const cstring progress_complete = "◉";

    TerminalCursor() : m_lastLength(0), m_progressIdx(0), m_overwriteMode(false)
    {
#if defined(COFFEE_LINUX)
        m_interactive = isatty(fileno(stderr));
#else
        m_interactive = false;
#endif
    }

    /* These functions are found in cdebug.cpp */
    void print_basic(CString const& fmt, CString const& output);
    void print_nonl(CString const& fmt, CString const& output);
    /*  */

    template<typename... Args>
    void overwrite(cstring fmt, Args... args)
    {
        if(!m_interactive)
        {
            print(fmt, args...);
            return;
        }

        CString output = cStringFormat(fmt, args...);

        if(m_lastLength > output.size())
            output.insert(output.end(), m_lastLength - output.size(), ' ');

        szptr terminalWidth = 0;
#if defined(COFFEE_LINUX)
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        terminalWidth = w.ws_col;
#endif

        if(output.size() > terminalWidth)
        {
            output.resize(terminalWidth - 1);
            output.append(">");
        }

        cstring outFmt = nullptr;

        if(!m_overwriteMode)
            outFmt = "{0}\r";
        else
            outFmt = "\r{0}\r";

        print_nonl(outFmt, output);
        CurrentThread::sleep_for(Chrono::milliseconds(100));
        m_lastLength    = output.size();
        m_overwriteMode = true;
    }

    void setProgress(Function<CString()>&& progressMeter)
    {
        m_progressMeter = progressMeter;
    }

    template<typename... Args>
    void progress(cstring fmt, Args... args)
    {
        if(!m_progressMeter)
            this->overwrite(fmt, args...);

        CString newFmt =
            progress_bar[m_progressIdx] + ("  " + m_progressMeter()) + fmt;
        this->overwrite(newFmt.c_str(), args...);

        m_progressIdx = (++m_progressIdx % progress_bar.size());
    }

    template<typename... Args>
    void complete(cstring fmt, Args... args)
    {
        CString newFmt = progress_complete + CString("  ") + fmt;

        this->overwrite(newFmt.c_str(), args...);
        m_progressIdx = 0;
    }

    template<typename... Args>
    void print(cstring fmt, Args... args)
    {
        CString output = cStringFormat(fmt, args...);

        cstring outFmt = nullptr;

        if(m_overwriteMode)
            outFmt = "\n{0}";
        else
            outFmt = "{0}";

        print_basic(outFmt, output);
        m_overwriteMode = false;
        m_lastLength    = 0;
    }

  private:
    Function<CString()> m_progressMeter;
    szptr               m_lastLength;
    szptr               m_progressIdx;
    bool                m_interactive;
    bool                m_overwriteMode;
};

} // namespace Coffee
