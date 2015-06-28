#ifndef COFFEECAMERA_H
#define COFFEECAMERA_H

#include "general/common.h"
#include "engine/scripting/qscriptvectorvalue.h"

class ShaderVariant;

class CoffeeCamera : public QObject
{
    Q_PROPERTY(float zNear READ getZnear WRITE setZnear)
    Q_PROPERTY(float zFar READ getZfar WRITE setZfar)
    Q_PROPERTY(float fov READ getFov WRITE setFov)
    Q_PROPERTY(float aspectRatio READ getAspectValue WRITE setAspect)
    Q_PROPERTY(bool orthographic READ isOrthographic WRITE setOrthographic)

    Q_PROPERTY(QObject* position READ getPositionValue)
    Q_PROPERTY(QObject* rotation READ getRotationValue)

    Q_PROPERTY(QObject* matrixVP READ getMatrixVPVariant)
    Q_PROPERTY(QObject* matrix READ getMatrixVariant)
    Q_PROPERTY(QObject* right READ getCameraRightVariant)
    Q_PROPERTY(QObject* forward READ getCameraForwardVariant)
    Q_PROPERTY(QObject* positionVariant READ getCameraPositionVariant)

    Q_OBJECT
public:
    CoffeeCamera(QObject *parent);
    CoffeeCamera(QObject *parent,float aspect,float znear,float zfar,float fov);
    CoffeeCamera(QObject *parent, float aspect, float znear, float zfar, float fov, glm::vec3 pos, glm::vec3 rot);
    ~CoffeeCamera();

    QPointer<Vector3Value> getPosition();
    QPointer<Vector3Value> getRotation();
    QPointer<ScalarValue> getFieldOfView();
    QPointer<ScalarValue> getAspect();

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
    glm::mat4 getPerspective() const;

    glm::mat4 getMatrix() const;

    void setFramebufferSizeObject(QSize* fb);

    static void normalizeEulerAngles(QPointer<Vector3Value> e, float x_min, float x_max);

    bool isOrthographic() const;
    float getZnear() const;
    float getZfar() const;
    float getAspectValue() const;
    float getFov() const;

    QObject* getMatrixVPVariant() const;
    QObject* getMatrixVariant() const;
    QObject* getPositionValue();
    QObject* getRotationValue();

    QObject* getCameraRightVariant() const;
    QObject* getCameraForwardVariant() const;
    QObject* getCameraPositionVariant() const;

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
    QPointer<ScalarValue> aspect;
    float znear = 0.1;
    float zfar = 50;
    glm::mat4 m_view;
    QPointer<ScalarValue> fov;
    QPointer<Vector3Value> position;
    QPointer<QuatValue> orientation;
    QPointer<Vector3Value> rotation_euler;

    ShaderVariant* matrixVPVariant;
    ShaderVariant* matrixVariant;
    ShaderVariant* cameraRightVariant;
    ShaderVariant* cameraForwardVariant;
    ShaderVariant* cameraPosVariant;
};

#endif // COFFEECAMERA_H
