#ifndef COFFEERENDERER_H
#define COFFEERENDERER_H

#include <QThread>
#include <QVector>
#include <QDebug>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "general/vectors/vector3container.h"
#include "opengl/coffeescene.h"

class CoffeeRenderer : public QThread
{
    Q_OBJECT
public:
    explicit CoffeeRenderer(QObject *parent = 0, int w = 800, int h = 600);
    CoffeeRenderer(int w = 800, int h = 600);
    ~CoffeeRenderer();

    void setWindowDimensions(int w,int h);

    int init();
    int loop();

    //Callback functions
    void printInput(int btn, int action){
        if(btn==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)
            glfwSetWindowShouldClose(window,GL_TRUE);
        qDebug() << "INPUT: "+QString::number(btn)+" : "+QString::number(action);
    }
    void printInput(double x,double y){
        qDebug() << "MOVE: "+QString::number(x)+" : "+QString::number(y);
    }

private:
    //Settings
    int width = 1280;
    int height = 1280;
    QString windowTitle = "Café";

    //OpenGL
    glm::vec4 clearColor;

    //GLFW objects
    GLFWwindow* window;

    //Data source
    QPointer<CoffeeScene> scene;
signals:

public slots:
};

#endif // COFFEERENDERER_H
