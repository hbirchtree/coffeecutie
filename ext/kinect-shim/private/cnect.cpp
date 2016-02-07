#include <coffee_ext/kinect_shim/cnect.h>

#include <coffee/core/plat/memory/cmemory.h>
#include <coffee/core/base/cdebug.h>
#include <coffee/core/base/cthreading.h>

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/logger.h>
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>

namespace CoffeeExt{
namespace Freenect{

struct FreenectImplementation::FreenectContext
{
    FreenectContext();
    ~FreenectContext();

    libfreenect2::Freenect2 manager;
    libfreenect2::Freenect2Device* device;

    libfreenect2::SyncMultiFrameListener* listener;

    libfreenect2::FrameMap frames;

    std::mutex frame_mutex;

    libfreenect2::Frame** kframes;

    std::atomic_bool active;
    std::atomic_bool new_frame;

    std::future<void> exit_fun;
};

FreenectImplementation::FreenectContext::FreenectContext() :
    device(NULL)
{
    libfreenect2::setGlobalLogger(
                libfreenect2::createConsoleLogger(libfreenect2::Logger::Debug));
    int devices = manager.enumerateDevices();
    if(devices <= 0)
        throw std::runtime_error("No devices detected! Cannot continue!");
    cDebug("Number of Kinect 2 devices detected: {0}",devices);
    std::string serial /*= manager.getDefaultDeviceSerialNumber()*/;
    cDebug("Serial of device: {0}",serial);
    device = manager.openDevice(serial);
    listener = new libfreenect2::SyncMultiFrameListener(
                libfreenect2::Frame::Color|
                libfreenect2::Frame::Depth);

    device->setColorFrameListener(listener);
    device->setIrAndDepthFrameListener(listener);

    device->start();
}

FreenectImplementation::FreenectContext::~FreenectContext()
{
    delete listener;
    device->stop();
    device->close();
}

FreenectImplementation::FreenectContext *FreenectImplementation::Alloc()
{
    FreenectImplementation::FreenectContext *c =
            new FreenectImplementation::FreenectContext;
    Coffee::CMemClear(c,sizeof(FreenectImplementation::FreenectContext));
    return c;
}

void FreenectImplementation::LaunchAsync(FreenectImplementation::FreenectContext *c)
{
    c->kframes = (libfreenect2::Frame**)calloc(2,sizeof(libfreenect2::Frame*));
    c->active.store(true);

    c->exit_fun = std::async(std::launch::async,[=](){
        cDebug("Starting Kinect loop!");
        while(c->active.load())
        {
            c->listener->waitForNewFrame(c->frames);
            c->frame_mutex.lock();
            c->kframes[0] = (c->frames)[libfreenect2::Frame::Depth];
            c->kframes[1] = (c->frames)[libfreenect2::Frame::Color];
            c->new_frame.store(true);
            cDebug("Created new frame");
            c->frame_mutex.unlock();
        }
    });
}

void FreenectImplementation::ProcessFrame(FreenectImplementation::FreenectContext *c,
                                          FreenectImplementation::FreenectFrameProcessor p,
                                          size_t n)
{
    if(!c->active.load())
        return;
    for(size_t i=0;i<n;i++)
        if(!c->kframes[i])
            return;

    if(c->new_frame.load()){
        c->frame_mutex.lock();

        CNectRGB rgb(c->kframes[1]->width,c->kframes[1]->height);
        CNectDepth dep(c->kframes[0]->width,c->kframes[0]->height);

        p(rgb,dep);

        c->new_frame.store(false);
        c->frame_mutex.unlock();

        //We launch an async task to delete the frames, using a mutex to ensure we don't stomp on other data
        std::async(std::launch::async,[=](){
            c->frame_mutex.lock();
            c->listener->release(c->frames);
            c->frame_mutex.unlock();
        });
    }
}

void FreenectImplementation::ShutdownAsync(FreenectContext *c)
{
    c->active.store(false);
    c->exit_fun.get();
}

void FreenectImplementation::Free(FreenectImplementation::FreenectContext *c)
{
    delete c;
}

}
}