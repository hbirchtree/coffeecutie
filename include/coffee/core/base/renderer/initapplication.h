#pragma once

#include "../types/cdisplay.h"

namespace Coffee{
namespace CDisplay{

class InitApplication
{
public:
    /*!
     * \brief Initialize context manager with necessary details,
     *  should not be overridden by the user.
     * \param props Hints for creating the context
     */
    virtual void init(const CDProperties& props) = 0;
    /*!
     * \brief Function for running cleanup, should avoid memory leaks
     */
    virtual void cleanup() = 0;
};

}
}