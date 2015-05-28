#ifndef MODELREADERINTERFACE_H
#define MODELREADERINTERFACE_H

#include "general/common.h"
#include "opengl/components/coffeematerial.h"
#include "engine/models/coffeemesh.h"
class ModelReaderInterface
{
public:
    class ModelContainer : public QObject{
    public:
        ModelContainer(QObject* parent) : QObject(parent){}
        ~ModelContainer(){

        }
        QString mdlName;
        QPointer<CoffeeMesh> model;
        QString mtlName;
        QPointer<CoffeeMaterial> material;

        QVector<glm::vec3> vertices;
        QVector<glm::vec2> txcoords;
        QVector<glm::vec3> vnormals;
        QVector<QStringList> faces;
    };
};

#endif // MODELREADERINTERFACE_H
