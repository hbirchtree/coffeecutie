#ifndef PHYSICSDESCRIPTOR_H
#define PHYSICSDESCRIPTOR_H

#include "general/common.h"
class PhysicsDescriptor : public QObject
{

    //This will be used for all the information that PhysicsObject doesn't need later on in the process, such as mass, physical scale and etc. These are only useful for initializing new bodies and shapes.
    Q_PROPERTY(float mass READ mass WRITE setMass)
    Q_PROPERTY(float friction READ friction WRITE setFriction)
    Q_PROPERTY(float restitution READ restitution WRITE setRestitution)
    Q_PROPERTY(QString fileSource READ fileSource WRITE setFileSource)
    Q_PROPERTY(PhysicalShape shape READ shape WRITE setShape)

    Q_PROPERTY(QVariantList scale READ scale WRITE setScale)
    Q_PROPERTY(QVariantList position READ position WRITE setPosition)
    Q_PROPERTY(QVariantList orientation READ orientation WRITE setOrientation)

    Q_PROPERTY(QVariantList angularVelocity READ angularVelocity WRITE setAngularVelocity)
    Q_PROPERTY(QVariantList inertia READ inertia WRITE setInertia)
    Q_PROPERTY(QVariantList normal READ normal WRITE setNormal)

    Q_PROPERTY(QVariantList linearFactor READ linearFactor WRITE setLinearFactor)
    Q_PROPERTY(QVariantList velocity READ velocity WRITE setVelocity)
    Q_PROPERTY(QVariantList acceleration READ acceleration WRITE setAcceleration)

    Q_PROPERTY(bool activation READ activation WRITE setActivation)

    Q_OBJECT
public:
    enum PhysicalShape {
        Shape_None,

        //Primitives
        Shape_Sphere,Shape_Box,Shape_Cylinder,Shape_Capsule,Shape_Cone,

        //Meshes -
        /*
         * The *Bt ones loads from .bullet files. Some of these
         * formats can consume a lot of time being configured from other formats.
         * It is left as an option here.
         */
        Shape_TriMesh, Shape_TriMeshBt,

        Shape_ConvexHull, Shape_ConvexHullBt,

        Shape_Heightmap, Shape_HeightmapBt,

        Shape_StaticPlane,

        //Compound
        Shape_Compund //Loaded from .bullet file by default
    };
    Q_ENUMS(PhysicalShape)

    PhysicsDescriptor(QObject* parent);

    float mass() const;
    float friction() const;
    float restitution() const;

    PhysicalShape shape() const;

    QString fileSource() const;

    QVariantList scale() const;
    QVariantList position() const;
    QVariantList orientation() const;
    QVariantList linearFactor() const;
    QVariantList velocity() const;
    QVariantList acceleration() const;
    QVariantList angularVelocity() const;
    QVariantList inertia() const;
    QVariantList normal() const;

    bool activation() const;

public slots:

    void setMass(float mass);
    void setFriction(float friction);
    void setRestitution(float restitution);

    void setShape(PhysicsDescriptor::PhysicalShape shape);

    void setFileSource(QString fileSource);

    void setScale(QVariantList scale);
    void setPosition(QVariantList position);
    void setOrientation(QVariantList orientation);
    void setLinearFactor(QVariantList linearFactor);
    void setVelocity(QVariantList velocity);
    void setAcceleration(QVariantList acceleration);
    void setAngularVelocity(QVariantList angularVelocity);
    void setInertia(QVariantList inertia);
    void setNormal(QVariantList normal);

    void setActivation(bool activation);

private:
    float m_mass = 0.f;
    float m_friction = 1.f;
    float m_restitution = 1.f;
    PhysicalShape m_shape = Shape_None;
    QString m_fileSource;
    QVariantList m_scale;
    QVariantList m_position;
    QVariantList m_orientation;
    QVariantList m_linearFactor;
    QVariantList m_velocity;
    QVariantList m_acceleration;
    QVariantList m_angularVelocity;
    QVariantList m_inertia;
    QVariantList m_normal;
    bool m_activation = false;
};

#endif // PHYSICSDESCRIPTOR_H
