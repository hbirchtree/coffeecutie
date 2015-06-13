#ifndef COFFEECAMERA_H
#define COFFEECAMERA_H

#include "general/common.h"
#include "general/data/numbercontainer.h"

class VectorValue;

class CoffeeCamera : public QObject
{
    Q_PROPERTY(float zNear READ getZnear WRITE setZnear)
    Q_PROPERTY(float zFar READ getZfar WRITE setZfar)
    Q_PROPERTY(float fov READ getFov WRITE setFov)
    Q_PROPERTY(float aspectRatio READ getAspectValue WRITE setAspect)
    Q_PROPERTY(bool orthographic READ isOrthographic WRITE setOrthographic)
    Q_PROPERTY(QObject* position READ getPositionValue)
    Q_PROPERTY(QObject* rotation READ getRotationValue)

    Q_PROPERTY(QObject* matrix READ getMatrixVariant)

    Q_OBJECT
public:
    CoffeeCamera(QObject *parent);
    CoffeeCamera(QObject *parent,float aspect,float znear,float zfar,float fov);
    CoffeeCamera(QObject *parent, float aspect, float znear, float zfar, float fov, glm::vec3 pos, glm::vec3 rot);
    ~CoffeeCamera();

    QPointer<NumberContainer<glm::vec3> > getPosition();
    QPointer<NumberContainer<glm::vec3>> getRotation();
    QPointer<NumberContainer<float>> getFieldOfView();
    QPointer<NumberContainer<float>> getAspect();

    void cameraLookAt(glm::vec3 point);

    glm::vec3 getCameraRight() const;
    glm::vec3 getCameraForward() const;
    glm::vec3 getCameraUp() const;

    glm::vec3 getCameraRightNormal() const;
    glm::vec3 getCameraUpNormal() const;
    glm::vec3 getCameraForwardNormal() const;

    glm::vec3 getCameraPos() const;

    float getCameraForwardDirection() const;

    glm::mat4 getOrientationMatrix() const;
    glm::mat4 getProjection() const;
    glm::mat4 getOrthographic() const;

    glm::mat4 getMatrix() const;

    void setFramebufferSizeObject(QSize* fb);

    static void normalizeEulerAngles(QPointer<NumberContainer<glm::vec3> > e, float x_min, float x_max);

    bool isOrthographic() const;
    float getZnear() const;
    float getZfar() const;
    float getAspectValue() const;
    float getFov() const;

    QObject* getMatrixVariant() const;
    QObject* getPositionValue();
    QObject* getRotationValue();

signals:

public slots:
    void clearFramebufferSizeObject();

    void setFov(float fov);
    void setAspect(float value);
    void setZfar(float value);
    void setZnear(float value);
    void setOrthographic(bool value);
    void offsetOrientation(float rightAngle,float upAngle);

private:
    bool orthographic = false;
    QSize* framebufferSize = NULL;
    QPointer<NumberContainer<float>> aspect;
    float znear = 0.1;
    float zfar = 50;
    glm::mat4 m_view;
    QPointer<NumberContainer<float>> fov;
    QPointer<NumberContainer<glm::vec3> > position;
    QPointer<NumberContainer<glm::quat>> orientation;
    QPointer<NumberContainer<glm::vec3>> rotation_euler;

    VectorValue* posWrapper;
    VectorValue* rotWrapper;
};

#endif // COFFEECAMERA_H
