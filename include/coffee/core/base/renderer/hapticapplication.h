#pragma once

#include "../input/cinput.h"

namespace Coffee{

using namespace CInput;

namespace CDisplay{
class HapticApplication
{
public:
    /*!
     * \brief Function for handling haptic events.
     *  A user will call this with a constructed event e
     *  and have it play in a fire-and-forget fashion.
     * \param e
     * \param data
     */
    virtual void hapticInsert(const CIHapticEvent& e, c_cptr data) = 0;
};

}
}