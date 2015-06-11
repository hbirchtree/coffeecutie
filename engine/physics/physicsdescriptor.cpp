#include "physicsdescriptor.h"

PhysicsDescriptor::PhysicsDescriptor(QObject *parent) : QObject(parent)
{
    m_scale << 1.f << 1.f << 1.f;
    m_position << 0.f << 0.f << 0.f;
    m_orientation << 1.f << 0.f << 0.f << 0.f;
    m_linearFactor << 1.f << 1.f << 1.f;
    m_velocity << 0.f << 0.f << 0.f;
    m_acceleration << 0.f << 0.f << 0.f;
    m_normal << 0.f << 0.f << 0.f;
    m_angularVelocity << 0.f << 0.f << 0.f;
}

float PhysicsDescriptor::mass() const
{
    return m_mass;
}

float PhysicsDescriptor::friction() const
{
    return m_friction;
}

float PhysicsDescriptor::restitution() const
{
    return m_restitution;
}

PhysicsDescriptor::PhysicalShape PhysicsDescriptor::shape() const
{
    return m_shape;
}

QString PhysicsDescriptor::fileSource() const
{
    return m_fileSource;
}

QVariantList PhysicsDescriptor::scale() const
{
    return m_scale;
}

QVariantList PhysicsDescriptor::position() const
{
    return m_position;
}

QVariantList PhysicsDescriptor::orientation() const
{
    return m_orientation;
}

QVariantList PhysicsDescriptor::linearFactor() const
{
    return m_linearFactor;
}

QVariantList PhysicsDescriptor::velocity() const
{
    return m_velocity;
}

QVariantList PhysicsDescriptor::acceleration() const
{
    return m_acceleration;
}

QVariantList PhysicsDescriptor::angularVelocity() const
{
    return m_angularVelocity;
}

QVariantList PhysicsDescriptor::inertia() const
{
    return m_inertia;
}

QVariantList PhysicsDescriptor::normal() const
{
    return m_normal;
}

bool PhysicsDescriptor::activation() const
{
    return m_activation;
}

void PhysicsDescriptor::setMass(float mass)
{
    m_mass = mass;
}

void PhysicsDescriptor::setFriction(float friction)
{
    m_friction = friction;
}

void PhysicsDescriptor::setRestitution(float restitution)
{
    m_restitution = restitution;
}

void PhysicsDescriptor::setShape(PhysicsDescriptor::PhysicalShape shape)
{
    m_shape = shape;
}

void PhysicsDescriptor::setFileSource(QString fileSource)
{
    m_fileSource = fileSource;
}

void PhysicsDescriptor::setScale(QVariantList scale)
{
    if(scale.size()!=3)
        return;
    m_scale = scale;
}

void PhysicsDescriptor::setPosition(QVariantList position)
{
    if(position.size()!=3)
        return;
    m_position = position;
}

void PhysicsDescriptor::setOrientation(QVariantList orientation)
{
    if(orientation.size()!=4)
        return;
    m_orientation = orientation;
}

void PhysicsDescriptor::setLinearFactor(QVariantList linearFactor)
{
    if(linearFactor.size()!=3)
        return;
    m_linearFactor = linearFactor;
}

void PhysicsDescriptor::setVelocity(QVariantList velocity)
{
    if(velocity.size()!=3)
        return;
    m_velocity = velocity;
}

void PhysicsDescriptor::setAcceleration(QVariantList acceleration)
{
    if(acceleration.size()!=3)
        return;
    m_acceleration = acceleration;
}

void PhysicsDescriptor::setAngularVelocity(QVariantList angularVelocity)
{
    if(angularVelocity.size()!=3)
        return;
    m_angularVelocity = angularVelocity;
}

void PhysicsDescriptor::setInertia(QVariantList inertia)
{
    if(inertia.size()!=3)
        return;
    m_inertia = inertia;
}

void PhysicsDescriptor::setNormal(QVariantList normal)
{
    if(normal.size()!=3)
        return;
    m_normal = normal;
}

void PhysicsDescriptor::setActivation(bool activation)
{
    m_activation = activation;
}
