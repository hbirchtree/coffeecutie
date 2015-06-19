#ifndef COFFEEPLAYERCONTROLLER_H
#define COFFEEPLAYERCONTROLLER_H

#include "general/common.h"
#include "general/data/numbercontainer.h"
#include "engine/scripting/qscriptvectorvalue.h"

class CoffeePlayerController : public QObject
{
    //Describes the input that is to be performed
    Q_OBJECT
public:
    CoffeePlayerController(QObject *parent);
    ~CoffeePlayerController();

signals:
    void movePlayer(glm::vec4 d);
    void rotateCamera(glm::vec3 d);
    void moveCamera(glm::vec4 d);

public slots:
    void setRotation(glm::vec3 r);

    void joystickSetRotationPitch(float p);
    void joystickSetRotationYaw(float p);

    void mouseSetRotation(float pitch, float yaw);
    void mouseSetRotationPointF(const QPointF &offset);

    void addRotation(glm::vec3 d);
    void addSpeedForward(glm::vec3 d);
    void addSpeedRight(glm::vec3 d);
    void addAccel(glm::vec3 d);

    void addSpeedForward(VectorValue* d);
    void addSpeedRight(VectorValue* d);

    void addSpeedForward(QObject* d, const QVariantList &factor);
    void addSpeedRight(QObject* d, const QVariantList &factor);

    void tick(float time);

private:
    float mouseSensitivity = 0.1f;
    float joystickSensitivity = 1.f;

    //Because it's easy
    glm::vec3 forwardMove;
    glm::vec3 rightMove;

    NumberContainer<glm::vec3> *position;
    NumberContainer<glm::vec3> *rotation;
};

#endif // COFFEEPLAYERCONTROLLER_H
