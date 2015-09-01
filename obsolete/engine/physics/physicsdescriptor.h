#ifndef PHYSICSDESCRIPTOR_H
#define PHYSICSDESCRIPTOR_H

#include "general/common.h"
class PhysicsDescriptor : public QObject
{
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

public slots:

private:
};

#endif // PHYSICSDESCRIPTOR_H
