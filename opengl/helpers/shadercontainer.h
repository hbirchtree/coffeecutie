#ifndef SHADERCONTAINER_H
#define SHADERCONTAINER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QDebug>
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "glext.h"
#include "glcorearb.h"
#include "general/filehandler.h"

class ShaderContainer : public QObject
{
    Q_OBJECT
public:
    explicit ShaderContainer(QObject *parent = 0);
    ~ShaderContainer();

    int buildProgram(QString verShaderFile,QString fragShaderFile);
    int compileShader(QString shaderFile,int shaderType);
    int getProgramId();
signals:

public slots:

private:
    int programId = 0;

    QHash<QString,int> attributes;
    QHash<QString,int> uniforms;
};

#endif // SHADERCONTAINER_H
