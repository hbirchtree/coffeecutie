#include <coffee_ext/kinect_shim/cnect.h>

#include <coffee/core/base/cthreading.h>

#include <coffee/core/plat/memory/cmemory.h>
#include <coffee/core/base/cdebug.h>
#include <coffee/core/base/cthreading.h>

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/logger.h>
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>

namespace CoffeeExt{
namespace Freenect{

struct FramePacket
{
    libfreenect2::Frame* color;
    libfreenect2::Frame* depth;

    Mutex access;
};

struct FreenectImplementation::FreenectContext
{
    FreenectContext(int index);
    ~FreenectContext();

    libfreenect2::Freenect2 manager;
    libfreenect2::Freenect2Device* device;

    libfreenect2::SyncMultiFrameListener* listener;
    libfreenect2::Registration* regist;

    libfreenect2::FrameMap frames;

    std::mutex frame_mutex;

    FramePacket* kframes;

    FramePacket* frame_ready;

    std::atomic_bool active;
    std::atomic_bool new_frame;

    std::future<void> exit_fun;
};

FreenectImplementation::FreenectContext::FreenectContext(int index) :
    device(NULL)
{
    libfreenect2::setGlobalLogger(
                libfreenect2::createConsoleLogger(libfreenect2::Logger::Debug));
    int devices = manager.enumerateDevices();
    cLog(__FILE__,__LINE__,CFStrings::CNect_Library_Name,CFStrings::CNect_NumDevices,devices);
    if(devices <= 0)
        throw std::runtime_error(CFStrings::CNect_NoDevs);
    if(index >= devices)
        throw std::runtime_error(CFStrings::CNect_IndexFlt);
    std::string serial = manager.getDeviceSerialNumber(index);
    cLog(__FILE__,__LINE__,CFStrings::CNect_Library_Name,CFStrings::CNect_Loaded,serial);
    device = manager.openDevice(serial);
    listener = new libfreenect2::SyncMultiFrameListener(
                libfreenect2::Frame::Color|
                libfreenect2::Frame::Depth);

    device->setColorFrameListener(listener);
    device->setIrAndDepthFrameListener(listener);

    regist = new libfreenect2::Registration(device->getIrCameraParams(),device->getColorCameraParams());

    device->start();
}

FreenectImplementation::FreenectContext::~FreenectContext()
{
    delete regist;
    delete listener;
    if(device)
    {
        device->stop();
        device->close();
    }
}

FreenectImplementation::FreenectContext *FreenectImplementation::Alloc(int index)
{
    FreenectImplementation::FreenectContext *c =
            new FreenectImplementation::FreenectContext(index);
    return c;
}

void FreenectImplementation::LaunchAsync(FreenectImplementation::FreenectContext *c)
{
    c->kframes = new FramePacket[3];
    c->active.store(true);

    c->exit_fun = std::async(std::launch::async,[=](){
        cLog(__FILE__,__LINE__,CFStrings::CNect_Library_Name,
             CFStrings::CNect_AsyncStart);

        size_t curr_packet = 0;

        while(c->active.load())
        {
            c->listener->waitForNewFrame(c->frames);

            {

            }

            {
                Lock l(c->kframes[curr_packet].access);
                C_UNUSED(l);

                c->frame_ready = c->kframes[curr_packet];
            }
        }
    });
}

bool FreenectImplementation::RunningAsync(FreenectImplementation::FreenectContext *c)
{
    return c->active.load();
}

void FreenectImplementation::ProcessFrame(FreenectImplementation::FreenectContext *c,
                                          FreenectImplementation::FreenectFrameProcessor p,
                                          size_t n)
{
    if(!c->active.load())
        return;

    if(!c->frame_ready)
        return;

    FramePacket* packet = c->frame_ready;

    Lock l(c->kframes[curr_packet].access);
    C_UNUSED(l);

    CNectRGB rgb(packet->color->width,packet->color->height);
    CNectDepth dep(packet->depth->width,packet->depth->height);



    p(rgb,dep);

    c->new_frame.store(false);

    //We launch an async task to delete the frames, using a mutex to ensure we don't stomp on other data
    std::async(std::launch::async,[=](){
        c->frame_mutex.lock();
        c->listener->release(c->frames);
        c->frame_mutex.unlock();
    });
}

void FreenectImplementation::ShutdownAsync(FreenectContext *c)
{
    c->active.store(false);
    c->exit_fun.get();
    delete[] c->kframes;
    cLog(__FILE__,__LINE__,CFStrings::CNect_Library_Name,
         CFStrings::CNect_AsyncStop);
}

void FreenectImplementation::Free(FreenectImplementation::FreenectContext *c)
{
    delete c;
}

}
}
