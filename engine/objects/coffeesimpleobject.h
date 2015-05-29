#ifdef COFFEESIMPLEOBJECT_H
#define COFFEESIMPLEOBJECT_H

#include "general/common.h"
#include "engine/objects/coffeeobject.h"
#include "opengl/components/coffeematerial.h"
#include "general/data/numberbuffer.h"
#include "opengl/components/shadercontainer.h"
class CoffeeSimpleObject : public CoffeeObject
{
//    Q_PROPERTY(QString position READ getStringPosition)
//    Q_PROPERTY(QString rotation READ getStringRotation)
//    Q_PROPERTY(QString scale READ getStringScale)

public:
    virtual QPointer<CoffeeMaterial> getMaterial();
    virtual bool isBaked();
    virtual void setBaked(bool val);
    virtual QPointer<ShaderContainer> getShader();
    virtual void setShader(QPointer<ShaderContainer> shader);

protected:
    CoffeeSimpleObject(QObject *parent);

    bool baked = false;
    QPointer<CoffeeMaterial> mtl;
    QPointer<ShaderContainer> shader;

    // CoffeeObject interface
};

#endif // COFFEESIMPLEOBJECT_H
