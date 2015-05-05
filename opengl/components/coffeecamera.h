#ifndef COFFEECAMERA_H
#define COFFEECAMERA_H

#include "general/common.h"
#include "general/numbercontainer.h"
#include "general/qstringfunctions.h"
class CoffeeCamera : public QObject
{
    Q_OBJECT
public:
    CoffeeCamera(QObject *parent);
    CoffeeCamera(QObject *parent,float aspect,float znear,float zfar,float fov);
    CoffeeCamera(QObject *parent, float aspect, float znear, float zfar, float fov, glm::vec3 pos, glm::quat rot);
    ~CoffeeCamera();

    QPointer<NumberContainer<glm::vec3> > getPosition();
    QPointer<NumberContainer<glm::quat>> getRotation();
    QPointer<NumberContainer<float>> getFieldOfView();
    QPointer<NumberContainer<float>> getAspect();

    void offsetOrientation(float rightAngle,float upAngle);
    void cameraLookAt(glm::vec3 point);

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

    static void normalizeEulerAngles(QPointer<NumberContainer<glm::vec3> > e, float x_min, float x_max);

    void setOrthographic(bool value);
    bool isOrthographic();

signals:

public slots:
    void clearFramebufferSizeObject();

private:
    bool orthographic = false;
    QSize* framebufferSize = NULL;
    QPointer<NumberContainer<float>> aspect;
    float znear = 0.1;
    float zfar = 50;
    glm::mat4 m_view;
    QPointer<NumberContainer<float>> fov;
    QPointer<NumberContainer<glm::vec3> > position;
    QPointer<NumberContainer<glm::quat>> rotation;
    QPointer<NumberContainer<glm::vec3>> rotation_euler;
};

#endif // COFFEECAMERA_H
