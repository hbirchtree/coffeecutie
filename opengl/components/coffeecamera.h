#ifndef COFFEECAMERA_H
#define COFFEECAMERA_H

#include "general/common.h"
#include "general/vectors/vector3container.h"
#include "general/floatcontainer.h"
class CoffeeCamera : public QObject
{
    Q_OBJECT
public:
    CoffeeCamera(QObject *parent);
    CoffeeCamera(QObject *parent,float aspect,float znear,float zfar,float fov);
    CoffeeCamera(QObject *parent,float aspect,float znear,float zfar,float fov,glm::vec3 pos,glm::vec3 rot);
    ~CoffeeCamera();

    QPointer<Vector3Container> getPosition();
    QPointer<Vector3Container> getRotation();
    QPointer<FloatContainer> getFieldOfView();
    QPointer<FloatContainer> getAspect();

    void offsetOrientation(float rightAngle,float upAngle);
    void cameraLookAt(glm::vec3 point);

    void normalizeOrientation();

    glm::vec3 getCameraRight() const;
    glm::vec3 getCameraForward() const;
    glm::vec3 getCameraUp() const;

    glm::vec3 getCameraRightNormal() const;
    glm::vec3 getCameraUpNormal() const;
    glm::vec3 getCameraForwardNormal() const;

    glm::mat4 getOrientationMatrix() const;
    glm::mat4 getProjection() const;
    glm::mat4 getOrthographic() const;

    glm::mat4 getMatrix() const;

    void setFramebufferSizeObject(QSize* fb);

    void setOrthographic(bool value);
    bool isOrthographic();

signals:

public slots:
    void clearFramebufferSizeObject();

private:
    bool orthographic = false;
    const float math_pi = 3.141592654;
    QSize* framebufferSize = NULL;
    QPointer<FloatContainer> aspect;
    float znear = 0.1;
    float zfar = 50;
    QPointer<FloatContainer> fov;
    QPointer<Vector3Container> position;
    QPointer<Vector3Container> rotation;
};

#endif // COFFEECAMERA_H
