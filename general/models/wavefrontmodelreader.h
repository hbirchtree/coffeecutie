#ifndef WAVEFRONTMODELREADER
#define WAVEFRONTMODELREADER

#include "general/common.h"
#include "general/filehandler.h"
#include "engine/models/coffeemesh.h"
#include "opengl/rendering/coffeevertex.h"
#include "modelreaderinterface.h"
#include "opengl/components/coffeematerial.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/helpers/vaohelper.h"
class WavefrontModelReader : public QObject{

public:
    typedef ModelReaderInterface::ModelContainer ModelContainer;

    WavefrontModelReader(QObject* parent);
    ~WavefrontModelReader();

    static QList<int> parseStrVec(QString src, QChar sep);
    static glm::vec2 parseStrVec2f(QString src, QChar sep);
    static glm::vec3 parseStrVec3f(QString src, QChar sep);
    QHash<QString, QPointer<CoffeeMaterial> > getMaterials();
    QHash<QString, QPointer<ModelContainer> > parseModel(QString file);

    void clearData();
private:
    QHash<QString,QPointer<CoffeeMaterial> > materials;
    QHash<QString,QPointer<ModelContainer> > models;
    void parseMtlFile(QString file);
};

#endif // WAVEFRONTMODELREADER

