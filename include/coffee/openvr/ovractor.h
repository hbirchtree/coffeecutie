#pragma once

#include "openvr_common.h"

namespace Coffee{
namespace OpenVRFun{

struct OVRActor : ActorTracker_def
{
    friend struct OVRDevice;
private:
    OVRDevice* m_device;
    Nodes m_hierarchy;
public:
    bool tracksHead() const;
    bool tracksHands() const;
    bool tracksFeet() const;

    void reset();

    Matf4 camera(Eye e);

    BoundBox playSpace() const;
    Nodes& spine();

    Vecf3 angularVelocity() const;
    Vecf3 velocity() const;

protected:
    /* Updates matrices */
    void devtick();

private:
    OVRActor(OVRDevice* dev);

};

}
}
