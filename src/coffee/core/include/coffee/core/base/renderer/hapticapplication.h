#pragma once

#include <coffee/core/types/input/event_types.h>

namespace Coffee {
namespace Display {
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
    virtual void hapticInsert(const Input::CIHapticEvent& e, c_cptr data) = 0;
};

} // namespace Display
} // namespace Coffee
