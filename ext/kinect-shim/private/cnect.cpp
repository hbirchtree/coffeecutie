#include <coffee_ext/kinect_shim/cnect.h>

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

class NectLogger : public libfreenect2::Logger
{
public:
    NectLogger()
    {
    }

    void log(Level level, const std::string &message)
    {
        cLog(__FILE__,__LINE__,CFStrings::CNect_Library_Name,
             "{0}:{1}",this->level2str(level),message);
    }
};

struct FramePacket
{
    bool expended;
    libfreenect2::Frame* color;
    libfreenect2::Frame* depth;

    libfreenect2::FrameMap raw_frames;

    Mutex access;
};

class NectPacketPool : public Coffee::PacketPool<FramePacket>
{
public:
    NectPacketPool(CSize const&)
    {
        m_dsize = CSize(512,424);
        m_csize = CSize(512,424);
    }
protected:
    void initPacket(FramePacket *p)
    {
        Lock l(p->access);
        p->color = new libfreenect2::Frame(m_csize.w,m_csize.h,4);
        p->depth = new libfreenect2::Frame(m_dsize.w,m_dsize.h,4);
    }
    bool isAvailable(FramePacket *p)
    {
        Lock l(p->access);
        return p->expended;
    }
    void cleanPacket(FramePacket *p)
    {
        p->expended = false;
    }
    void freePacket(FramePacket *p)
    {
        delete p->color;
        delete p->depth;
        delete p;
    }

    CSize m_dsize;
    CSize m_csize;
};

class NectPacketProducer : public Coffee::PacketProducer<FramePacket>
{
public:
    NectPacketProducer(CSize framesize):
        m_ppool(framesize),
        PacketProducer(m_ppool)
    {
    }
protected:
    NectPacketPool m_ppool;
};

class NectPacketConsumer : public Coffee::PacketConsumer<FramePacket>
{
public:
    NectPacketConsumer()
    {
    }
    ~NectPacketConsumer()
    {
    }

    virtual void processPacket(FramePacket *packet)
    {
        Lock l(m_packets_mutex);
        C_UNUSED(l);

        m_packets_queue.push(packet);
    }

    FramePacket* getPacket()
    {
        Lock l(m_packets_mutex);
        C_UNUSED(l);

        if(m_packets_queue.size()<1)
            return nullptr;
        FramePacket* p = m_packets_queue.front();
        m_packets_queue.pop();
        return p;
    }

protected:
    Mutex m_packets_mutex;
    std::queue<FramePacket*> m_packets_queue;
};

struct FreenectImplementation::FreenectContext
{
    FreenectContext(int index);
    ~FreenectContext();

    libfreenect2::Freenect2 manager;
    libfreenect2::Freenect2Device* device;

    libfreenect2::SyncMultiFrameListener* listener;
    libfreenect2::Registration* regist;

    NectPacketProducer packets;
    NectPacketConsumer consumer;

    std::atomic_bool active;

    std::future<void> exit_fun;

    NectLogger logger;
};

FreenectImplementation::FreenectContext::FreenectContext(int index) :
    packets(CSize(512,424)),
    device(NULL)
{
    libfreenect2::setGlobalLogger(&logger);

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

    device->start();

    regist = new libfreenect2::Registration(device->getIrCameraParams(),device->getColorCameraParams());
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
    return new FreenectImplementation::FreenectContext(index);
}

const CRGBA& SampleImage(FreenectImplementation::NectRGB const& img, const scalar& x, const scalar& y)
{
    return img.data()[(uint32)(CMath::floor(y)*img.size.h*img.size.w)+(uint32)(CMath::floor(x)*img.size.w)];
}

void FreenectImplementation::LaunchAsync(FreenectImplementation::FreenectContext *c)
{
    c->active.store(true);

    c->exit_fun = std::async(std::launch::async,[=](){

        cLog(__FILE__,__LINE__,CFStrings::CNect_Library_Name,
             CFStrings::CNect_AsyncStart);

        Profiler::PushContext("Kinect loop");
        while(c->active.load())
        {
            /* Get a packet handle */
            FramePacket* p = c->packets.getPacket();

            /* Grab a set of frames */
            c->listener->waitForNewFrame(p->raw_frames);

            libfreenect2::Frame* cf = p->raw_frames[libfreenect2::Frame::Color];
            libfreenect2::Frame* df = p->raw_frames[libfreenect2::Frame::Depth];

            /* Transform the frames */
            Profiler::Profile("Waiting for frame");

            c->regist->apply(cf,df,p->depth,p->color);

            p->depth = df;
            p->color = cf;

            /* Queue it for use on different thread */
            c->consumer.processPacket(p);

            Profiler::Profile("Processing frame");
        }
        Profiler::PopContext();
    });
}

bool FreenectImplementation::RunningAsync(FreenectImplementation::FreenectContext *c)
{
    return c->active.load();
}

bool FreenectImplementation::ProcessFrame(FreenectImplementation::FreenectContext *c,
                                          FreenectImplementation::FreenectFrameProcessor fun,
                                          size_t n)
{
    if(!c->active.load())
        return false;

    /* Retrieve a frame */
    FramePacket* p = c->consumer.getPacket();

    /* If p is NULL, there is no frame ready */
    if(!p)
    {
        return false;
    }

    /* Create wrapper frames */
    NectDepth depthframe(p->depth->width,p->depth->height,(scalar*)p->depth->data);
    NectRGB colorframe(p->color->width,p->color->height,(CRGBA*)p->color->data);

    const CRGBA& value = colorframe.data()[500];

    cDebug("Pixel value: {0},{1},{2}",value.r,value.g,value.b);

    /* Hand it over to user */
    fun(colorframe,depthframe);

    /* Recycle it */
    p->expended = true;
    c->packets.usePacket(p);

    return true;
}

void FreenectImplementation::ShutdownAsync(FreenectContext *c)
{
    /* Set atomic flag to quit */
    c->active.store(false);
    /* Wait for async job to stop */
    c->exit_fun.get();
    cLog(__FILE__,__LINE__,CFStrings::CNect_Library_Name,
         CFStrings::CNect_AsyncStop);
}

void FreenectImplementation::Free(FreenectImplementation::FreenectContext *c)
{
    delete c;
}

}
}
