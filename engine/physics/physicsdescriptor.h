#ifndef PHYSICSDESCRIPTOR_H
#define PHYSICSDESCRIPTOR_H

#include "general/common.h"
class PhysicsDescriptor
{
    //This will be used for all the information that PhysicsObject doesn't need later on in the process, such as mass, physical scale and etc. These are only useful for initializing new bodies and shapes.
    Q_PROPERTY(float mass READ mass WRITE setMass)
    Q_PROPERTY(glm::vec3 position READ position WRITE setPosition)
    Q_PROPERTY(glm::quat orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(glm::vec3 normal READ normal WRITE setnormal)
    Q_PROPERTY(glm::vec3 scale READ scale WRITE setScale)
    Q_PROPERTY(float friction READ friction WRITE setFriction)
    Q_PROPERTY(float restitution READ restitution WRITE setRestitution)
    Q_PROPERTY(glm::vec3 inertia READ inertia WRITE setInertia)
    Q_PROPERTY(glm::vec3 linearFactor READ linearFactor WRITE setLinearFactor)
    Q_PROPERTY(bool activation READ activation WRITE setActivation)
    Q_PROPERTY(bool updateRotation READ updateRotation WRITE setUpdateRotation)
    Q_PROPERTY(bool updatePosition READ updatePosition WRITE setUpdatePosition)
    Q_PROPERTY(QByteArray* fileSource READ fileSource WRITE setFileSource)

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

    PhysicsDescriptor();
    QByteArray* fileSource() const
    {
        return m_fileSource;
    }
    bool updatePosition() const
    {
        return m_updatePosition;
    }

    bool updateRotation() const
    {
        return m_updateRotation;
    }

    bool activation() const
    {
        return m_activation;
    }

    glm::vec3 linearFactor() const
    {
        return m_linearFactor;
    }

    glm::vec3 inertia() const
    {
        return m_inertia;
    }

    float restitution() const
    {
        return m_restitution;
    }

    float friction() const
    {
        return m_friction;
    }

    glm::vec3 scale() const
    {
        return m_scale;
    }

    glm::quat orientation() const
    {
        return m_orientation;
    }

    glm::vec3 position() const
    {
        return m_position;
    }

    float mass() const
    {
        return m_mass;
    }


    PhysicalShape getShape() const
    {
        return shape;
    }
    void setShape(const PhysicalShape &value);

    glm::vec3 normal() const
    {
        return m_normal;
    }

public slots:
    void setFileSource(QByteArray* fileSource)
    {
        m_fileSource = fileSource;
    }
    void setUpdatePosition(bool updatePosition)
    {
        m_updatePosition = updatePosition;
    }

    void setUpdateRotation(bool updateRotation)
    {
        m_updateRotation = updateRotation;
    }

    void setActivation(bool activation)
    {
        m_activation = activation;
    }

    void setLinearFactor(glm::vec3 linearFactor)
    {
        m_linearFactor = linearFactor;
    }

    void setInertia(glm::vec3 inertia)
    {
        m_inertia = inertia;
    }

    void setRestitution(float restitution)
    {
        m_restitution = restitution;
    }

    void setFriction(float friction)
    {
        m_friction = friction;
    }

    void setScale(glm::vec3 scale)
    {
        m_scale = scale;
    }

    void setOrientation(glm::quat orientation)
    {
        m_orientation = orientation;
    }

    void setPosition(glm::vec3 position)
    {
        m_position = position;
    }

    void setMass(float mass)
    {
        m_mass = mass;
    }

    void setnormal(glm::vec3 normal)
    {
        if (m_normal == normal)
            return;

        m_normal = normal;
    }

private:
    PhysicalShape shape = Shape_None;
    QByteArray* m_fileSource;
    bool m_updatePosition;
    bool m_updateRotation;
    bool m_activation;
    glm::vec3 m_linearFactor;
    glm::vec3 m_inertia;
    float m_restitution;
    float m_friction;
    glm::vec3 m_scale;
    glm::quat m_orientation;
    glm::vec3 m_position;
    float m_mass;
    glm::vec3 m_normal;
};

#endif // PHYSICSDESCRIPTOR_H
