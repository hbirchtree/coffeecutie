#pragma once

#include "../types/cdisplay.h"

namespace Coffee{
namespace Display{

class InitApplication
{
public:
    /*!
     * \brief Initialize context manager with necessary details,
     *  should not be overridden by the user.
     * \param props Hints for creating the context
     */
    virtual bool init(const CDProperties& props,CString*) = 0;
    /*!
     * \brief Function for running cleanup, should avoid memory leaks
     */
    virtual void cleanup() = 0;
};

}
}
