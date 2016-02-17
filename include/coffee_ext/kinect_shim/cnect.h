#ifndef COFFEE_EXT_FREENECT_CNECT_H
#define COFFEE_EXT_FREENECT_CNECT_H

#include <coffee/core/CTypes>

namespace CoffeeExt{
namespace Freenect{

using namespace Coffee;

struct FreenectImplementation
{
    struct ColorVec3
    {
	CVec3 p;
	CRGBA color;
    };

    using NectCloud = std::vector<ColorVec3>;
    using NectRGB = CBitmap;
    using NectDepth = _cbasic_bitmap_base<scalar,uint32>;

    using FreenectFrameProcessor = void(*)(NectRGB const&,NectDepth const&,NectCloud const&);

    struct FreenectContext;

    /*!
     * \brief Allocates a Freenect device context, manages all Kinect 2 device state, throws a chair if no devices are found.
     * \return Valid pointer if succeeded, nullptr if failure
     */
    static FreenectContext* Alloc(int index);
    static void Free(FreenectContext* c);

    /*!
     * \brief Launch the context in an asynchronous thread for awaiting frames from the device
     * \param c
     */
    static void LaunchAsync(FreenectContext* c);

    /*!
     * \brief Allows user to check if the context is currently active in an async manner
     * \param c
     * \return True if the context is running async
     */
    static bool RunningAsync(FreenectContext* c);

    /*!
     * \brief Ask the context to process a frame
     * \param c
     * \param p
     * \param n
     */
    static bool ProcessFrame(FreenectContext* c, FreenectFrameProcessor p, size_t n);
    /*!
     * \brief Closes the asynchronous thread, called from main thread
     * \param c
     */
    static void ShutdownAsync(FreenectContext* c);

    class FContext
    {
    public:
        FContext(int index):
            m_val(Alloc(index))
        {
        }
        ~FContext()
        {
            if(RunningAsync(m_val))
                ShutdownAsync(m_val);
            Free(m_val);
        }
        FreenectContext* context()
        {
            return m_val;
        }
    private:
        FreenectContext* m_val;
    };
};

}
}

#endif
