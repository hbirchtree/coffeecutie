#ifndef SHADERCONTAINER_H
#define SHADERCONTAINER_H

#include <QObject>

class ShaderContainer : public QObject
{
    Q_OBJECT
public:
    explicit ShaderContainer(QObject *parent = 0);
    ~ShaderContainer();

signals:

public slots:
};

#endif // SHADERCONTAINER_H
