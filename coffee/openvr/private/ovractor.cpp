#include <coffee/openvr/ovractor.h>

#include <coffee/core/types/cdef/geometry.h>

#include "ovr_private.h"

namespace Coffee{
namespace OpenVRFun{

OVRController::OVRController(OVRDevice *dev) :
    HWDeviceInfo("HTC","Vive","1.0","0000")
{
}

OVRActor::OVRActor(OVRDevice *dev):
    m_device(dev),
    m_controller(dev)
{
    m_hierarchy.createNode(Spine,0);

    m_hierarchy.createNode(Head,Spine);

    m_hierarchy.createNode(LeftEye,Spine);
    m_hierarchy.createNode(RightEye,Spine);

    m_hierarchy.createNode(LeftHand,Spine);
    m_hierarchy.createNode(RightHand,Spine);

    m_hierarchy.createNode(LeftFoot,Spine);
    m_hierarchy.createNode(RightFoot,Spine);

    uint32 state;
    m_hierarchy.validateTree(&state);

    if(state != 0)
    {
        cWarning("Hierarchy creation failed: {0}",state);
    }
}

bool OVRActor::tracksHead() const
{
    return true;
}

bool OVRActor::tracksHands() const
{
    return true;
}

bool OVRActor::tracksFeet() const
{
    return false;
}

void OVRActor::reset()
{
}

Matf4 OVRActor::camera(ActorTracker_def::Eye e)
{
    return Matf4();
}

BoundBox OVRActor::playSpace() const
{
    BoundBox space(0,0,0);

    return space;
}

Nodes &OVRActor::spine()
{
    return m_hierarchy;
}

Vecf3 OVRActor::angularVelocity() const
{
    if(!m_device->isConnected())
        return CVec3();

    const vr::HmdVector3_t& angle = m_Context->devicePoses[m_device->m_dIndex].vAngularVelocity;

    return CVec3(angle.v[0],angle.v[1],angle.v[2]);
}

Vecf3 OVRActor::velocity() const
{
    if(!m_device->isConnected())
        return CVec3();
    const vr::HmdVector3_t& vel = m_Context->devicePoses[m_device->m_dIndex].vVelocity;

    return CVec3(vel.v[0],vel.v[1],vel.v[2]);
}

const OVRController &OVRActor::controller() const
{
    return m_controller;
}

const Matf4 &OVRActor::head()
{
    return *m_hierarchy.node(Head).transform;
}

}
}
