#ifndef COFFEEPLAYERCONTROLLER_H
#define COFFEEPLAYERCONTROLLER_H

#include "general/common.h"
#include "general/data/numbercontainer.h"
#include "general/qstringfunctions.h"
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
    void addRotation(glm::vec3 d);
    void addSpeed(glm::vec3 d);
    void addAccel(glm::vec3 d);
    void tick(float time);

private:
    NumberContainer<glm::vec3> *position;
    NumberContainer<glm::vec3> *rotation;
};

#endif // COFFEEPLAYERCONTROLLER_H
