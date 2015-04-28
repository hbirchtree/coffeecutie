#ifndef MODELREADERINTERFACE_H
#define MODELREADERINTERFACE_H

#include "general/common.h"
#include "opengl/components/coffeematerial.h"
#include "opengl/rendering/coffeemesh.h"
class ModelReaderInterface
{
public:
    class ModelContainer : public QObject{
    public:
        QString mdlName;
        QPointer<CoffeeMesh> model;
        QString mtlName;
        QPointer<CoffeeMaterial> material;

        QList<glm::vec3> vertices;
        QList<glm::vec2> txcoords;
        QList<glm::vec3> vnormals;
        QList<QStringList> faces;
    };
};

#endif // MODELREADERINTERFACE_H
