#pragma once

#include "../types/cdisplay.h"
#include "../../types/edef/dbgenum.h"
#include <peripherals/stl/types.h>

namespace Coffee{
namespace Display{

class WindowApplication
{
public:
    /*!
     * \brief Get monitor information, physical size, bits etc.
     * \return Struct containing monitor information
     */
    virtual CDMonitor monitor() = 0;

    /*!
     * \brief Show window
     * \return Whether it was successful or not
     */
    virtual bool showWindow() = 0;
    /*!
     * \brief Hide window
     * \return Whether it was successful or not
     */
    virtual bool hideWindow() = 0;
    /*!
     * \brief Close window
     * \return Whether it was successful or not
     */
    virtual bool closeWindow() = 0;

    /*!
     * \brief General function used when an error occurs
     *  and there is a need to notify the user.
     * \param s Severity of message, may change its looks
     * \param title Title of the message box
     * \param msg Messaing in the message box
     */
    virtual void popErrorMessage(Severity s, cstring title, cstring msg) = 0;
protected:
    virtual bool windowPreInit(const CDProperties&,CString*) = 0;
    virtual bool windowInit(const CDProperties&,CString*) = 0;
    virtual bool windowPostInit(const CDProperties&,CString*) = 0;

    virtual void windowTerminate() = 0;
};

}
}
