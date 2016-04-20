#include <coffee/core/CUnitTesting>

#include <coffee/openvr/copenvr.h>

using namespace Coffee;
using namespace HMD;

using API = OpenVR;

using Actor = API::Actor;
using Device = API::Device;

bool interface_test()
{
    Device* dev = QuickLoadHMD<API,Device>();

    if(!dev)
        return false;

    API::GetDriverInfo();
    API::GetRuntimeInfo();
    API::IsDummy();

    dev->startFrame();
    dev->endFrame();
    dev->getLenses();
    dev->deviceInfo();
    dev->firmwareInfo();
    dev->isConnected();
    dev->windowCanvas();

    Actor actor = dev->actor();

    actor.tracksHead();
    actor.tracksHands();
    actor.tracksFeet();

    actor.camera(Actor::Eye::Left);
    actor.reset();
    actor.angularVelocity();
    actor.playSpace();
    actor.spine();
    {
        cDebug("Spine: {0}",
               *actor.spine().root().transform);
        cDebug("Left hand: {0}",
               *actor.spine().node(Actor::LeftHand).transform);
    }
    actor.tracksHands();
    actor.tracksHead();
    actor.velocity();

    auto ctxt = API::GetContext();
    API::SetContext(ctxt);
    API::GetConstContext();
    API::Shutdown();

    return true;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {interface_test},
};

COFFEE_RUN_TESTS(_tests);
