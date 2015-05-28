#ifndef VAOHELPER_H
#define VAOHELPER_H

#include "general/common.h"
#include "engine/models/coffeemesh.h"

class VAOHelper
{
public:
//    static void genVAO(RenderableObject* object,int vertLocation,int vertTexCoordLocation,int vertNormalLocation,int vertTangentLocation);
    static void genTangents(QList<QPointer<CoffeeVertex> > &vertices);
    static void modifyVbo(GLint vboId,QPointer<CoffeeMesh> mesh);
    static void generateIndices(QPointer<CoffeeMesh> mesh);
};

#endif // VAOHELPER_H
