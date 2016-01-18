#ifndef COFFEE_EXT_FREENECT_CNECT_H
#define COFFEE_EXT_FREENECT_CNECT_H

#include <coffee/core/CTypes>

namespace CoffeeExt{
namespace Freenect{

using namespace Coffee;

struct FreenectImplementation
{
    using CNectRGB = CBitmap;
    using CNectDepth = _cbasic_bitmap_base<scalar,uint32>;
    using FreenectFrameProcessor = void(*)(CNectRGB const&,CNectDepth const&);

    struct FreenectContext;

    /*!
     * \brief Allocates a Freenect device context, manages all Kinect 2 device state, throws a chair if no devices are found.
     * \return Valid pointer if succeeded, nullptr if failure
     */
    static FreenectContext* Alloc();
    static void Free(FreenectContext* c);

    /*!
     * \brief Launch the context in an asynchronous thread for awaiting frames from the device
     * \param c
     */
    static void LaunchAsync(FreenectContext* c);
    /*!
     * \brief Ask the context to process a frame
     * \param c
     * \param p
     * \param n
     */
    static void ProcessFrame(FreenectContext* c, FreenectFrameProcessor p, size_t n);
    /*!
     * \brief Closes the asynchronous thread, called from main thread
     * \param c
     */
    static void ShutdownAsync(FreenectContext* c);
};

}
}

#endif