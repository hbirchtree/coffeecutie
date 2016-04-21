#pragma once

#include "openvr_common.h"

namespace Coffee{
namespace OpenVRFun{

struct OVRController : HWDeviceInfo
{
    friend struct OVRImpl;

    OVRController(OVRDevice* dev);

    using Stick = Vecf2;

private:
    Vector<bool> m_buttons;
    Vector<Stick> m_pads;

public:
    Vector<bool> const& buttons() const
    {
        return m_buttons;
    }
    Vector<Stick> const& pads() const
    {
        return m_pads;
    }
};

struct OVRActor : ActorTracker_def
{
    friend struct OVRImpl;
    friend struct OVRDevice;

private:
    OVRDevice* m_device;
    Nodes m_hierarchy;
    OVRController m_controller;
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

    OVRController const& controller() const;

    Matf4 const& head();

protected:
    /* Updates matrices and others */
    void devtick();

private:
    OVRActor(OVRDevice* dev);
    C_DELETE_COPY_CONSTRUCTOR(OVRActor);
};

}
}
