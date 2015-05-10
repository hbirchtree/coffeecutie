#ifndef GENERICPHYSICSINTERFACE_H
#define GENERICPHYSICSINTERFACE_H

#include <QObject>

class GenericPhysicsInterface
{
public:
    enum PhysicsProperty {
        PhysProp_Pos,PhysProp_Orientation,
        PhysProp_AngularVelocity,PhysProp_Gravity,
        PhysProp_Velocity,PhysProp_Force,
        PhysProp_Activation
    };
    virtual QString systemName();

private:
    GenericPhysicsInterface(QObject *parent);
};

#endif // GENERICPHYSICSINTERFACE_H
