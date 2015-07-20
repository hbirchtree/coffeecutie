#ifndef COFFEESKYBOX_H
#define COFFEESKYBOX_H

#include "coffeeobject.h"
#include "engine/shaders/coffeeuniformsetter.h"

class CoffeeMesh;
class CoffeeCamera;
class CoffeeShader;
class CoffeeTexture;
class CoffeeResource;

class CoffeeSkybox : public CoffeeObject
{
    Q_PROPERTY(QObject* texture READ texture WRITE setTexture)
    Q_PROPERTY(QObject* mesh READ mesh WRITE setMesh)
    Q_PROPERTY(QObject* shader READ shader WRITE setShader)
    Q_PROPERTY(QObject* camera READ camera WRITE setCamera)

    Q_PROPERTY(QObject* skyboxMatrix READ skyboxMatrix)

    Q_OBJECT
public:
    CoffeeSkybox(QObject* parent = 0, CoffeeCamera *m_camera = 0);

    void addMap(GLenum side, CoffeeResource *source);

    void setCamera(CoffeeCamera* m_camera);

    QPointer<CoffeeTexture> getTexture() const;
    void setTexture(CoffeeTexture* value);

    QPointer<CoffeeMesh> getSkymesh() const;
    void setSkymesh(CoffeeMesh* value);

    QPointer<CoffeeShader> getShader() const;
    void setShader(CoffeeShader *value);

    QObject* camera() const;
    QObject* shader() const;
    QObject* mesh() const;
    QObject* texture() const;

    QObject* skyboxMatrix();

public slots:
    void render();
    void unload();
    void load();

    void setMesh(QObject* mesh);
    void setShader(QObject* shader);
    void setCamera(QObject* camera);
    void setTexture(QObject* texture);

protected:
    QPointer<CoffeeCamera> m_camera;
    QPointer<CoffeeTexture> m_texture;
    QPointer<CoffeeMesh> m_mesh;

    Matrix4Value* m_matrix;

    GLuint buffs[2];
    GLuint vao;
};

#endif // COFFEESKYBOX_H
