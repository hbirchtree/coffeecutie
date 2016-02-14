#ifndef COFFEE_CORE_DIALOGS_FUNCTIONAL_H
#define COFFEE_CORE_DIALOGS_FUNCTIONAL_H

#include "../coffee_macros.h"
#include "../types/basetypes.h"

namespace Coffee{

/*!
 * \brief Used for minor messages and similar, implemented on window system-layer
 */
struct DialogInterface
{
    static void ErrorMessage(cstring title,cstring message){C_UNUSED(title);C_UNUSED(message);}
    static void WarningMessage(cstring title,cstring message){C_UNUSED(title);C_UNUSED(message);}
    static void InformationMessage(cstring title,cstring message){C_UNUSED(title);C_UNUSED(message);}

    static void InfiniteLoadingMessage(cstring title,cstring message){C_UNUSED(title);C_UNUSED(message);}

    static void InputDialog(cstring title,cstring message,cstring_w output,szptr max_size)
    {C_UNUSED(title);C_UNUSED(message);C_UNUSED(output);C_UNUSED(max_size);}
};

}

#endif